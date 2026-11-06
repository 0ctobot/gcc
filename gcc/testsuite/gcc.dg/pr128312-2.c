/* PR tree-optimization/128312 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-gimple" } */

/* An initializer that cannot run again in the same lifetime of the variable
   still uses the variable as the return slot of the call: a loop body is a new
   block on each iteration, a label before the block ends the lifetime when
   jumped to, and a label after the declaration doesn't re-run it.  */

struct S { long a[16]; };

struct S f (long);
void use (struct S *);

void
none (long n)
{
  for (int i = 0; i < n; i++)
    {
      struct S d = f (i);
      use (&d);
    }
}

void
outer (long n)
{
back:;
  {
    struct S d = f (n);
    use (&d);
    if (n--)
      goto back;
  }
}

void
inner (long n)
{
  struct S d = f (n);
  use (&d);
lab:
  if (n--)
    {
      use (&d);
      goto lab;
    }
}

void
rerun (long n)
{
back:;
  struct S d = f (n);
  use (&d);
  if (n--)
    goto back;
}

/* { dg-final { scan-tree-dump-times "return slot optimization" 3 "gimple" } } */
