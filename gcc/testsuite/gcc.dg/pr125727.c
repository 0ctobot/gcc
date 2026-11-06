/* PR rtl-optimization/125727 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-forwprop -fno-tree-pre -fno-tree-vrp -fno-tree-dce" } */

int a, c;
volatile long b;
void d() {
  int e = 3;
  while (b) {
    int f = -1;
    unsigned g = (a || 0) & ((b ^ e) & (c && f) && 1);
    if (e || g)
      a = g;
    for (; e; e++)
      ;
  }
}
