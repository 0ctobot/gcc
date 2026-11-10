/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-profile_estimate" } */

/* Cases where a __builtin_expect () hint on a PHI node of constants says
   nothing about some of the branches that pick the constants.  */

#define unlikely(x) __builtin_expect (!!(x), 0)

void slow (void);
void fast (void);
int ext (int);

/* The constants 2 and 3 take the expected branch just like 1 does, so only
   the edge that picks 0 is unlikely.  */

int
m1 (int a, int b)
{
  int r;
  if (a)
    r = 2;
  else if (b)
    r = 3;
  else
    r = 0;
  if (__builtin_expect (r, 1))
    {
      fast ();
      return 1;
    }
  slow ();
  return 0;
}

/* Both arms of the test of x pick an unexpected constant, so the hint
   describes the test of err and not the test of x.  */

int
m2 (int err, int x)
{
  int v;
  if (err)
    {
      if (x)
	{
	  ext (1);
	  v = 1;
	}
      else
	{
	  ext (2);
	  v = 2;
	}
    }
  else
    v = 0;
  if (__builtin_expect (v, 0))
    {
      slow ();
      return v;
    }
  fast ();
  return 0;
}

/* The hint is only reached when mode is zero, so it says nothing about
   a || b on the path that returns early.  */

int
m3 (int a, int b, int mode)
{
  int v = ext (a) || ext (b);
  if (mode)
    return ext (v);
  if (unlikely (v))
    slow ();
  return 0;
}

/* The block that sets v to 1 is entered from a block whose other successor
   leaves the loop.  */

int
m4 (int *p, int n)
{
  int s = 0;
  for (int i = 0; i < n; i++)
    {
      int v;
      if (p[i] < 0)
	{
	  if (ext (i))
	    break;
	  v = 1;
	}
      else
	v = 0;
      if (unlikely (v))
	slow ();
      s += p[i];
    }
  return s;
}

/* As m4 without a loop: the other successor returns early.  */

int
m5 (int x, int a)
{
  int v;
  if (x < 0)
    {
      if (ext (a))
	return -1;
      v = 1;
    }
  else
    v = 0;
  if (unlikely (v))
    slow ();
  return 0;
}

/* Expected predictions: the edge that picks 0 in m1, the test of err in m2
   and each hinted test itself.  */
/* { dg-final { scan-tree-dump-times "__builtin_expect heuristics of edge \[0-9\]+->\[0-9\]+: 10.00%" 6 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-times "__builtin_expect heuristics of edge \[0-9\]+->\[0-9\]+: 90.00%" 1 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-not "Lowered guessed probability" "profile_estimate" } } */
