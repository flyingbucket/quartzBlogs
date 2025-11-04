| 数据类型 (关键字)                    | 常见定义/声明方式                                           | 占位符 (printf/scanf) | 说明                                             |
| ----------------------------- | --------------------------------------------------- | ------------------ | ---------------------------------------------- |
| `char`                        | `char c = 'A';`                                     | `%c`               | 字符类型（实际上存储整数，对应 ASCII 码）。通常 1 字节 (8 位)。        |
| `signed char`                 | `signed char sc = -10;`                             | `%c`               | 明确带符号的字符，范围一般是 -128 ~ 127。                     |
| `unsigned char`               | `unsigned char uc = 250;`                           | `%c` 或 `%hhu`      | 无符号字符，范围 0 ~ 255。                              |
| `int`                         | `int x = 42;`                                       | `%d` 或 `%i`        | 整数，通常 4 字节，范围约 -2,147,483,648 ~ 2,147,483,647。 |
| `short` / `short int`         | `short s = 100;`                                    | `%hd`              | 短整型，一般 2 字节。                                   |
| `unsigned short`              | `unsigned short us = 60000;`                        | `%hu`              | 无符号短整型，范围约 0 ~ 65535。                          |
| `long` / `long int`           | `long l = 123456;`                                  | `%ld`              | 长整型，一般 4 或 8 字节（取决于系统）。                        |
| `unsigned long`               | `unsigned long ul = 4000000000UL;`                  | `%lu`              | 无符号长整型。                                        |
| `long long` / `long long int` | `long long ll = 123456789LL;`                       | `%lld`             | 更大的整数，一般 8 字节。                                 |
| `unsigned long long`          | `unsigned long long ull = 18446744073709551615ULL;` | `%llu`             | 最大的标准无符号整数类型。                                  |
| `float`                       | `float f = 3.14f;`                                  | `%f`               | 单精度浮点数，约 6~7 位有效数字。                            |
| `double`                      | `double d = 3.141592653;`                           | `%lf`              | 双精度浮点数，约 15~16 位有效数字。                          |
| `long double`                 | `long double ld = 3.141592653589793L;`              | `%Lf`              | 扩展精度浮点，精度比 `double` 高（平台依赖）。                   |
| `_Bool` (C99)                 | `_Bool flag = 1;`                                   | `%d`               | 只能是 `0` 或 `1`。需要 `<stdbool.h>` 头文件时可写作 `bool`。 |
| `void`                        | `void func(void);`                                  | 无                  | 表示“无类型”，用于函数返回类型或指针 (`void*`)。                 |