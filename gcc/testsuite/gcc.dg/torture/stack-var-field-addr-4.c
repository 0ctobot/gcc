/* { dg-do run } */

/* Same as stack-var-field-addr-2.c, except that the pointer goes through an
   integer on its way into and out of the const call.  q is still derived from
   &v or &w, so it has to count as a mention of them.  */

struct S { long pad; int f; int g; };

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

__attribute__((const, noipa)) __UINTPTR_TYPE__
fld (__UINTPTR_TYPE__ p)
{
  return p + __builtin_offsetof (struct S, f);
}

__attribute__((noipa)) int
foo (int sel, int n)
{
  struct S y;
  int first = 1;

  for (int i = 0; __builtin_expect_with_probability (i < n, 1, 0.5); i++)
    {
      {
	struct S v, w;
	struct S *p = sel ? &v : &w;
	int *q = (int *) fld ((__UINTPTR_TYPE__) p);
	set (q, i + 100);
      }
      if (first)
	{
	  y.f = 42;
	  first = 0;
	}
      use (&y.f);
    }
  return first ? -1 : y.f;
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
