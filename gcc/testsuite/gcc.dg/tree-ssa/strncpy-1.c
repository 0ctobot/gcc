/* { dg-do compile } */
/* { dg-options "-O -fdump-tree-optimized" } */

void sink (void*);

void f (const char *s)
{
  char a[256];

  __builtin_memset (a, 0, sizeof a);   // stays, since the strncpy can fault
  __builtin_strncpy (a, s, sizeof a);

  sink (a);
}

/* { dg-final { scan-tree-dump "memset" "optimized" } } */
