/* PR tree-optimization/128402 */
/* { dg-do compile { target bitint } } */
/* { dg-options "-O2 -std=gnu23 -fdump-tree-sra-details" } */

/* Total scalarization must not replace a large _BitInt member after _BitInt
   lowering either.  The address of S is only gone once USE has been inlined,
   which keeps early SRA from seeing it.  */

#if __BITINT_MAXWIDTH__ >= 129
struct S { _BitInt(129) x; };
extern struct S ga, gb;
extern volatile int n;
extern void h (int);

static void
use (struct S *p, int k)
{
  for (int i = 0; i < k; i++)
    {
      h (n + i);
      h (n - i);
      h (n * i);
      if (n > 100)
	h (i);
      if (n > 200)
	h (i + 1);
    }
  ga = *p;
}

void
f (int c, int k)
{
  struct S s;
  if (c)
    s = ga;
  else
    s = gb;
  use (&s, k);
}
#endif

/* { dg-final { scan-tree-dump-not "Created a replacement for \[^ \]+ offset: \[0-9\]+, size: (1\[3-9\]\[0-9\]|\[2-9\]\[0-9\]\[0-9\])" "sra" } } */
