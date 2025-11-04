[[MapReduce 1.0#MapReduce1.0 的体系结构]]
- JobTracker端：
	- 整个系统只有一个JobTracker,单点故障风险高
	- JobTracker内存开销大

 - TaskTracker端：
	 - 资源完全按照MapReduce任务个数来分配，当两个具有较大内存消耗的任务被分配到同一个TaskTracker上时，容易发生内存溢出
	 - 资源按照slot进行划分，分为Map Slot和Reduce Slot,iangzhe不能通用，当节点只有Map或Reduce一类任务运行时，另一类Slot会闲置，导致资源浪费
	