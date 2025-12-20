Map是scala中的键值对表，默认为immutable,scala.collection.mutable下提供了多种mutable的Map
## 不可变Map
### 定义Map
```scala
//REPL
//immutable
scala> val states = Map(
     |   "AK" -> "Alaska",
     |   "AL" -> "Alabama",
     |   "AZ" -> "Arizona"
     | )
val states: scala.collection.immutable.Map[String,String] = Map(AK -> Alaska, AL -> Alabama, AZ -> Arizona)
```

### 访问
```scala
scala> val ak = states("AK")
val ak: String = Alaska

scala> val al = states("AL")
val al: String = Alabama
```

```scala
//REPL
// for-loop
scala> for ((key,value)<-states) {println(s"$key:$value")}
AK:Alaska
AL:Alabama
AZ:Arizona
```

###  增添
```scala
val a = Map(1 -> "one")    // a: Map(1 -> one)
val b = a + (2 -> "two")   // b: Map(1 -> one, 2 -> two)
val c = b ++ Seq(
  3 -> "three",
  4 -> "four"
)
// c: Map(1 -> one, 2 -> two, 3 -> three, 4 -> four)
```
### 删除
```scala
val a = Map(
  1 -> "one",
  2 -> "two",
  3 -> "three",
  4 -> "four"
)

val b = a - 4       // b: Map(1 -> one, 2 -> two, 3 -> three)
val c = a - 4 - 3   // c: Map(1 -> one, 2 -> two)
```
### 修改
```scala
val a = Map(
  1 -> "one",
  2 -> "two",
  3 -> "three"
)

val b = a.updated(3, "THREE!")   // b: Map(1 -> one, 2 -> two, 3 -> THREE!)
val c = a + (2 -> "TWO...")      // c: Map(1 -> one, 2 -> TWO..., 3 -> three)
```
