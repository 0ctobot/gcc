/* The late objsz pass replaces .ACCESS_WITH_SIZE with the address of the
   counted_by array, but only after folding the object size builtins, so a
   __builtin_dynamic_object_size call that only sees the array once the IPA
   inliner has run still reads the counter.  The early pass must leave the
   call alone for that.  The stores through the pointer keep the
   .ACCESS_WITH_SIZE calls in use until the late pass drops them.  */
/* { dg-do run } */
/* { dg-options "-O2 -fno-early-inlining -fdump-tree-optimized" } */

struct V
{
  unsigned int n;
  unsigned int flags;
  unsigned long e[] __attribute__ ((counted_by (n)));
};

static unsigned long
elems_size (unsigned long *p)
{
  p[0] = 1;
  return __builtin_dynamic_object_size (p, 1);
}

static unsigned long
elems_size0 (unsigned long *p)
{
  p[1] = 2;
  return __builtin_dynamic_object_size (p, 0);
}

__attribute__ ((noipa)) unsigned long
size_inl (struct V *v)
{
  return elems_size (v->e);
}

__attribute__ ((noipa)) unsigned long
size0_inl (struct V *v)
{
  return elems_size0 (v->e);
}

static unsigned long buf[8];

int
main (void)
{
  struct V *v = (struct V *) buf;
  v->n = 4;
  if (size_inl (v) != 4 * sizeof (unsigned long))
    __builtin_abort ();
  if (size0_inl (v) != 4 * sizeof (unsigned long))
    __builtin_abort ();
  if (v->e[0] != 1 || v->e[1] != 2)
    __builtin_abort ();
  v->n = 6;
  if (size_inl (v) != 6 * sizeof (unsigned long))
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-tree-dump-not "ACCESS_WITH_SIZE" "optimized" } } */
/* { dg-final { scan-tree-dump-not "object_size" "optimized" } } */
