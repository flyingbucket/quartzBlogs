---
title: scala编程语言基础知识
---
## 简介
语言特性
- **强类型 + 类型推断**：编译期安全，常可省略类型标注；表达式风格，少用 `return`。
- **OO × FP 融合**：一等函数、不可变集合、`map/flatMap` 管道；`class`/`trait` 便于组合复用。
- **模式匹配 + ADT**：`match` 搭配 `case class`/`enum`，表达复杂分支既简洁又类型安全。
- **JVM 互操作与并发**：无缝调用 Java 库；`Future/Promise`、Actor（Akka）等并发模型可选。

 常见用途
- **大数据与分布式计算**：编写 Spark 作业（RDD/DataFrame/Dataset）、ETL、批/流处理。
- **高并发后端服务**：基于 Akka HTTP、Play、http4s 的微服务与实时系统。
- **数据工程与ML管线**：任务编排、特征工程、与 Hadoop 生态集成。
- **DSL 与工具开发**：构建领域特定语言、配置/脚本（sbt、Scala-CLI）、规则引擎与静态分析工具。

## 参考教程
Scala 官网：
- [Scala Book](https://docs.scala-lang.org/scala3/book/introduction.html)
- [Scala for Python Developers](https://docs.scala-lang.org/scala3/book/scala-for-python-devs.html)

YouTube：
- [Scala Programming Full Course | Scala tutorial For Beginners](https://youtu.be/tamJAEtSEEo?si=hYi-wNqVJluvgidZ)


*本仓库的笔记均基于scala 2.13,scala3中的部分新特性可能会做简单的对比*

*Current State*：只写了第一第二个笔记，后面的计划可能推倒重来