/* { dg-do compile } */
/* { dg-options "-O2 -fwrapv -fdump-tree-ivopts" } */

/* NODE is made from an integer, so ivopts must not take a pointer from the
   integer's computation as the base of an address it rewrites.  GAPS is
   computed from NODE by pointer arithmetic, though, and the original store
   was based on it.  The rewritten store should keep GAPS as its base and
   index it with the induction variable, rather than computing the whole
   address as an integer and converting it back to a pointer.  */

typedef __UINTPTR_TYPE__ uintptr_t;

struct node
{
  unsigned long pivot[9];
  unsigned long pad[46];
  unsigned long gap[10];
  unsigned char meta[2];
};

struct big
{
  unsigned long pivot[34];
  unsigned long gap[34];
};

void
f (struct big *b, uintptr_t entry, unsigned char start, unsigned char last,
   int type)
{
  struct node *node = (struct node *) (entry & ~(uintptr_t) 255);
  unsigned long *pivots = node->pivot;
  unsigned long *gaps = type == 3 ? node->gap : 0;
  unsigned long max_gap = 0;
  unsigned char offset = 0, end;
  int i = start, j = 0;

  do
    pivots[j++] = b->pivot[i++];
  while (i <= last && b->pivot[i]);
  end = j - 1;
  do
    {
      gaps[--j] = b->gap[--i];
      if (gaps[j] > max_gap)
	{
	  offset = j;
	  max_gap = gaps[j];
	}
    }
  while (j);
  node->meta[0] = end;
  node->meta[1] = offset;
}

/* P starts out as Q, but every later P is converted from an integer, so Q
   is no base for the store even though it's the base of the iv.  This store
   still has to be computed as an integer and converted to a pointer, which
   is the only conversion to void * that the dump should have.  */

void
g (int *q, int *b, int n)
{
  int *p = q;
  for (int i = 0; i < n; i++)
    {
      *p = b[i];
      p = (int *) ((uintptr_t) p + 4);
    }
}

/* { dg-final { scan-tree-dump-times {= \(void \*\) _[0-9]+;} 1 "ivopts" } } */
/* { dg-final { scan-tree-dump-not {MEM\[\(int \*\)q_[0-9]+\(D\)} "ivopts" } } */
