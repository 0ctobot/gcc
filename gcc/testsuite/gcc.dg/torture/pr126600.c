/* PR tree-optimization/126600 */
/* { dg-do run } */

/* The widening_mul pass folds the x != 0 guard in front of an r / x == y
   overflow check only when x == 0 lands on the no-overflow path.  When the
   FALSE edge of the check came first in the successor vector, it looked at the
   overflow path instead and removed guards that were needed.  */

typedef unsigned int u32;

int ok_calls, bad_calls;

__attribute__((noipa)) void
ok_path (void)
{
  ok_calls++;
}

__attribute__((noipa)) void
bad_path (void)
{
  bad_calls++;
}

/* A single case switch keeps its default edge first after it becomes a
   condition.  */

__attribute__((noipa)) int
f1 (u32 x)
{
  if (x != 0)
    {
      u32 r = x * 3;
      switch (r / x)
	{
	case 3:
	  ok_path ();
	  return 0;
	default:
	  break;
	}
    }
  bad_path ();
  return 1;
}

__attribute__((noipa)) int
f2 (u32 x)
{
  if (x == 0)
    goto bad;
  u32 r = x * 3;
  switch (r / x)
    {
    default:
    bad:
      bad_path ();
      return 1;
    case 3:
      ok_path ();
      return 0;
    }
}

/* Inverting a boolean condition swaps the edge flags in place.  */

static inline _Bool
fits (u32 x, u32 y, u32 *out)
{
  u32 r = x * y;
  *out = r;
  return r / x == y;
}

__attribute__((noipa)) int
f3 (u32 x)
{
  u32 o;
  _Bool good = x != 0;
  if (good)
    good = fits (x, 3, &o);
  if (!good)
    {
      bad_path ();
      return 1;
    }
  ok_path ();
  return 0;
}

/* Reading the CFG back in LTO mode sorts the successors by the position of
   their destinations in the block chain, which puts the FALSE edge first
   here.  */

__attribute__((noipa)) int
f4 (u32 x)
{
  if (x != 0)
    {
      u32 r = x * 3;
      if (r / x == 3)
	;
      else
	goto bad;
      ok_path ();
      return 0;
    }
bad:
  bad_path ();
  return 1;
}

/* Here x == 0 does join the no-overflow path, so the guard is redundant and
   the result must stay the same whether or not it's removed.  */

__attribute__((noipa)) int
f5 (u32 x)
{
  if (x != 0)
    {
      u32 r = x * 3;
      switch (r / x)
	{
	default:
	  bad_path ();
	  return 1;
	case 3:
	  break;
	}
    }
  ok_path ();
  return 0;
}

static volatile u32 vals[] = {
  0, 1, 2, 3, 1000, 0x55555555, 0x55555556, 0x80000000, 0xffffffff
};

static void
check (int (*fn) (u32), int zero_ok)
{
  for (unsigned int i = 0; i < sizeof (vals) / sizeof (vals[0]); i++)
    {
      u32 x = vals[i], r;
      int expect = x == 0 ? !zero_ok : __builtin_mul_overflow (x, 3U, &r);
      int ok_before = ok_calls, bad_before = bad_calls;
      if (fn (x) != expect
	  || ok_calls - ok_before != !expect
	  || bad_calls - bad_before != expect)
	__builtin_abort ();
    }
}

int
main (void)
{
  check (f1, 0);
  check (f2, 0);
  check (f3, 0);
  check (f4, 0);
  check (f5, 1);
  return 0;
}
