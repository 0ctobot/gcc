/* PR middle-end/100529 */
/* { dg-do run } */
/* { dg-additional-options "-std=gnu11 -w" } */

/* The stack space of a variable-sized TARGET_EXPR temporary is reclaimed at the
   end of its full expression, without help from the enclosing block.  */

extern void abort (void);
extern void *memset (void *, int, __SIZE_TYPE__);

static void *first;

/* Abort if the stack pointer moved since the last reset.  */

__attribute__ ((noipa)) void
check_frame (int reset, ...)
{
  void *f = __builtin_frame_address (0);

  if (reset)
    first = f;
  else if (first != f)
    abort ();
}

__attribute__ ((noipa)) void
clobber_stack (void)
{
  volatile char buf[4096];

  memset ((void *) buf, 0x55, sizeof (buf));
}

__attribute__ ((noipa)) int
use (const char *p, int n)
{
  clobber_stack ();
  return p[n - 1];
}

/* A jump over the statement with the temporary, to a label in the same block,
   must not end up restoring a stack pointer that was never saved.  */

__attribute__ ((noipa)) int
t_jump (int n, int skip)
{
  struct S { char a[n]; } x;
  int v = 0;

  memset (&x, 1, n);
  if (skip)
    goto l;
  {
    v = use (({ struct S t; t = x; t; }).a, n);
  l:
    v += 2;
  }
  return v;
}

/* A jump into an if statement's arm that skips its predicate's temporary.  */

__attribute__ ((noipa)) int
t_if (int n, int skip)
{
  struct S { char a[n]; } x;
  int v = 0;

  memset (&x, 1, n);
  if (skip)
    goto l;
  if (({ struct S t; t = x; t; }).a[n - 1])
    {
    l:
      v += 2;
    }
  return v;
}

/* The space must not accumulate across the iterations of a loop.  */

__attribute__ ((noipa)) void
t_loop (int n)
{
  struct S { char a[n]; } x;

  memset (&x, 1, n);
  check_frame (1);
  for (int i = 0; i < 3; i++)
    {
      use (({ struct S t; t = x; t; }).a, n);
      check_frame (0);
    }
}

/* Likewise when the statement expression can leave the iteration early.  */

__attribute__ ((noipa)) void
t_continue (int n, int stop)
{
  struct S { char a[n]; } x, y;

  memset (&x, 1, n);
  memset (&y, 2, n);
  check_frame (1);
  for (int i = 0; i < 3; i++)
    {
      check_frame (0);
      use (({ if (stop) continue; y; }).a, n);
    }
  check_frame (0);
}

/* Likewise with a statement that has the temporaries of a ?: operator as well.  */

__attribute__ ((noipa)) void
t_mixed (int n, int c)
{
  struct S { char a[n]; } x, y;

  memset (&x, 1, n);
  memset (&y, 2, n);
  check_frame (1);
  for (int i = 0; i < 3; i++)
    {
      if (use ((c ? x : y).a, n) + use (({ struct S t; t = y; t; }).a, n) != 4 - c)
	abort ();
      check_frame (0);
    }
}

/* Likewise in a loop's condition, also with a jump into the loop's body.  */

__attribute__ ((noipa)) int
t_while (int n, int skip)
{
  struct S { char a[n]; } x;
  int i = 0;

  memset (&x, 1, n);
  check_frame (1);
  if (skip)
    goto l;
  while (({ struct S t; t = x; t; }).a[n - 1] && i < 3)
    {
    l:
      check_frame (0);
      i++;
    }
  return i;
}

/* Likewise in a loop inside of a statement expression.  */

__attribute__ ((noipa)) void
t_inner (int n)
{
  struct S { char a[n]; } x;

  memset (&x, 1, n);
  check_frame (1);
  int v = ({ for (int i = 0; i < 3; i++)
	       {
		 use (({ struct S t; t = x; t; }).a, n);
		 check_frame (0);
	       }
	     0; });
  if (v)
    abort ();
}

/* The ?: predicate's temporary is read by the arm, which has its own label.  */

__attribute__ ((noipa)) void
t_pred (int n, int c)
{
  struct S { char a[n]; } x;
  char *volatile p;

  memset (&x, c + 1, n);
  (p = ({ struct S t; t = x; t; }).a)[0]
    ? ({ goto l; l: clobber_stack (); if (p[n - 1] != c + 1) abort (); (void) 0; })
    : (void) 0;
}

/* An __auto_type variable initialized from a statement expression lives until
   the end of its block, so the temporary it uses must survive the statement.  */

__attribute__ ((noipa)) void
t_auto (int n, int c)
{
  struct S { char a[n]; } x;

  memset (&x, c, n);
  __auto_type z = ({ struct S t; t = x; t; });
  clobber_stack ();
  for (int i = 0; i < n; i++)
    if (z.a[i] != c)
      abort ();
}

int
main (void)
{
  if (t_jump (64, 0) != 3 || t_jump (64, 1) != 2)
    abort ();
  if (t_if (64, 0) != 2 || t_if (64, 1) != 2)
    abort ();
  t_loop (256);
  t_continue (256, 0);
  t_continue (256, 1);
  t_mixed (64, 0);
  t_mixed (64, 1);
  if (t_while (64, 0) != 3 || t_while (64, 1) != 3)
    abort ();
  t_inner (256);
  t_pred (64, 0);
  t_pred (64, 1);
  t_auto (64, 3);
  return 0;
}
