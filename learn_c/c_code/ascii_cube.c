// ascii_cube.c
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// if M_PI is not provided in the global env, we specify it as below
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 画布大小（可调）
#define W 100
#define H 35

// char canvas[H][W] declared a 2 dim char arr
// static declaration makes this arr only visible in this file
// it will be alloced when the program begin and won't be freed until the
// program returns
static char canvas[H][W];

// 在画布上安全地设置一个字符
void pset(int x, int y, char c) {
  if (x >= 0 && x < W && y >= 0 && y < H) {
    canvas[y][x] = c;
  }
}

// Bresenham 画线
void line(int x0, int y0, int x1, int y1, char c) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;

  while (1) {
    pset(x0, y0, c);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy) {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx) {
      err += dx;
      y0 += sy;
    }
  }
}

// 等距投影：加入 y_aspect（纵向压缩因子）
void iso_project(double x, double y, double z, double scale, double y_aspect,
                 int offset_x, int offset_y, int *u, int *v) {
  // 30°/30° 等距投影
  double u_d = (x - y) * 0.8660254; // cos(30°)
  double v_d = (x + y) * 0.5 - z;   // sin(30°) - z

  *u = (int)lround(u_d * scale) + offset_x;
  *v = (int)lround(v_d * scale * y_aspect) + offset_y; // ← 这里压缩 y
}
int main(int argc, char **argv) {
  // 参数：边长 scale（字符像素尺度），立方体尺寸 side（单位长度）
  double scale = 2.0;   // 投影到字符网格时的缩放
  double side = 8.0;    // 立方体边长（世界坐标）
  int ox = 50, oy = 10; // 画布中的偏移（相当于“把立方体摆在画布哪里”）

  if (argc >= 2)
    side = atof(argv[1]); // ./a.out 12
  if (argc >= 3)
    scale = atof(argv[2]); // ./a.out 12 2.5

  // 1) 清空画布
  for (int y = 0; y < H; ++y)
    for (int x = 0; x < W; ++x)
      canvas[y][x] = ' ';

  // 2) 定义立方体 8 个顶点（以 (0,0,0) 为一角）
  // 顶点索引：按二进制位含义 (x,y,z) 是否加 side
  double vx[8], vy[8], vz[8];
  for (int i = 0; i < 8; ++i) {
    vx[i] = (i & 1) ? side : 0.0;
    vy[i] = (i & 2) ? side : 0.0;
    vz[i] = (i & 4) ? side : 0.0;
  }

  // 3) 投影到 2D
  int px[8], py[8];
  for (int i = 0; i < 8; ++i) {
    double y_aspect = 0.48;
    iso_project(vx[i], vy[i], vz[i], scale, y_aspect, ox, oy, &px[i], &py[i]);
  }

  // 4) 12 条棱（每条边一对顶点）
  int edges[12][2] = {{0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3},
                      {2, 6}, {3, 7}, {4, 5}, {4, 6}, {5, 7}, {6, 7}};

  // 5) 优先画“背面”再画“前面”可以减少穿插，但这里简单全部画
  for (int e = 0; e < 12; ++e) {
    int a = edges[e][0], b = edges[e][1];
    // 斜边用 '/', '\'；水平用 '-'；竖直用 '|'
    int dx = px[b] - px[a], dy = py[b] - py[a];
    char ch = (abs(dx) > 0 && abs(dy) > 0) ? ((dx * dy < 0) ? '/' : '\\')
                                           : (dx ? '-' : '|');
    line(px[a], py[a], px[b], py[b], ch);
  }

  // （可选）给前脸画框线更明显
  // 前脸四点：0,1,3,2（z=0 的那一面）
  line(px[0], py[0], px[1], py[1], '-');
  line(px[1], py[1], px[3], py[3], '|');
  line(px[3], py[3], px[2], py[2], '-');
  line(px[2], py[2], px[0], py[0], '|');

  // 6) 打印画布
  for (int y = 0; y < H; ++y) {
    fwrite(canvas[y], 1, W, stdout);
    putchar('\n');
  }
  return 0;
}
