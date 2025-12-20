选择scala的Sequence子类数据结构时需要考虑一个问题，我们需要array那样带有索引的内存连续的结构，还是像链表那样更灵活的结构。

| Type/Category         | Immutable | Mutable       |
| --------------------- | --------- | ------------- |
| Indexed               | `Vector`  | `ArrayBuffer` |
| Linear (Linked lists) | `List`    | `ListBuffer`  |
## List
### 定义List
List是一种线性的不可变结构。可以这样创建List
```scala
val ints = List(1, 2, 3) 
val names = List("Joel", "Chris", "Ed") 
// another way to construct a List 
val namesAgain = "Joel" :: "Chris" :: "Ed" :: Nil
```

也可以为List中的元素声明类型
```scalaj j
val ints: List[Int] = List(1, 2, 3)
```
List中的元素如果不是同一个类的实例，scala会按照类型推断的集成关系找到它们的父类
```scalaj j
val things: List[Any] = List(1, "two", 3.0)
```
*上述代码是scala2的行为，scala3中会推断为并集类型*

### 修改
List是不可变类型，如果要修改则必须创建新的List. 在List开头或结尾添加但个元素，使用`::`连接,添加List使用`:::`连接。
```scala
val a = List(1, 2, 3)
val b = 0 :: a // List(0, 1, 2, 3) 
val c = List(-1, 0) ::: a // List(-1, 0, 1, 2, 3)
```

List是一种不可变单链表，在头部添加元素是O(1)的，只需把新List绑定给新节点，并把新节点的next指针指向原List即可；而在尾部添加元素是O(n)的，因为原List是不可变的，我们不能把最后一个元素的next从Nil修改成新节点的地址，而必须把整个链表复制一遍。
同理，拼接两个链表是，复杂度取决于前面的链表的长度。

### 遍历
```scala
scala> val names:List[String] = List("Jim","Jack","Joe")
val names: List[String] = List(Jim, Jack, Joe)

scala> for (name<-names){println(name)}
Jim
Jack
Joe
```
### 其他常用方法
O(1)方法：
```scala
//REPL
scala> names.head
val res2: String = Jim

scala> names.tail //返回除去第一个元素外的剩余列表
val res3: List[String] = List(Jack, Joe)

scala> names.isEmpty
val res4: Boolean = false
```
O(n)方法
```scala
//REPL
scala> names.last
val res5: String = Joe

scala> names.init //返回除去最后一个元素外的剩余列表
val res6: List[String] = List(Jim, Jack)

scala> names.apply(2) //按照索引查询
val res7: String = Joe

scala> names.apply(1)
val res8: String = Jack
```
### map方法
使用`List.map`方法，传入一个函数，自动遍历List并返回新的List
#### 匿名函数
```scala
//REPL
scala> val ints = List(1,2,3,4,5)
val ints: List[Int] = List(1, 2, 3, 4, 5)

scala> val doubleInts = ints.map((i:Int)=>i*2)
val doubleInts: List[Int] = List(2, 4, 6, 8, 10)
```
可以这样简化匿名函数的写法
```scala
//REPL
scala> val doubleInts = ints.map((i)=>i*2)
val doubleInts: List[Int] = List(2, 4, 6, 8, 10)

scala> val doubleInts = ints.map(i=>i*2)
val doubleInts: List[Int] = List(2, 4, 6, 8, 10)

scala> val doubleInts = ints.map(_*2)
val doubleInts: List[Int] = List(2, 4, 6, 8, 10)
```
#### 一般函数
```scala
scala> val ints = List(1,2,3,4,5)
val ints: List[Int] = List(1, 2, 3, 4, 5)

scala> val addTwo:Int=>Int = (a)=>{a+2}
val addTwo: Int => Int = $Lambda/0x00007f4150570220@272f5373

scala> val plus2Ints=ints.map(addTwo)
val plus2Ints: List[Int] = List(3, 4, 5, 6, 7)
```

### flatMap方法
如果一个函数返回list,把它传给map会得到一个列表的列表，而使用flatMap会把这些列表都展开成一维列表。
```scala
scala> val xs = List(List(1,2),List(3,4),List(5,6))
val xs: List[List[Int]] = List(List(1, 2), List(3, 4), List(5, 6))

scala> val flat = xs.flatMap(identity)
val flat: List[Int] = List(1, 2, 3, 4, 5, 6)
```

常用于构造笛卡尔积
```scala
//REPL
scala> val nums=List(1,2,3)
val nums: List[Int] = List(1, 2, 3)

scala> val chars=List('a','b','c')
val chars: List[Char] = List(a, b, c)

scala> val comb=nums.flatMap(n=>chars.map(c=>(n,c)))
val comb: List[(Int, Char)] = List((1,a), (1,b), (1,c), (2,a), (2,b), (2,c), (3,a), (3,b), (3,c))
```

##  Vector
Vector是有索引、可随机访问的不可变序列。对任何索引的读取都可以在常数时间内完成。
### 定义Vector
```scala
val nums = Vector(1, 2, 3, 4, 5)

val strings = Vector("one", "two")

case class Person(name: String)
val people = Vector(
  Person("Bert"),
  Person("Ernie"),
  Person("Grover")
)
```
### 添加元素
```scala
val a = Vector(1,2,3)         // Vector(1, 2, 3)
val b = a :+ 4                // Vector(1, 2, 3, 4)
val c = a ++ Vector(4, 5)     // Vector(1, 2, 3, 4, 5)
```

## ArrayBuffer
ArrayBuffer是有索引的可变序列，可以修改值也可以修改数组大小。
### 定义
```scala
import scala.collection.mutable.ArrayBuffer
//空ArrayBuffer
var strings = ArrayBuffer[String]()
var ints = ArrayBuffer[Int]()
var people = ArrayBuffer[Person]()

//定义大小
// ready to hold 100,000 ints
val buf = new ArrayBuffer[Int](100_000)

//赋值初始化
val nums = ArrayBuffer(1,2,3)
```

*创建ArrayBuffer时要声明容量必须使用new调用ArrayBuffer类的构造器，其他情况下不必使用new关键字，直接从ArrayBuffer伴生对象实例化即可。*

### 修改
添加
```scala
val nums = ArrayBuffer(1, 2, 3)   // ArrayBuffer(1, 2, 3)
nums += 4                         // ArrayBuffer(1, 2, 3, 4)
nums ++= List(5, 6)               // ArrayBuffer(1, 2, 3, 4, 5, 6)
```

删除
```scala
val a = ArrayBuffer.range('a', 'h')   // ArrayBuffer(a, b, c, d, e, f, g)
a -= 'a'                              // ArrayBuffer(b, c, d, e, f, g)
a --= Seq('b', 'c')                   // ArrayBuffer(d, e, f, g)
a --= Set('d', 'e')                   // ArrayBuffer(f, g)
```

修改
```scala
val a = ArrayBuffer.range(1,5)        // ArrayBuffer(1, 2, 3, 4)
a(2) = 50                             // ArrayBuffer(1, 2, 50, 4)
a.update(0, 10)                       // ArrayBuffer(10, 2, 50, 4)
```