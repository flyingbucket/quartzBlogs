## 指针概念

指针本身不是一个数据类型，而是一种**类型构造器**，给定任何一种类型（int、float、char等），可以通过星号生成指向该类型变量的指针。

```c
int x = 10;
int* ptr_to_int = &x;
```

`&`是**取地址**运算符，它可以作用在C语言的任何**左值**（lvalue）上，并返回该左值的内存地址。

`*`是**解引用**运算符，它作用在一个指针上时，`*ptr`等价于指针`ptr`所指向的变量。

```c
int x = 10;
int* xptr = &x;
x = 5;        // 将x的值置为5
*xptr = 5;    // 将x的值置为5
```

---
## Swap revisit

有了指针，我们就可以通过传递变量指针的方式，设计一个函数来访问另一个函数的变量。

```c
void swap(int* x, int* y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

int main(void) {
    int a = 3;
    int b = 4;
    printf("a:%d; b:%d\n", a, b);
    swap(&a, &b);
    printf("a:%d; b:%d\n", a, b);
    return EXIT_SUCCESS;
}
```

指针`x`、`y`分别指向原先的`a`、`b`，所以我们可以用以指针为参数的函数（swap）修改调用该函数的函数（main）中的局部变量的值。

---
## 指针的硬件工作原理

### 地址寻址

计算机通过**将数据存储在内存中**来管理所有信息。硬件使用**数值地址**来唯一标识内存中的每个存储单元。  
现代计算机通常采用**字节寻址内存**，即每个不同的地址对应内存中的一个字节。

- 每个内存单元代表一个字节。
    
- 每个字节都有一个唯一的地址（0, 1, 2, …）。
    

在 32 位机器上，地址由 32 位二进制数表示，因此一个指针变量需要 4 个字节来存储地址值，而与它指向的数据类型大小无关。  
在 64 位机器上，地址为 64 位，因此指针大小通常是 8 个字节。


### 指针的硬件实现

通过理解**内存**和**地址**，可以更清楚地认识指针的本质：  
**指针本质上就是一个变量，它存储了另一个变量的内存地址。**

- 对于占用空间超过一个字节的变量（如 `int`、`double`、`struct` 等），该变量在内存中会占据连续的一段字节。
    
- 其指针并不是记录这段内存的全部，而是**记录该对象起始字节的地址**。
    
- 只要知道起始地址，并结合类型信息（告诉编译器需要读取多少字节），程序就能正确地找到并访问变量的完整数据。

**PS：**
- 标准类型（如 `int`、`float`、`double`）以及数组在内存中的存储一定是连续的；结构体的成员变量可能存在填充（padding），但整体上依然是连续的一块内存。
- 对于链表、树等复杂数据结构，它们的节点通常分散存储，通过指针组合实现逻辑上的关联。
- 对于超大对象（如大数组、malloc 分配的大内存块），在**虚拟内存**中依然表现为连续的地址区间，但在**物理内存**中可能分散在多个页面上。
例如：

```c
int x = 42;    // 假设 int 占 4 字节
int *p = &x;   // p 保存的是 x 的起始地址
```

即使 `x` 占用 4 个字节，指针 `p` 只存储 `x` 所在内存区域的第一个字节的地址，编译器通过 `int*` 类型信息知道要取连续 4 字节来解释为一个整数。


### 重要限制

指针是值为“地址”的变量，这带来一个重要限制：  
**指针只能指向可寻址的数据对象。**

这意味着：

- 只有占据实际内存空间的对象（变量、数组、结构体等）才有地址。
    
- 不对应内存位置的临时结果或字面量不能被取地址。
    

例如：

```c
int *ptr = &3;        // ❌ 错误：字面量 3 不是可寻址对象
int *ptr = &(x + y);  // ❌ 错误：表达式 (x+y) 结果是临时值，无存储位置
```

这是因为 `3` 和 `(x+y)` 都不是 **左值（lvalue）**。

- 左值指代一个实际存储位置（“内存中的盒子”）。
    
- 临时值不是存储单元，因此没有地址。
    

编译器会报错，提醒我们只能对**变量**取地址。

