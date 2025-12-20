Sequence Map 和Set是Iterable的子类，也是很多其他数据类型的父类，是scala中最重要的三种数据结构。

scala.collection.immutable
![[collections-immutable-diagram-213.svg]]

scala.collection.muatable
![[collections-mutable-diagram-213.svg]]


常用数据结构：

| Collection Type | Immutable | Mutable | Description                                                                                                             |
| --------------- | --------- | ------- | ----------------------------------------------------------------------------------------------------------------------- |
| `List`          | ✓         |         | A linear (linked list), immutable sequence                                                                              |
| `Vector`        | ✓         |         | An indexed, immutable sequence                                                                                          |
| `LazyList`      | ✓         |         | A lazy immutable linked list, its elements are computed only when they’re needed; Good for large or infinite sequences. |
| `ArrayBuffer`   |           | ✓       | The go-to type for a mutable, indexed sequence                                                                          |
| `ListBuffer`    |           | ✓       | Used when you want a mutable `List`; typically converted to a `List`                                                    |
| `Map`           | ✓         | ✓       | An iterable collection that consists of pairs of keys and values.                                                       |
| `Set`           | ✓         | ✓       | An iterable collection with no duplicate elements                                                                       |

## Sequence