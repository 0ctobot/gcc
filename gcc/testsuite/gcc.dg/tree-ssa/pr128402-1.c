/* PR tree-optimization/128402 */
/* { dg-do compile { target bitint } } */
/* { dg-options "-O3 -std=gnu23 -fdump-tree-sra-details" } */

/* A topmost VIEW_CONVERT_EXPR to a large _BitInt must not make late SRA replace
   the array holding the value, since the replacement cannot stay in SSA form
   after _BitInt lowering.  */

#if __BITINT_MAXWIDTH__ >= 129
_BitInt(129) x, y;
int c, n = 1;

void
f (_BitInt(129) a, _BitInt(129) b)
{
  if (n--)
    {
      x = 0;
      f (c ? x : 0x10000000000000001uwb, y);
    }
  y += a;
}

int
main ()
{
  f (0, 0);
  return 0;
}
#endif

/* { dg-final { scan-tree-dump-not "Created a replacement for \[^ \]+ offset: \[0-9\]+, size: (1\[3-9\]\[0-9\]|\[2-9\]\[0-9\]\[0-9\])" "sra" } } */
