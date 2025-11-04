![[YARN工作流程_inverted.png]]

- 用户编写客户端应用程序，向YARN提交应用程序，提交的内容包括ApplicationMaster程序、启动ApplicationMaster的命令、用户程序等
- YARN中的ResourceManager负责接收和处理来自客户端的请求，为应用程序分配一个容器，在该容器中启动一个ApplicationMaster
- ApplicationMaster被创建后会首先向ResourceManager注册
- ApplicationMaster采用轮询的方式向ResourceManager申请资源
- ResourceManager以“容器”的形式向提出申请的ApplicationMaster分配资源
- 在容器中启动任务（运行环境、脚本）
- 各个任务向ApplicationMaster汇报自己的状态和进度
- 应用程序运行完成后，ApplicationMaster向ResourceManager的应用程序管理器注销并关闭自己

**疑问**:第一步中的AM程序是指什么？是用户选择的计算框架中封装好的某个ApplicationMaster类吗？