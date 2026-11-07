/* PR c/100529 */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

/* Each arm declares its own type sized from n, and an arm changes n before it
   declares its type: through a pointer that gets the address of n after the
   conditional and reaches it through a backward goto, or through a nested
   function defined after the conditional.  The size of the result has to come
   from n as the arm that runs sees it.  */

extern void abort (void);

#define ARM(x) ({ x; struct T { char a[n]; } t; t; })

__attribute__ ((noipa)) unsigned long
goto_addr (int n, int c)
{
  int *volatile p = 0;
  unsigned long s;
 again:
  s = sizeof (c ? ARM (if (p) *p = 6) : ARM ());
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
  unsigned long s = sizeof (c ? ARM (set ()) : ARM ());
  void set (void) { n = 6; }
  return s;
}

int
main (void)
{
  if (goto_addr (5, 1) != 6 || goto_addr (5, 0) != 5)
    abort ();
  if (later_nested (5, 1) != 6 || later_nested (5, 0) != 5)
    abort ();
  return 0;
}
