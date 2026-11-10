/* { dg-do run } */

/* Same as stack-var-field-addr-2.c, except that fld is only discovered to be
   const by IPA, so the call can still be hoisted out of the loop and q has to
   count as a mention of v and w.  */

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

__attribute__((noinline)) int *
fld (struct S *p)
{
  return &p->f;
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
	int *q = fld (p);
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
