/* PR tree-optimization/128312 */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-gimple" } */

/* An initializer that cannot run again in the same lifetime of the variable
   still uses the variable as the return slot of the call: a loop body is a new
   block on each iteration, a label before the block ends the lifetime when
   jumped to, and a label after the declaration doesn't re-run it.  A function
   with nothing to regain control after a longjmp has no use for the old value
   of the variable, since the longjmp ends the frame, unless the variable's
   address gives the callee a way to read the old value.  */

struct S { long a[16]; };

struct S f (long);
void use (struct S *);
void use2 (void *);
void use3 (long);
int sj (void) __attribute__ ((returns_twice));

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
  use3 (d.a[0]);
  if (n--)
    goto back;
}

void
rerun_address (long n)
{
back:;
  struct S d = f (n);
  use (&d);
  if (n--)
    goto back;
}

void
rerun_receiver (long n)
{
back:;
  struct S d = f (n);
  use3 (d.a[0]);
  if (n--)
    {
      if (sj ())
	return;
      goto back;
    }
}

struct P { char c; long l; };

char g (void);

void
ctor (long n)
{
back:;
  struct P x = { .c = g (), .l = n };
  use3 (x.l);
  if (n--)
    goto back;
}

void
ctor_address (long n)
{
back:;
  struct P v = { .c = g (), .l = n };
  use2 (&v);
  if (n--)
    goto back;
}

void
ctor_receiver (long n)
{
back:;
  struct P w = { .c = g (), .l = n };
  use3 (w.l);
  if (n--)
    {
      if (sj ())
	return;
      goto back;
    }
}

/* { dg-final { scan-tree-dump-times "return slot optimization" 4 "gimple" } } */
/* { dg-final { scan-tree-dump-times "\\mx\\.c = " 1 "gimple" } } */
/* { dg-final { scan-tree-dump-not "\\mv\\.c = " "gimple" } } */
/* { dg-final { scan-tree-dump-not "\\mw\\.c = " "gimple" } } */
