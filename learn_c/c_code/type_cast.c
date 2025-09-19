#include <stdio.h>

int main() {
  int a = 5;
  int b = 2;
  // a and b are int; with integer division, a/b = 2 (truncates toward zero).
  // Assigning to float happens after the integer division, so c becomes 2.0.
  float c = a / b;

  // To keep precision, make at least one operand float.
  // Then the whole expression is evaluated in floating-point.
  float d = a / (float)b;
  printf("c:%f\n", c);
  printf("d:%f\n", d);
  return 0;
}
