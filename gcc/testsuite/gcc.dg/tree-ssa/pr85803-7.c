/* PR tree-optimization/85803 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

/* A store stays when an access through another pointer may fault after it.  */

int g, h[4];

void f1 (int *p, int *q) { *p = 42; *q = 43; *p = 1; }
int f2 (int *p, int *q) { *p = 42; int v = *q; *p = 1; return v; }
void f3 (int *q) { g = 42; *q = 43; g = 1; }
int f4 (int *q) { g = 42; int v = *q; g = 1; return v; }
void f5 (int *p, int *q, int i) { if (i) { *p = 42; *q = 43; } *p = 1; }

/* An access through the same pointer or to a named object can't take away the
   store's chance of being deleted, and neither can one when a local variable
   holds the value since nothing can see it.  */

void g1 (int *p) { *p = 77; *p = 1; }
void g2 (int *p) { p[0] = 77; p[1] = 3; p[0] = 1; }
int g3 (int *p) { p[0] = 77; int v = p[1]; p[0] = 1; return v; }
void g4 (void) { g = 77; h[1] = 3; g = 1; }
void g5 (int i) { g = 77; h[i] = 3; g = 1; }
int g6 (int *q) { int l[2]; l[0] = 77; *q = 1; l[0] = *q; return l[0]; }

/* { dg-final { scan-tree-dump-times " = 42;" 5 "optimized" } } */
/* { dg-final { scan-tree-dump-not " = 77;" "optimized" } } */
