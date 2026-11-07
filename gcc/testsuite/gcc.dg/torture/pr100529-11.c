/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* An arm's own type is sized from n after the arm changes n through memory,
   and what lets it do that only comes later in the source: the address of n
   taken after the conditional and reaching it through a backward goto, or a
   nested function that writes n defined after the conditional.  The size of
   t has to be computed from n after the store, rather than ahead of the
   arm.  */

extern void abort (void);

__attribute__ ((noipa)) unsigned long
goto_addr (int n, int c)
{
  int m = 6;
  struct T { char a[m]; } o;
  unsigned long s = 0;
  int *volatile p = 0;
 again:
  (void) (c ? ({ if (p) *p = 6; struct T { char a[n]; } t; s = sizeof t; t; })
	  : o);
  if (!p)
    {
      p = &n;
      goto again;
    }
  return s;
}

__attribute__ ((noipa)) unsigned long
later_nested (int n, int c)
{
  auto void set (void) __attribute__ ((noinline));
  int m = 6;
  struct T { char a[m]; } o;
  unsigned long s = 0;
  (void) (c ? ({ set (); struct T { char a[n]; } t; s = sizeof t; t; }) : o);
  void set (void) { n = 6; }
  return s;
}

int
main (void)
{
  if (goto_addr (5, 1) != 6)
    abort ();
  if (later_nested (5, 1) != 6)
    abort ();
  return 0;
}
