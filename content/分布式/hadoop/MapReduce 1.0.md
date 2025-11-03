MapReduce 是一种基于分治思想的分布式并行计算框架，它将大规模数据处理抽象为由 Map 阶段和 Reduce 阶段组成的计算模型，通过键值对（key–value）的转换实现数据的分布式分解与汇聚。
## MapReduce1.0 的体系结构

MapReduce1.0包含四个组件：Client、JobTracker、TaskTracker、Task
JobTracker进程会运行在master节点(一般是NameNode)上，TaskTracker会运行在slave节点(一般是DataNode)上。
 - Client：用户编写的MapReduce程序通过Client想JobTracker提交
 - JobTracker：负责资源调度与作业调度
	 - JobTracker通过内部的TaskScheduler对象调度集群的计算资源
	 - JobTracker会监控所有TaskTracker和Job的健康状况，发现某个任务失败时，会将该人物转移到其他节点进行计算
- TaskTracker： 
	- 通过“心跳”机制向JobTracker汇报本节点的资源使用情况和任务完成情况
	- 使用slot等量划分节点上的cpu和内存资源，为每个Task以slot为单位分配计算资源
- Task：分为Map Task和Reduce Task,均由TaskTracker启动，分别使用Map slot和Reduce slot资源进行计算

## MapReduce的工作流程

- 文件读取与分割：
	- 针对不同类型的文件，MapReduce内部实现了多个InputFormat的子类，用于对大文件进行读取和逻辑分割（Split）
	- 一般来说一个hdfs block对应一个Split,但这个对应关系可以手动设置
	- InputFormat会为每个Split创建一个RecordReader（RR）,RR负责将每个Split的数据拆解成键值对的形式
	- RR是Inputformat的一个内部组件，将Split分割为键值对的方式有文件类型决定，分割规则在InputFormat的子类中有定义。比如读取文本文件的TextInputFormat就会使用LineRecordReader将文本文件按行分割成键值对。

- Map：
	- 每个Split对应一个Map Task；
	- map函数接受键值对作为输入，按照用户编写的map函数代码逻辑对数据进行计算
	- 以键值对的形式输出计算结果

- Shuffle：
	- Map端的Shuffle过程：
		- 缓存：随着Map Task的进行，输出结果会首先被保存到缓存中，Map Task的输出结果累计起来的总量达到最大缓存容量的某一比例时（默认为100mb的80%），会触发溢写。这种批量写入策略可以减少磁盘写入次数和数据序列化次数，提高性能。
		- 溢写（Spill）：
			- 首先对缓存数据的key进行分区，按照某个约定好的哈希函数对key进行哈希，然后对Reduce Task 数量取模，从而把Map Task的输出均匀分配给Reduce Task
			- 对每个分区内的key进行排序
			- 如果用户还定义了Combiner函数，则会对分区内的数据按照该函数的逻辑执行合并；该函数的功能和Reduce有些相似，但并非所有场合都可以使用Combiner,一个基本要求是，Combiner函数的逻辑不可以影响Reduce函数的最终输出
			- 写入磁盘，每次溢写操作都会在磁盘中产生一个新的溢写文件，溢写文件的键值对都是经过分区和排序的
		- 文件归并：
			- 当一个Map Task积累了足够多的溢写文件时，会对这些溢写文件中具有相同键的键值对进行合并，该键会被保留，而所有的值会被罗列成一个“数组”作为该键的新的值。
			- 文件归并是随着Map Task的进行而同时进行的，但最终当Map Task完成时，会把所有文件归并成一个完整的大文件。
		- 一个Map Task以及Shuffle完成后，TaskTracker会通过RPC向JobTracker汇报任务完成状态
	- Reduce端的Shuffle过程：
		- 当一个Map Task完成后，JobTracker会接到TaskTracker的汇报，然后向Reduce Task的TaskTracker发出通知，来拉去该Map Task的输出结果
		- 一个Reduce Task会从多个节点拉去Map Task的结果，拉去到的数据会被存放在Reduce Task机器的缓存中，如果缓存写满，同样会触发溢写操作
		- 来自不同Map机器的数据往往包含相同的键，Reduce端触发溢写时，会对这些含有相同键的键值对再一次执行归并
		- 如果用户定义了Combiner,则还会执行合并操作
		- 磁盘上的溢写文件数量达到io.sort.factor参数规定的值（默认为10）后，溢写文件会被归并成一个大文件，随着Map结果的汇总，最终会得到多个大文件
	
- Reduce ：Reduce端的Shuffle生成的多个大文件不会再进一步归并成一个大文件，而是直接输入给各个Reduce Task；Reduce Task按照用户的reduce函数规则计算出最终结果，并将结果进行保存或保存到HDFS中