**推论**：赋值语句的左边必须是一个左值，因为赋值操作的本质就是“往某个盒子里放数据”。  
因此像下面的语句会报错：

```c
3 = 4;        // ❌ 错误
x + y + z = 2; // ❌ 错误
```


## 核心要点总结

1. **指针的本质**
    
    - 指针不是独立类型，而是**类型构造器**，用于创建“指向某类型对象”的新类型。
        
2. **地址运算符 `&`**
    
    - 取出变量的内存地址（必须是左值）。
        
3. **解引用运算符 `*`**
    
    - 根据指针存储的地址访问对应内存中的值。
        
4. **内存寻址**
    
    - 每个内存位置都有唯一地址。指针存储的是地址，而不是对象的值。
        
5. **类型信息的重要性**
    
    - 指针只存储起始地址，编译器依靠指针的类型来确定访问多少字节。
        
6. **限制条件**
    
    - 指针只能指向可寻址的对象，不能指向常量或临时计算结果。
        
7. **赋值规则**
    
    - 赋值操作的左边必须是左值（实际内存单元）。
        

## Pointer to strut
When we have pointers to structs, we _can_ just use the * and . operators that we have seen so far, however, the order of operations means that . happens first. If we write *_a.b_, it means *(_a.b_)—a should be a struct, which we look inside to find a field named _b_ (which should be a pointer), and we dereference that pointer. If we have a pointer to a struct (c, and we want to refer to the field d in the struct at the end of the arrow, we would need parenthesis, and write (*_c_)._d_ (or the _->_ operator we will learn about momentarily).

In the figure above, we have a **struct** which has a field _p_ (which is a pointer to an int), and a field _x_ which is an int. We then declare _y_ (an int), _a_ (a struct), and _q_ (a pointer to a struct), and initialize them. When we write _*a.p_, the order of operations is to evaluate _a.p_ (which is an arrow pointing at _y_), then dereference that arrow. If we wrote _*q.x,_ we would receive a compiler error, as _q_ is not a struct, and the order of operations would say to do _q.x_ first (which is not possible, since _q_ is not a struct). We could write parenthesis, as in the figure ((_*q_)_.x_).

However, pointers to structs are incredibly common, and the above syntax gets quite cumbersome, especially with pointers to structs which have pointers to structs, and so on. For (_*q_)_.x,_ it may not be so bad, but if we have _(*(*(*q).r).s).t_ it becomes incredibly ugly, and confusing. Instead, we should use the _->_ operator, which is shorthand for dereferencing a pointer to a struct and selecting a field—that is, we could write _q->x_ (which means exactly the same thing as (_*q_)_.x_). For our more complex example, we could instead write _q->r->s->t_ (which is easier to read and modify).
## Aliasing
## const

| 类型                            | Can we change **p | Can we change *p | Can we change p | 解读                                                                    |
| ----------------------------- | ----------------- | ---------------- | --------------- | --------------------------------------------------------------------- |
| `int ** p`                    | Yes               | Yes              | Yes             | 三层都非 `const`；可以改底层 `int`、中间指针的指向、以及最外层指针变量本身。                         |
| `const int ** p`              | No                | Yes              | Yes             | 最里层是 `const int`，不能改数据；两层指针本身都可改动其指向。                                 |
| `int * const * p`             | Yes               | No               | Yes             | 中间层是“**const 指针**指向 `int`”：不能给 `*p` 赋新地址；但底层 `int` 可改、外层 `p` 可改。      |
| `int ** const p`              | Yes               | Yes              | No              | 最外层是“**const 指针**”：`p` 不能改；但可改中间层指向与底层 `int`。                         |
| `const int * const * p`       | No                | No               | Yes             | 中间层是“**const 指针**指向 `const int`”：既不能改底层数据，也不能给 `*p` 换地址；仅能改外层 `p` 本身。 |
| `const int ** const p`        | No                | Yes              | No              | 最外层是 `const`（不能改 `p`）；底层数据是 `const int`（不能改 `**p`）；但能让中间层 `*p` 指向别处。  |
| `int * const * const p`       | Yes               | No               | No              | 两层指针都是 `const`（`*p` 与 `p` 都不能改）；但中间层指向的是非常量 `int`，因此可改底层数据。           |
| `const int * const * const p` | No                | No               | No              | 三层都被“读写能力”锁死：底层数据是 `const`，两层指针也都是 `const`。                           |

### 洋葱法

**ps：** const相当于一个形容词，`int float double  * `等等相当于名词，const对“名词”的修饰可以放在“名词之前或名词之后
eg：
	const int 和 int const完全等价

解读这类复杂声明时，应先找到变量名，然后按照英语的顺序,以“名词”为一层，一层一层向外解读。const形容词优先和右侧(内层)的名词结合。

eg:
```c
const int * const * const p
```
- 首先找到变量名p
- 向外读：
	-  `* const p`：p is a const pointer to
	-  `* const` :  a const pointer to
	-  `int const` : a const int
- 和起来：p is a const pointer to a const pointer to a const int
- 翻译过来：
	- p是一个指向（指向常量整型的常量指针）的（常量指针）
	- `p` 是一个 **常量指针**，它指向一个 **常量指针**，而这个常量指针又指向一个 **常量整型**

## 指针运算 (Pointer Arithmetic)

在 C 语言中，指针带有**类型信息**。当你做 `p + n`、`p - n` 或两个同类型指针相减时，编译器会按“元素”为单位而不是“字节”为单位计算位移。

**核心公式（概念上）：**  
`p + n == (char*)p + n * sizeof(*p)`  
也就是说，步长由“指针所指向的类型”的大小决定。

### 常见步长举例

- `int*`：`p + 1` 前进 `sizeof(int)` 字节（常见为 4）
    
- `float*`：`q + 1` 前进 `sizeof(float)` 字节（常见为 4）
    
- `double*`：`r + 1` 前进 `sizeof(double)` 字节（常见为 8）
    
- `char*`：`c + 1` 前进 1 字节（逐字节移动，常用于二进制/内存操作）
    

### 小示例（地址变化对比）

```c
#include <stdio.h>

int main(void) {
    int    xi = 42;     int*    pi = &xi;
    float  xf = 3.14f;  float*  pf = &xf;
    double xd = 6.28;   double* pd = &xd;

    printf("pi    = %p, pi+1    = %p\n", (void*)pi, (void*)(pi+1));
    printf("pf    = %p, pf+1    = %p\n", (void*)pf, (void*)(pf+1));
    printf("pd    = %p, pd+1    = %p\n", (void*)pd, (void*)(pd+1));

    // 逐字节观察
    char* pc = (char*)&xi;
    printf("pc    = %p, pc+1    = %p\n", (void*)pc, (void*)(pc+1));
    return 0;
}
```

具体数值可能因平台不同而变化
### 重要规则 & 易错点

1. **数组边界规则**  
    指针运算**只在同一数组对象内**有定义（含“尾后位置”one-past-the-end）。
    
    - 你可以把 `p` 移动到 `arr + n`（`n` 为数组长度）的“尾后”位置，但**不能解引用**那个位置。
        
    - 超出这个范围的计算或任何越界解引用都是**未定义行为（UB）**。
        
    - 对于单个变量 `int x; int* p = &x; p+1` 只是“算出一个地址”，对其**解引用**是 UB。
        
2. **`void*` 不能做算术（标准 C）**  
    标准 C 不允许对 `void*` 做加减（某些编译器作为扩展支持）。如需按字节移动，**先转成 `char*`** 再运算：  
    `char* pc = (char*)p; pc += n;`
    
3. **指针对齐（alignment）**  
    把不同类型指针“硬转换”并解引用，可能触发未对齐访问（某些架构会崩溃/降速）。尽量遵守原始类型的对齐要求。
    
4. **指针相减**  
    仅当两个指针指向**同一数组**的元素（或其尾后位置）时，`p2 - p1` 才有定义，结果类型是 `ptrdiff_t`，表示**元素个数**的差，而非字节差。
    


### 速记总结

- **步长 = `sizeof(*p)`**，不是 1 字节（除 `char*`）。
    
- **只在同一数组内**做运算并**谨慎解引用**尾后位置（不可解引用）。
    
- **逐字节移动**请用 `char*`。
    
- **避免未对齐**与越界，`p2 - p1` 返回**元素差**而非字节差。
    
