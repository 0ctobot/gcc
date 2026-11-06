/* PR rtl-optimization/119980 */
/* { dg-do run } */
/* { dg-options "-O2 -fdisable-rtl-combine" } */

/* With combine disabled, the i386 peephole2 removes the self-copy instead.  */

typedef struct foo { int x; } F;
typedef struct bar { int x; } B;

static int
foo (void *p, int i, int j, int v)
{
  F *pp = p;
  pp[i] = (F) { v };
  return pp[j].x;
}

static int
bar (void *p, int i, int j, int v)
{
  B *pp = p;
  pp[i] = (B) { v };
  return pp[j].x;
}

__attribute__((noinline)) int
test (void *fp, int i, int j, int k)
{
  foo (fp, i, j, 1);
  int s = bar (fp, j, k, 2);
  return foo (fp, k, i, s);
}

int
main (void)
{
  void *fp = __builtin_malloc (sizeof (F));
  if (fp && test (fp, 0, 0, 0) != 2)
    __builtin_abort ();
  return 0;
}
