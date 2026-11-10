/* { dg-do run } */

/* Same as stack-var-field-addr-2.c, except that the address reaches q through
   one const call nested in another, both of which LIM hoists out of the loop.
   q has to count as a mention of v and w through both calls.  */

struct S { long pad; int f; int g; };
struct T { long pad; struct S s; };

volatile int sink;

__attribute__((noipa)) void
use (int *p)
{
  sink += *p;
}

__attribute__((noipa)) void
set (int *p, int val)
{
  *p = val;
}

__attribute__((const, noipa)) struct S *
inner (struct T *p)
{
  return &p->s;
}

__attribute__((const, noipa)) int *
fld (struct S *p)
{
  return &p->f;
}

__attribute__((noipa)) int
foo (int sel, int n)
{
  struct T y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct T v, w;
	struct T *p = sel ? &v : &w;
	int *q = fld (inner (p));
	set (q, i + 100);
      }
      if (first)
	{
	  y.s.f = 42;
	  first = 0;
	}
      use (&y.s.f);
    }
  return first ? -1 : y.s.f;
}

int
main (void)
{
  if (foo (0, 3) != 42)
    __builtin_abort ();
  if (foo (1, 3) != 42)
    __builtin_abort ();
  return 0;
}
