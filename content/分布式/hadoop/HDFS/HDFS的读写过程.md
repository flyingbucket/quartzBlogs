## 相关的 Java 类与 IO 对象

### 抽象层次

- **`org.apache.hadoop.fs.FileSystem`**
    
    - Hadoop 的文件系统抽象类。
        
    - 本地文件系统是 `LocalFileSystem`。
        
    - HDFS 是 `DistributedFileSystem`。
        
- **`org.apache.hadoop.fs.Path`**
    
    - 表示 HDFS 的路径对象。
        

### 输入类（读）

- **`FSDataInputStream`**
    
    - Hadoop 的输入流，支持 `seek()`。
        
    - 内部通过 `DFSInputStream` 管理与 DataNode 的通信。
        
    - 类似 `java.io.DataInputStream`，但能随机定位。
        

### 输出类（写）

- **`FSDataOutputStream`**
    
    - Hadoop 的输出流，支持写入。
        
    - 内部用 `DFSOutputStream` 来分片、分块、写 pipeline。
        
    - 类似 `java.io.DataOutputStream`。
        
## HDFS 读流程 (Read Path)

1. **客户端请求文件**
    
    - 客户端通过 `FileSystem fs = FileSystem.get(conf)` 获取一个 `DistributedFileSystem` 实例。
        
    - 执行 `FSDataInputStream in = fs.open(new Path("/path/file.txt"))`。
        
    - 内部会先通过 RPC([[HDFS体系结构#Client ↔ NameNode]])向 **NameNode** 请求这个文件的 **元数据**([[HDFS概述#名称节点（NameNode）]])（block 列表、每个 block 存在哪些 DataNode）。
        
2. **获取数据块位置**
    
    - NameNode 返回：文件的 block 信息（blockID、偏移量、长度、DataNode 地址列表）。
        
    - 相同的block有存储冗余，客户端会根据块所在的DataNode的距离、负载状况等选择其中合适的一份来读取。
        
3. **与 DataNode 通信**
    
    - 客户端直接与 DataNode 建立 连接进行读取[[[HDFS体系结构#Client ↔ DataNode]]。
        
    - 通过 `BlockReader` 类来逐块读取数据。
        
    - **FSDataInputStream** 支持 `seek()`，所以可以随机访问大文件的任意偏移。
        

## HDFS 写流程 (Write Path)

1. **客户端请求创建文件**
    
    - 调用 `FSDataOutputStream out = fs.create(new Path("/path/file.txt"))`。
        
    - 客户端通过 RPC[[HDFS体系结构#Client ↔ NameNode]] 向 **NameNode** 申请在命名空间中新建文件。
        
    - NameNode 检查是否存在、权限是否允许，然后返回是否可写。
        
2. **数据切分为 block**
    
    - 客户端将数据写入 `DFSOutputStream`，它会把数据拆分成 **block**。
        
    - 每个 block 默认大小 128MB（`dfs.blocksize` 可配置）。
        
3. **NameNode 分配 DataNode 管道**
    
    - 客户端向 NameNode 请求：我要写下一个 block。
        
    - NameNode 返回一个 **DataNode 列表**（如副本数=3，返回3个节点地址）。
        
    - 客户端建立一个 pipeline：client → DN1 → DN2 → DN3。[[HDFS体系结构#Client ↔ DataNode]]
        
4. **流水线写入**
    
    - 客户端先把数据流送到第一个 DataNode。
        
    - DN1 写本地后再转发给 DN2，DN2 再转发 DN3。
        
    - 每个 DataNode 都会返回 ACK，最终确认写成功。
        
5. **关闭文件**
    
    - 客户端调用 `out.close()`。
        
    - DFSOutputStream 通知 NameNode 文件写完。
        
    - NameNode 将最终的文件元数据标记为“完成”，之后其他客户端可以读。

### ## 二、**写流程中的 editlog 更新**

写入时 NameNode 要维护 **命名空间一致性**，它的 editlog（增量日志）记录了对文件系统的元数据修改。

流程可以分两类更新：

1. **文件创建时**
    
    - 客户端 `create("/path/file")` → RPC 到 NameNode
        
    - NameNode 在内存元数据中加一个“正在写”的文件条目
        
    - 并立即写入一条 **OP_ADD (新增文件)** 到 editlog
        
2. **分配 block 时**
    
    - 客户端缓存数据达到 block 大小或 `close()` 时 → 请求 NameNode 分配新 block
        
    - NameNode 在内存中给文件分配一个新的 blockID，并决定副本放在哪些 DataNode
        
    - NameNode 写入一条 **OP_ADD_BLOCK (增加 block)** 到 editlog
        
    - 然后把 DataNode 列表返回给客户端
        
    - 客户端才去和 DataNode 建 pipeline，传输数据
        
3. **文件关闭时**
	- `DFSOutputStream`管理的pipeline会收集所有DataNode的写入ACK,得到所有ACK后返回给客户端
    
    - 客户端 `close()` → RPC 到 NameNode
        
    - NameNode 把该文件标记为 **complete**
        
    - 写入一条 **OP_CLOSE (关闭文件)** 到 editlog
        


#### **关键点**

- **DataNode pipeline 传输**  
    DataNode 之间的复制、校验、确认 **不涉及 NameNode**，完全是客户端和 DataNode 之间协作完成的。  
    NameNode 只在“分配 block、完成 block”时写 editlog。
    
- **editlog 内容**  
    editlog 是纯元数据操作日志，**不记录实际数据内容**。  
    它记录的操作包括：文件创建、删除、添加 block、副本位置、关闭文件等。
    
- **最终一致性**  
    - 客户端写完数据后，只有在 `close()` 之后，NameNode 才把文件标记为完成（否则文件一直是“正在写”状态）。  这时 editlog 里已经有了完整的 **create + addBlock + close** 三类记录。
	- 如果中途客户端宕机，NameNode也可以通过周期性心跳和block report向NomaNode汇报。

