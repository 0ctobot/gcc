/* PR tree-optimization/128481 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-slp-vectorize" } */

#include <setjmp.h>

jmp_buf jb;
void usei (unsigned);
void usep (unsigned char *);
struct S { unsigned a : 4, b : 4, c : 8; };
struct A { unsigned char b[8]; } __attribute__((aligned (8)));

/* The merged bit-field stores use the abnormal x past its next version.  */

void
g1 (struct S *p, unsigned x)
{
  setjmp (jb);
  usei (x);
  p->a = x;
  x = x * 3 + 1;
  p->b = x;
  p->c = 7;
  usei (x);
}

/* The stores merged by the bswap recognizer do the same.  */

void
g2 (unsigned char *p, unsigned x)
{
  setjmp (jb);
  usei (x);
  p[0] = x;
  x = x >> 8;
  p[1] = x;
  x = x >> 8;
  p[2] = x;
  x = x >> 8;
  p[3] = x;
  usei (x);
}

/* And so does the bswap pass on its own.  */

unsigned
g3 (unsigned x)
{
  setjmp (jb);
  usei (x);
  unsigned a = x & 0xff; x = x >> 8;
  unsigned b = x & 0xff; x = x >> 8;
  unsigned c = x & 0xff; x = x >> 8;
  unsigned r = a << 24 | b << 16 | c << 8 | x;
  usei (x);
  return r;
}

/* The merged load uses the abnormal q past its next version, unless SLP
   vectorization gets to the load first.  */

void
g4 (unsigned char *restrict p, unsigned char *restrict q)
{
  setjmp (jb);
  usep (q);
  unsigned char a = q[0];
  p[0] = a;
  unsigned char b = q[1];
  q = q + 2;
  p[1] = b;
  usep (q);
}

/* A split store covering just one original store reuses its value at the last
   store in the group, here the abnormal x past its next version.  */

void
g5 (struct A *restrict p, const struct A *restrict q)
{
  unsigned char x = 0;
  setjmp (jb);
  usei (x);
  x = q->b[3];
  unsigned char a = q->b[4], b = q->b[5], c = q->b[6], d = q->b[7];
  usei (x);
  p->b[3] = x;
  x = x * 3 + 1;
  p->b[4] = a;
  p->b[5] = b;
  p->b[6] = c;
  p->b[7] = d;
  usei (x);
}

/* The same with the reused value not coming straight from a load.  */

void
g6 (struct A *restrict p, const struct A *restrict q)
{
  unsigned char x = 0;
  setjmp (jb);
  usei (x);
  unsigned char v = q->b[3];
  x = v ^ 5;
  unsigned char a = q->b[4] ^ 5, b = q->b[5] ^ 5;
  unsigned char c = q->b[6] ^ 5, d = q->b[7] ^ 5;
  usei (x);
  p->b[3] = x;
  x = x * 3 + 1;
  p->b[4] = a;
  p->b[5] = b;
  p->b[6] = c;
  p->b[7] = d;
  usei (x);
}
