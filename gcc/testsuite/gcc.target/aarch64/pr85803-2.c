/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-dse" } */

/* RTL DSE has to leave a store alone when an access through another pointer
   may fault after it, without missing the stores it can delete.  */

int g, h[4];

void f1 (int *p, int *q) { *p = 42; *q = 43; *p = 1; }
int f2 (int *p, int *q) { *p = 42; int v = *q; *p = 1; return v; }
void f3 (int *q) { g = 42; *q = 43; g = 1; }
int f4 (int *q) { g = 42; int v = *q; g = 1; return v; }

void g1 (int *p) { *p = 77; *p = 1; }
void g2 (int *p) { p[0] = 77; p[1] = 3; p[0] = 1; }
void g3 (void) { g = 77; h[1] = 3; g = 1; }
void g4 (int i) { g = 77; h[i] = 3; g = 1; }

/* { dg-final { scan-assembler-times "mov\tw\[0-9\]+, 42" 4 } } */
/* { dg-final { scan-assembler-not "mov\tw\[0-9\]+, 77" } } */
