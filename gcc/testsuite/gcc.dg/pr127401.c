/* PR tree-optimization/127401 */
/* Hoisting the loads out of the conditional in f () makes the load of s->b
   unconditional, so it reads a byte that isn't a valid _Bool when c is
   nonzero and d is true.  That must not change the value of q.  */
/* { dg-do run } */
/* { dg-options "-O2 -fhoist-adjacent-loads --param l1-cache-line-size=64 -fdump-tree-phiopt2-details" } */

struct S { _Bool a; _Bool b; };
union U { struct S s; unsigned char c[2]; };
union U u;
volatile unsigned char va = 1;
volatile unsigned char vb = 2;
volatile int vc = 1;
volatile _Bool vd = 1;

__attribute__((noipa)) int
f (struct S *s, int c, _Bool d)
{
  _Bool r, q;
  if (c)
    r = s->a;
  else
    r = s->b;
  if (d)
    q = 1;
  else
    q = s->b;
  return r * 16 + q;
}

/* Loads of a type without padding bits are hoisted as they are.  */

struct T { char a; char b; };
struct T t = { 1, 2 };

__attribute__((noipa)) int
g (struct T *p, int c)
{
  char r;
  if (c)
    r = p->a;
  else
    r = p->b;
  return r;
}

int
main (void)
{
  u.c[0] = va;
  u.c[1] = vb;
  if (f (&u.s, vc, vd) != 17)
    __builtin_abort ();
  if (g (&t, vc) != 1)
    __builtin_abort ();
  return 0;
}

/* Not every target hoists adjacent loads.  The _Bool loads in f () must be
   hoisted as loads of the whole byte.  */
/* { dg-final { scan-tree-dump-times "Hoisting adjacent loads" 2 "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
/* { dg-final { scan-tree-dump "= VIEW_CONVERT_EXPR<unsigned char>\\(s_\[0-9\]+\\(D\\)->b\\);" "phiopt2" { target { aarch64*-*-* x86_64-*-* i?86-*-* } } } } */
