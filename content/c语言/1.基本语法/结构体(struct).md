
## 结构体概念

在C语言中，**结构体（struct）** 是一种用户自定义的数据类型，它可以把多个不同类型的数据组合在一起，形成一个整体。  
结构体适用于描述 **复杂对象**，比如一个学生（有学号、姓名、成绩等）、一个点（有x、y坐标）、一辆车（有车牌号、颜色、速度等）。

---

## 结构体的定义方式

### 基本语法

```c
struct 结构体名 {
    数据类型 成员名1;
    数据类型 成员名2;
    ...
};
```

### 示例

```c
struct Student {
    int id;        // 学号
    char name[20]; // 姓名
    float score;   // 成绩
};
```

这样就定义了一个 `Student` 类型。

---

## 定义结构体变量

```c
struct Student s1;    // 定义一个结构体变量
struct Student s2 = {1001, "Alice", 95.5};  // 定义并初始化
```

还可以和[[typedef]]配合：

```c
typedef struct {
    int x;
    int y;
} Point;

Point p1 = {10, 20};
```


## 访问结构体成员

结构体成员通过 **`.` 运算符** 来访问：

```c
printf("ID: %d, Name: %s, Score: %.2f\n", s2.id, s2.name, s2.score);
```

如果是 **结构体指针**，则通过 `->` 运算符访问：

```c
struct Student *ps = &s2;
printf("Name = %s\n", ps->name);
```


## 结构体数组

结构体也能组成数组，方便批量管理。

```c
struct Student class[3] = {
    {1001, "Alice", 95.5},
    {1002, "Bob", 88.0},
    {1003, "Charlie", 92.5}
};
```


## 结构体嵌套

结构体成员还可以是另一个结构体：

```c
struct Date {
    int year, month, day;
};

struct Student {
    int id;
    char name[20];
    struct Date birthday;  // 嵌套的结构体
};

struct Student s = {1001, "Alice", {2003, 5, 10}};
printf("Birthday: %d-%d-%d\n", s.birthday.year, s.birthday.month, s.birthday.day);
```


## 结构体与函数

结构体可以作为函数的 **参数** 或 **返回值**：

```c
struct Point {
    int x, y;
};

struct Point move(struct Point p, int dx, int dy) {
    p.x += dx;
    p.y += dy;
    return p;
}

int main() {
    struct Point p1 = {0, 0};
    p1 = move(p1, 5, 10);
    printf("(%d, %d)\n", p1.x, p1.y);  // (5, 10)
}
```


## 内存与对齐

- 结构体中每个成员在内存中是按顺序存储的，但会有 **字节对齐（padding）**。
    
- `sizeof(struct)` 的结果可能大于成员大小之和。
    
- 如果需要优化内存，可以调整成员顺序，或者使用 `#pragma pack`（依赖编译器）。
    


## 常见用法总结

- **描述实体对象**：如学生、点、日期等。
    
- **结构体数组**：批量存储多个实体。
    
- **结构体指针**：常用于函数参数传递，避免大规模复制。
    
- **嵌套结构体**：描述复杂对象。