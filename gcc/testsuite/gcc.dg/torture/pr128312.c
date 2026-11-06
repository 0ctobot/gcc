/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "" } */
/* { dg-require-effective-target indirect_jumps } */
/* { dg-require-effective-target label_values } */

/* A goto back to a label inside the block of a declaration doesn't end the
   lifetime of the variable, so the variable can hold a value when its
   initializer runs again.  The callee abandons the second run with a longjmp
   after storing to its result, which must leave the old value in place.  */

#include <setjmp.h>

extern void abort (void);

struct S { long a[16]; };
struct O { long k; struct S s; long z; };

jmp_buf env;
volatile int thr;

__attribute__ ((noipa)) void
op (void)
{
  if (thr)
    longjmp (env, 1);
}

__attribute__ ((noipa)) struct S
f (long v)
{
  struct S r;
  for (int i = 0; i < 16; i++)
    r.a[i] = v;
  op ();
  r.a[15] = v + 1;
  return r;
}

__attribute__ ((noipa)) long
plain (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  back:;
  struct S d = f (n + 1);
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->a[0];
}

/* The variable doesn't have to be addressable.  */

__attribute__ ((noipa)) long
direct (int c)
{
  volatile int n = 0;
back:;
  struct S d = f (n + 1);
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	{
	  thr = 0;
	  return d.a[0];
	}
      thr = 1;
      goto back;
    }
  return -1;
}

/* The label can be in a block that already closed.  */

__attribute__ ((noipa)) long
nested (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  {
  back:;
  }
  struct S d = f (n + 1);
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->a[0];
}

/* A computed goto can reach it too.  */

__attribute__ ((noipa)) long
computed (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  void *volatile l = &&back;
  back:;
  struct S d = f (n + 1);
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto *l;
    }
done:
  thr = 0;
  return p->a[0];
}

/* The call doesn't have to be the whole initializer.  */

__attribute__ ((noipa)) long
cond (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  back:;
  struct S d = c ? f (n + 1) : f (n + 2);
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->a[0];
}

__attribute__ ((noipa)) long
comma (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  back:;
  struct S d = (c++, f (n + 1));
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->a[0];
}

__attribute__ ((noipa)) long
stmt_expr (int c)
{
  volatile int n = 0;
  struct S *volatile p = 0;
  back:;
  struct S d = ({ f (n + 1); });
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->a[0];
}

__attribute__ ((noipa)) long
member (int c)
{
  volatile int n = 0;
  struct O *volatile p = 0;
  back:;
  struct O d = { .k = 1, .s = f (n + 1), .z = 2 };
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->s.a[0];
}

__attribute__ ((noipa)) long
member_clear (int c)
{
  volatile int n = 0;
  struct O *volatile p = 0;
  back:;
  struct O d = { .s = f (n + 1) };
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->s.a[0];
}

__attribute__ ((noipa)) long
literal (int c)
{
  volatile int n = 0;
  struct O *volatile p = 0;
  back:;
  struct O d = (struct O) { .k = 1, .s = f (n + 1) };
  p = &d;
  if (n == 0)
    {
      n = 1;
      if (setjmp (env))
	goto done;
      thr = 1;
      goto back;
    }
done:
  thr = 0;
  return p->s.a[0];
}

/* A setjmp before the declaration can bring control back as well, and a
   volatile variable keeps its value across the longjmp to it.  */

__attribute__ ((noipa)) long
via_setjmp (int c)
{
  volatile int n = 0;
  volatile struct S *volatile p = 0;
  if (setjmp (env) && n == 2)
    {
      thr = 0;
      return p->a[0];
    }
  n = n + 1;
  volatile struct S d = f (n);
  p = &d;
  thr = 1;
  longjmp (env, 1);
}

int
main (void)
{
  if (plain (1) != 1
      || direct (1) != 1
      || nested (1) != 1
      || computed (1) != 1
      || cond (1) != 1
      || comma (1) != 1
      || stmt_expr (1) != 1
      || member (1) != 1
      || member_clear (1) != 1
      || literal (1) != 1
      || via_setjmp (1) != 1)
    abort ();
  return 0;
}
