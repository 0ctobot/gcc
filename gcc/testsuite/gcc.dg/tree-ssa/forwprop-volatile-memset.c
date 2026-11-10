/* A volatile byte store must not be folded into a following memset.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-forwprop1" } */

void
store_then_pad (char *p)
{
  *(volatile char *) p = 'a';
  __builtin_memset (p + 1, ' ', 3);
}

/* Here the memset covers the byte, so merging drops the store.  */
void
store_then_fill (char *p)
{
  *(volatile char *) p = 'a';
  __builtin_memset (p, ' ', 4);
}

void
vptr_store_then_pad (volatile char *p)
{
  *p = 'a';
  __builtin_memset ((char *) p + 1, ' ', 3);
}

/* A plain byte store still merges with the memset into one copy.  */
void
plain_store_then_pad (char *p)
{
  *p = 'a';
  __builtin_memset (p + 1, ' ', 3);
}

/* { dg-final { scan-tree-dump-times "={v} 97;" 3 "forwprop1" } } */
/* { dg-final { scan-tree-dump-times "__builtin_memset" 3 "forwprop1" } } */
/* { dg-final { scan-tree-dump-times "__builtin_memcpy \\(p_\[0-9\]+\\(D\\), &\"a   \"\\\[0\\\], 4\\);" 1 "forwprop1" } } */
