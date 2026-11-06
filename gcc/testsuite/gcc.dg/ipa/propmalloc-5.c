/* The result of an inlined callee reaches the return value of cand, so the
   callee's own malloc state still decides whether cand returns fresh memory.
   Inlining notmalloc into cand mustn't make cand look like malloc.  */
/* { dg-do run } */
/* { dg-options "-O2 -fdump-ipa-pure-const-details -fdelete-null-pointer-checks" } */
/* { dg-skip-if "" keeps_null_pointer_checks } */

int g;
int sink[64];

static int *
notmalloc (int n)
{
  for (int i = 0; i < n; i++)
    sink[(i * 7) & 63] += i ^ n;
  for (int i = 0; i < n; i++)
    sink[(i * 11) & 63] -= i * n;
  return &g;
}

__attribute__ ((noinline)) int *
cand (int c, int n)
{
  return c ? notmalloc (n) : __builtin_malloc (sizeof (int));
}

__attribute__ ((noinline)) int
test (int c, int n)
{
  int *p;
  g = 1;
  p = cand (c, n);
  if (!p)
    __builtin_abort ();
  *p = 2;
  return g;
}

int
main (void)
{
  if (test (1, 3) != 2)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-ipa-dump-not "Function cand/\[0-9+\]+ found to be malloc" "pure-const" } } */
