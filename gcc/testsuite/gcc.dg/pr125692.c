/* { dg-do compile } */
/* { dg-options "-O3 -g -fchecking -w" } */

/* Hoisting the guard around the inner loop of h made the code after the outer
   loop reachable without going through the definition of a value that a debug
   bind there still referenced.  */

int *a, b;
int f(int g) {
  int c = 1;
  while (1) {
    if (c - g)
      break;
    c = -1;
  }
}
void h() {
  int c = 0, i;
d:
  a = &c;
  *a = c - 1;
  if (b)
    goto d;
  *a = c - 20002849452162;
  if (f(c))
    goto d;
  i = c;
}
