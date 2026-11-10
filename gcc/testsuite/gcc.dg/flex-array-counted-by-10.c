/* The late objsz pass replaces .ACCESS_WITH_SIZE with the address of the
   counted_by array once it has folded the object size builtins, so the
   passes after it see a plain array: a clearing loop becomes a memset and a
   field read before a store into the array is reused after it.  */
/* { dg-do run } */
/* { dg-options "-O2 -fno-strict-aliasing -fdump-tree-optimized" } */

struct V
{
  unsigned int n;
  unsigned int flags;
  unsigned long e[] __attribute__ ((counted_by (n)));
};

__attribute__ ((noipa)) void
clear (struct V *v)
{
  for (unsigned int i = 0; i < v->n; i++)
    v->e[i] = 0;
}

__attribute__ ((noipa)) unsigned int
store_then_flags (struct V *v, unsigned int i, unsigned long x)
{
  unsigned int f = v->flags;
  v->e[i] = x;
  return f + v->flags;
}

__attribute__ ((noipa)) unsigned long
size (struct V *v)
{
  return __builtin_dynamic_object_size (v->e, 1);
}

static unsigned long buf[8];

int
main (void)
{
  struct V *v = (struct V *) buf;
  v->n = 4;
  v->flags = 3;
  for (int i = 0; i < 6; i++)
    v->e[i] = 7;
  clear (v);
  if (v->e[0] != 0 || v->e[3] != 0 || v->e[4] != 7)
    __builtin_abort ();
  if (store_then_flags (v, 1, 5) != 6 || v->e[1] != 5)
    __builtin_abort ();
  if (size (v) != 4 * sizeof (unsigned long))
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not "ACCESS_WITH_SIZE" "optimized" } } */
/* { dg-final { scan-tree-dump-times "__builtin_memset" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-times "->flags" 1 "optimized" } } */
