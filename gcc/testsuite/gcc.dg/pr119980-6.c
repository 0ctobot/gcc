/* PR target/119980 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* A store with base alias set int conflicts with struct B, which has an int
   member, but one with base alias set F doesn't, so the last store in each
   function has to stay.  Without it the load through Q gets disambiguated from
   the remaining store and returns the value stored through Q first.  */

struct F { int x; };
struct B { int x; };

__attribute__((noipa)) int
child (struct F *p, struct B *q)
{
  q->x = 1;
  *p = (struct F) { 2 };
  *(int *) p = 2;
  return q->x;
}

__attribute__((noipa)) int
child_zero (struct F *p, struct B *q)
{
  q->x = 1;
  *p = (struct F) { 0 };
  *(int *) p = 0;
  return q->x;
}

/* The same with a void * member, which isn't a parent of other pointers
   either.  */

struct FP { void *p; };
struct BP { int *p; };
int a, b;

__attribute__((noipa)) int *
universal (struct FP *p, struct BP *q)
{
  q->p = &a;
  *p = (struct FP) { &b };
  *(int **) p = &b;
  return q->p;
}

int
main (void)
{
  void *m = __builtin_malloc (sizeof (union { struct F f; struct FP fp; }));
  if (!m)
    return 0;
  if (child (m, m) != 2)
    __builtin_abort ();
  if (child_zero (m, m) != 0)
    __builtin_abort ();
  if (universal (m, m) != &b)
    __builtin_abort ();
  return 0;
}
