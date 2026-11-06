/* PR c/100529 */
/* { dg-do compile } */
/* { dg-options "-fgnu-tm -O2 -std=gnu11 -w" } */

void bar ();

void
tm_arg (int n)
{
  struct S { char a[n]; } x;
  bar (__transaction_atomic (x));
}

void
tm_assign (int n)
{
  struct S { char a[n]; } x, y;
  y = __transaction_atomic (x);
}

int
tm_member (int n)
{
  struct S { char a[n]; } x;
  return (__transaction_atomic (x)).a[0];
}

/* A statement in a transaction reclaims its temporaries in the transaction.  */
void
tm_stmt (int n, int c)
{
  struct S { char a[n]; } x, y;
  __transaction_relaxed { bar (c ? x : y); }
}

void
tm_loop (int n, int c)
{
  struct S { char a[n]; } x, y;
  for (int i = 0; i < n; i++)
    __transaction_relaxed { bar (c ? x : y); }
}
