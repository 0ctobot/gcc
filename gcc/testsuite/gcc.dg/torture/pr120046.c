/* PR rtl-optimization/120046 */
/* { dg-do run } */
/* { dg-additional-options "-fno-tree-ccp -fno-tree-dominator-opts -fno-tree-fre -fno-expensive-optimizations -fno-gcse" } */

/* CSE follows the path where j.b is zero and turns the copy of j.b on that
   path into a load of zero, leaving behind the REG_EQUAL note that names the
   real value of j.b.  That path never runs, but cross jumping used to match
   the copy against the same copy on the path that does run, and replace it.  */

struct a { long long b; } g;
int c, h;
long long y;
volatile int e;

__attribute__ ((noipa)) void
foo (int x)
{
}

__attribute__ ((noipa)) struct a
i1 (void)
{
  struct a j = { 5000000000LL }, *k = &j;
  foo (0);
l:
  if (j.b)
    return j;
  if (h)
    {
      if (c)
	goto l;
      return g;
    }
  foo (e);
  struct a *m, **n = &m;
  *n = k;
  return *m;
}

/* The compare below keeps j.b in a register on targets where late combine would
   otherwise fold a combined compare and branch in i1, removing the path that
   never runs.  */

__attribute__ ((noipa)) struct a
i2 (void)
{
  struct a j = { 5000000000LL }, *k = &j;
  foo (0);
l:
  if (j.b)
    {
      if (j.b < y)
	foo (1);
      return j;
    }
  if (h)
    {
      if (c)
	goto l;
      return g;
    }
  foo (e);
  struct a *m, **n = &m;
  *n = k;
  return *m;
}

/* The same with a constant source on the path that runs, which makes the two
   insns replaceable in either direction.  */

__attribute__ ((noipa)) struct a
i3 (void)
{
  struct a j = { 5000000000LL }, *k = &j;
  foo (0);
l:
  if (j.b)
    {
      struct a t = { 5000000000LL };
      if (j.b < y)
	foo (1);
      return t;
    }
  if (h)
    {
      if (c)
	goto l;
      return g;
    }
  foo (e);
  struct a *m, **n = &m;
  *n = k;
  return *m;
}

int
main (void)
{
  if (i1 ().b != 5000000000LL)
    __builtin_abort ();
  if (i2 ().b != 5000000000LL)
    __builtin_abort ();
  if (i3 ().b != 5000000000LL)
    __builtin_abort ();
  return 0;
}
