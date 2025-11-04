## 命名空间管理

- HDFS的命名空间包含目录、文件和块
    
- 在HDFS1.0体系结构中，在整个HDFS集群中只有一个命名空间，并且只有唯一一个名称节点，该节点负责对这个命名空间进行管理
    
- HDFS使用的是传统的分级文件体系，因此，用户可以像使用普通文件系统一样，创建、删除目录和文件，在目录间转移文件，重命名文件等

## 通信协议

### Client ↔ NameNode

#### 协议

- **Hadoop RPC**（`org.apache.hadoop.ipc` 提供）
    
- NameNode 实现了 **ClientProtocol 接口**（Java），客户端通过 RPC Stub 调用 NameNode 方法。
    

#### 典型调用

- `create()`：请求新建文件
    
- `addBlock()`：申请新的 block 并返回 DataNode 列表
    
- `getBlockLocations()`：查询某个文件的 block 分布
    
- `complete()`：关闭文件，标记完成
    

#### 特点

- 基于 **TCP**
    
- 默认用 **protobuf 序列化**
    
- 端口：在`core-site.xml` 中使用 `fs.defaultFS=hdfs://namenode:9000`进行配置，默认是9000


### Client ↔ DataNode

#### 协议

- **DataTransferProtocol**
    
- 专门用于数据读写，走 **TCP**（默认端口 **50010**，在 docker 环境里可能映射到外部的 9866/9864）。
    

#### 典型场景

- 读流程

	1. 客户端请求一个 block（block ID + token + 偏移）。
	    
	2. DataNode 返回数据流，采用 **packet-based streaming**：
	    
	    - 每个 packet 包含若干个 chunk + 校验和。
	        
	    - 客户端校验后确认。
        
- 写流程

	1. 客户端先联系 pipeline 第一个 DN，发送写 block 请求。
	    
	2. DN1 接收后写本地，同时转发给 DN2；DN2 再转发给 DN3 …
	    
	3. 最后 ACK 按链路逆序返回 → 客户端。
	    

#### 特点

- **高吞吐流式传输**
    
- 使用 **校验和 (CRC32)** 确保数据完整性。
    


### DataNode ↔ NameNode

这部分不涉及文件数据，而是 **心跳与元数据汇报**。

#### 协议

- **Hadoop RPC**（ResourceManager 也用这一套）
    
- DataNode 向 NameNode 提供 **DatanodeProtocol** 接口。
    

#### 典型通信

- **心跳（Heartbeat）**：默认 3s，汇报 DataNode 状态（磁盘使用量、负载等）。
    
- **Block Report**：默认 1 小时一次，汇报自己持有的所有 block 列表。
    
- **Incremental Block Report**：增量汇报新增或删除的 block。
    
- **命令下发**：NameNode 也会通过心跳响应下发命令（如复制、删除 block）。
    
#### DataNode ↔ DataNode

#### 协议

DataNode ↔ DataNode 之间使用的协议 **并不是新的协议**，而是和 Client ↔ DataNode 一样的：

- **DataTransferProtocol**
    
    - 基于 TCP
        
    - 用于数据的读写、复制、转发
        
    - 内部是 packet + checksum 的流式格式
        

也就是说：

- Client → DN1 → DN2 → DN3 的 **流水线写入**，用的就是 **DataTransferProtocol**。
    
- DN_A → DN_B 的 **副本复制**，也是用 **DataTransferProtocol**，只不过是由 DataNode 发起，而不是客户端。

#### 典型场景

1. **写入副本（replication pipeline）**
    
    - 当客户端写文件时，NameNode 分配副本链（DN1 → DN2 → DN3）。
        
    - 客户端只把数据推给第一个 DataNode（DN1）。
        
    - DN1 会 **转发数据** 给 DN2，DN2 再转发给 DN3。
        
    - 这就是 **pipeline 写入**，实质上是 DataNode 之间的数据转发。
        
2. **副本恢复 / 冗余复制**
    
    - NameNode 发现某个 block 的副本丢失或不足（例如某个 DataNode 宕机）。
        
    - 它会通过 **心跳命令**指示某个 DataNode 去“复制 block 到另一个 DataNode”。
        
    - 此时，源 DataNode 会把本地的 block 通过数据传输协议发给目标 DataNode。
        

---


### 总结表

| 通信对端                | 协议/接口                             | 用途                                 |
| ------------------- | --------------------------------- | ---------------------------------- |
| Client ↔ NameNode   | **Hadoop RPC / ClientProtocol**   | 元数据操作：文件创建、查询、关闭                   |
| Client ↔ DataNode   | **DataTransferProtocol**          | 数据流传输（读写 block）                    |
| DataNode ↔ NameNode | **Hadoop RPC / DatanodeProtocol** | 心跳、block 报告、命令下发                   |
| DataNode ↔ DataNode | **DataTransferProtocol**          | 副本复制、写入流水线转发（replication pipeline） |

### 补充

- Hadoop RPC 本质上是 **TCP + Protobuf + 动态代理**，有点像轻量级的 gRPC。
    
- DataTransferProtocol 是 **面向流的私有协议**，不是 RPC，而是固定格式的二进制数据包。
    
- 客户端一般只跟 NameNode RPC 和 DataNode DataTransfer 打交道；  
    DataNode 和 NameNode 则靠心跳 RPC 保持一致性。

## HDFS体系结构的局限性

- HDFS的设计理念是“一次写入，多次读取”，本身不支持文件的随机写入，如果要进行修改，只能将整个文件读取到客户端完成修改后再整个写回HDFS
- 命名空间的限制：命名空间保存在名称节点，系统的总容纳量受到名称节点内存的限制
- 性能瓶颈：整个分布式文件系统只有一个名称节点，吞吐量受限于名称节点的处理能力
- 隔离问题：只有一个名称节点，无法对不同的应用程序进行隔离
- 集群可用性：一旦名称节点损坏，集群会变得不可用