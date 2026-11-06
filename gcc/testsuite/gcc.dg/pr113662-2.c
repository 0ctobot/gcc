/* The store's address is an integer sum of the pointer iv P and an offset
   that takes it out of A and into B.  IVOPTS must not base the reference on
   P, since P only points to A and the store to B would then look unrelated
   to the read of b[1] after the loop.  */
/* { dg-do run } */
/* { dg-options "-O2 -fno-tree-vectorize -fdump-tree-ivopts" } */

typedef __UINTPTR_TYPE__ uintptr_t;

__attribute__ ((noipa)) int
f (int n)
{
  int a[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
  int b[8] = { 0 };
  uintptr_t d = (uintptr_t) b - (uintptr_t) a;
  int *end = a + n;

  b[1] = 1;
  for (int *p = a; p != end; p++)
    *(int *) ((uintptr_t) p + d) = *p + 5;
  return b[1];
}

int
main (void)
{
  if (f (0) != 1 || f (4) != 6)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not {MEM\[\(int \*\)p_[0-9]+ \+ d_} "ivopts" } } */
