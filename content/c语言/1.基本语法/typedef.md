## `typedef`概念

- `typedef` 是 **类型重定义关键字**，用来给已有的数据类型起一个新的名字（别名）。
    
- 它不会创建新的类型，只是让代码更简洁、更易读。
    


## 基本语法

```c
typedef 原类型 新类型名;
```


## 示例

### 基本类型重命名

```c
typedef unsigned int uint;

uint x = 10;  // 等价于 unsigned int x = 10;
```


### 指针类型别名

```c
typedef char* string;

string s1 = "Hello";   // 等价于 char* s1 = "Hello";
```


### 结构体简化

没有 `typedef` 时，定义变量必须写 `struct`：

```c
struct Student {
    int id;
    char name[20];
};
struct Student s1;
```

使用 `typedef` 可以给 `struct` 类型起一个别名：

```c
struct StudentInfo {
    int id;
    char name[20];
};

typedef struct StudentInfo Student; // 用 typedef 起一个别名
Student s1;   // 等价于 struct Student s1
```

也可以简写成匿名结构体 + typedef：

```c
typedef struct {
    int id;
    char name[20];
} Student;

Student s1;   // 不再需要写 struct
```

---

#### 两种 `typedef struct` 的区别

1. **有标签的写法**
    
    ```c
    typedef struct StudentInfo {
        char name[20];
        int id;
        float score;
    } Student1;
    
    struct StudentInfo a;  // 用标签
    Student1 b;            // 用别名
    ```
    
    - 定义了 **标签 `struct StudentInfo`** 和 **别名 `Student1`**。
        
    - 允许 **前置声明**，因此支持自引用（链表、树等数据结构常用）。
        
2. **匿名结构体写法**
    
    ```c
    typedef struct {
        char name[20];
        int id;
        float score;
    } Student2;
    
    Student2 c;  // 用别名
    // struct ??? d; 没有标签，不能这样写
    ```
    
    - 没有结构体标签，只有 **别名 `Student2`**。
        
    - **不能前置声明**，因此 **不能自引用**，只适合简单数据结构。
        

---

#### 总结

- 有标签的写法功能更完整（能前置声明、自引用），更适合复杂场景。
    
- 匿名写法更简洁，常用于简单结构或库接口。
    

### 复杂类型简化

```c
typedef int (*FuncPtr)(int, int);  

int add(int a, int b) { return a + b; }

FuncPtr f = add;
printf("%d\n", f(3, 4));  // 输出 7
```


## `typedef` 与 `#define` 的区别

| 特性         | `typedef`     | `#define`   |
| ---------- | ------------- | ----------- |
| 处理方式       | 编译阶段          | 预处理阶段（文本替换） |
| 是否有作用域     | 有（仅在定义作用域内有效） | 无（全局宏替换）    |
| 能否定义指针类型别名 | 可以            | 需要括号，容易出错   |
| 代码可读性      | 更安全，更直观       | 容易产生歧义      |

例子：

```c
#define PTR_INT int*
typedef int* PtrInt;

PTR_INT a, b;   // 实际是 int* a, b;  (b不是指针)
PtrInt c, d;    // 两个都是 int* 类型
```


## 常见用法总结

- 给 **基本类型** 起短名（如 `uint`）。
    
- 给 **指针类型** 起别名（如 `string`）。
    
- 给 **结构体/联合/枚举** 起别名，避免每次写 `struct`。
    
- 给 **函数指针** 起别名，简化复杂声明。
    


小结：  
`typedef` 本质上是 **类型别名机制**，它不创建新类型，只是提高 **可读性** 和 **可维护性**。  
在大型项目中，`typedef` 常与 `struct`、`enum`、函数指针一起使用，能让代码更简洁、更清晰。

