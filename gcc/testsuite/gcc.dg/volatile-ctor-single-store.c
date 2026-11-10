/* A CONSTRUCTOR with one element assigned to a volatile aggregate must
   store to the target once, through a temporary, unless the element fills
   the target by itself.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-optimized" } */

#define WRITE_ONCE(x, val) \
  do { *(volatile __typeof__ (x) *) &(x) = (val); } while (0)

struct r { int a, b, c, d; };
struct in { int x, y; };
struct o { struct in i; };
struct one { unsigned long v; };
struct two { unsigned int lo, hi; };
struct wr { struct two t; };
struct holder { int pad; volatile struct two reg; };
union hp { unsigned long w; struct { unsigned int lo, hi; }; };

/* Incomplete CONSTRUCTORs, whose missing members are implicitly zero.  */
void
f1v (volatile struct r *p, int v)
{
  *p = (struct r) { .a = v };
}

void
f6 (volatile struct two *p, unsigned int a)
{
  *p = (struct two) { .lo = a };
}

void
f7 (struct holder *h, unsigned int a)
{
  h->reg = (struct two) { .hi = a };
}

volatile struct r gr;

void
f9 (int v)
{
  gr = (struct r) { .b = v };
}

void
f10 (union hp *p, unsigned int v)
{
  WRITE_ONCE (*p, ((union hp) { .lo = v }));
}

void
f11 (struct two *p, unsigned int v)
{
  WRITE_ONCE (*p, ((struct two) { .hi = v }));
}

/* A lone element that is itself a CONSTRUCTOR.  */
void
f2 (volatile struct o *p, int a, int b)
{
  *p = (struct o) { { a, b } };
}

void
f4 (volatile struct wr *p, unsigned int a)
{
  *p = (struct wr) { .t = { .lo = a } };
}

void
f12 (struct o *p, int a, int b)
{
  WRITE_ONCE (*p, ((struct o) { { a, b } }));
}

/* Two elements already went through the temporary.  */
void
f5 (volatile struct two *p, unsigned int a, unsigned int b)
{
  *p = (struct two) { a, b };
}

/* A lone scalar member that fills the struct, like __pte (), is still
   stored directly.  */
void
f13 (struct one *p, unsigned long v)
{
  WRITE_ONCE (*p, ((struct one) { v }));
}

/* A lone zero element is still one clear of the target.  */
void
e4 (volatile struct two *p)
{
  *p = (struct two) { .lo = 0 };
}

/* { dg-final { scan-tree-dump-times "={v} D\\.\[0-9\]+;" 10 "optimized" } } */
/* { dg-final { scan-tree-dump-times "={v} \[a-z\]+_\[0-9\]+\\(D\\);" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "\\.v ={v} v_\[0-9\]+\\(D\\);" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "={v} \\{\\};" 1 "optimized" } } */
