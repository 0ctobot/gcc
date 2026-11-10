/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-profile_estimate" } */

/* A __builtin_expect () hint on a value that an error path sets goes to the
   branch that enters the error path, even when the error path has branches
   of its own before it sets the value.  Those branches keep the estimates of
   the other heuristics.  */

void slow (void);
void fast (void);
int ext (int);

/* The two arms of the test of x1 meet again before v is set.  */

int
g1 (int err1, int x1)
{
  int v;
  if (err1)
    {
      if (x1)
	ext (1);
      ext (2);
      v = 1;
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

/* The same through an inlined body, whose branches already have guessed
   probabilities.  */

static inline __attribute__ ((always_inline)) int
check (int errc, int xc)
{
  if (errc)
    {
      if (xc)
	ext (1);
      ext (2);
      return 1;
    }
  return 0;
}

int
g2 (int err2, int x2)
{
  if (__builtin_expect (check (err2, x2), 0))
    {
      slow ();
      return 1;
    }
  fast ();
  return 0;
}

/* The error path picks one of several values.  */

int
g3 (int err3, int x3, int y3)
{
  int v;
  if (err3)
    {
      if (x3)
	{
	  if (y3)
	    v = 2;
	  else
	    v = 3;
	}
      else
	{
	  ext (1);
	  v = 1;
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

/* { dg-final { scan-tree-dump-times "if \\(err\[123\]_\[0-9\]+\\(D\\) != 0\\)\[^\n\]*\n\[^\n\]*\\\[10.00%\\\]" 3 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-not "if \\(\[xy\]\[123\]_\[0-9\]+\\(D\\) != 0\\)\[^\n\]*\n\[^\n\]*\\\[\[19\]0.00%\\\]" "profile_estimate" } } */
