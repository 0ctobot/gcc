/* { dg-do run } */
/* { dg-options "-O2 -fdump-tree-store-merging" } */
/* { dg-final { scan-tree-dump-times "MEM\\\[\\(int \\*\\)\[ps\]_\[0-9\]+\\(D\\)\\\] = 0;" 2 "store-merging" } } */

/* The store of zero through int changes the type of the memory that the
   clear before it wrote as struct F, so store merging can't drop it: the
   loads through struct B conflict with it but not with the clear.  */

struct F { int x; int y[7]; };
struct B { int x; };
struct S { struct F f; unsigned char pad[4]; };
struct B gout;

__attribute__((noipa)) int
f1 (struct F *p, struct B *q)
{
  q->x = 1;
  *p = (struct F) { 0 };
  *(int *) p = 0;
  return q->x;
}

/* The clear's base is a struct with a char array here.  */

__attribute__((noipa)) void
f2 (struct S *s, struct B *q)
{
  q->x = 1;
  s->f = (struct F) { 0 };
  *(int *) &s->f = 0;
  gout = *q;
}

int
main (void)
{
  void *m = __builtin_malloc (sizeof (struct S));
  if (!m)
    return 0;
  if (f1 (m, m) != 0)
    __builtin_abort ();
  f2 (m, m);
  if (gout.x != 0)
    __builtin_abort ();
  __builtin_free (m);
  return 0;
}
