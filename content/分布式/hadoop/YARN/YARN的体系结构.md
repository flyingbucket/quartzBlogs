YARN包含三个组件：ResourceManager、ApplicationMaster、NodeManager

## ResourceManager
运行在master节点上的常驻进程。
RM包含两个组件：ResourceScheduler、ApplicationsManager

### ResourceScheduler：资源管理和分配
- 接受来自ApplicationsMaster的资源请求，并把资源以容器（container）的形式分配给程序
- [[#容器（container）]]的选择会考虑程序要处理的数据的所在位置，就近选择
- 监控NodeManager

### ApplicationsManager
- 通过RPC接受客户端请求
- 接受到ResourceScheduler的资源分配方案后，在某个NodeManager上启动AM
- 跟踪应用状态
- 应用重启与恢复

## 容器（container）

容器是YARN上资源调度的单位，和MapReduce1的Slot相比，具有一下改进：
- **数量可变**：容器的数量由当前负载决定，不再固定；
- **资源可变**：每个容器可请求不同的资源量；
- **用途灵活**：容器可以执行任意类型任务（Map、Reduce、Spark executor、Tez task等）；
- **生命周期短**：容器用完即销毁，资源即时回收。


## ApplicationMaster
运行在worker节点上的非常驻进程，客户端提交的一个任务唯一对应一个AM。
- RM接收到客户端请求后分配任务的第一个容器并在该容器中启动该任务的AM 
- AM根据任务需要向RM申请更多容器，这些容器可以在其他节点上
- 与容器所在的各个节点的NodeManager通信，进行容器及容器上程序的启动、运行、监控和停止以及重启
- 对ResourceManager分配的资源进而二次分配，具体将容器分配给程序内的Map、Reduce等等任务
- 定时向ResourceManager发送心跳，汇报资源使用情况和程序完成进度，作业完成后向RM注销容器

## NodeManager
运行在worker节点上的常驻进程。
- 资源汇报与心跳： 向 ResourceManager 注册节点、周期性汇报所在节点的资源使用与健康状态。
- 容器生命周期管理：启动、监控和停止容器，负责环境初始化、资源隔离与清理。
- 监控与日志收集：跟踪容器运行状态、执行健康检查，并汇总日志上传至 HDFS。
- 通信协调：与 ResourceManager 进行心跳同步，与 ApplicationMaster 协作启动或销毁容器。