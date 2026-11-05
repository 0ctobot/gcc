/* PR middle-end/127596 */
/* { dg-do run } */

/* LIM hoists the address of p->f out of the loop, and the peeled first
   iteration ends the scope of v and w before y gets written.  From then on
   the loop only reaches v and w through q, so q has to count as a mention
   of them.  Otherwise neither one conflicts with y and they can share its
   stack slot.  */

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
	int *q = &p->f;
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
