/* A CONSTRUCTOR with one element assigned to a volatile aggregate is
   written with a single store, as WRITE_ONCE () requires.  */
/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only" } */

#define WRITE_ONCE(x, val) \
  do { *(volatile __typeof__ (x) *) &(x) = (val); } while (0)

struct in { int x, y; };
struct o { struct in i; };
struct one { unsigned long v; };
struct two { unsigned int lo, hi; };
struct wr { struct two t; };
union hp { unsigned long w; struct { unsigned int lo, hi; }; };

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
f6 (volatile struct two *p, unsigned int a)
{
  *p = (struct two) { .lo = a };
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

void
f12 (struct o *p, int a, int b)
{
  WRITE_ONCE (*p, ((struct o) { { a, b } }));
}

/* Stored directly, as before.  */
void
f13 (struct one *p, unsigned long v)
{
  WRITE_ONCE (*p, ((struct one) { v }));
}

/* { dg-final { scan-assembler-times "\tstr\tx\[0-9\]+, \\\[x0\\\]\n" 7 } } */
/* { dg-final { scan-assembler-not "\tstr\tw" } } */
/* { dg-final { scan-assembler-not "\tstr\txzr" } } */
