/* PR c/100529 */
/* { dg-do compile } */
/* { dg-options "-std=gnu23" } */

/* Each arm declares its own type sized from n, so the size of the result can
   only come from n evaluated ahead of the arms, and an arm changes n through
   memory before it declares its type.  What lets the arm change n comes later
   in the source than the conditional, so the conditional is an error once the
   rest of the function shows that n can change.  */

#define ARM(x) ({ x; struct T { char a[n]; } t; t; })

int
goto_addr (int n,	/* { dg-message "computed from 'n'" } */
	   int c)
{
  int *volatile p = 0;
 again:
  (void) (c ? ARM (if (p) *p = 6) : ARM ()); /* { dg-error "type mismatch in conditional expression" } */
  if (!p)
    {
      p = &n;
      goto again;
    }
  return 0;
}

int
later_nested (int n,	/* { dg-message "computed from 'n'" } */
	      int c)
{
  auto void set (void);
  (void) (c ? ARM (set ()) : ARM ()); /* { dg-error "type mismatch in conditional expression" } */
  void set (void) { n = 6; }
  return 0;
}
