/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "" } */

/* The store of an initializer goes down into the arms of a conditional and the
   right operand of a comma, whichever form holds the aggregate.  A longjmp out
   of the second run of the initializer must leave the old value in place.  */

#include <setjmp.h>

extern void abort (void);

struct S { long a, b, c; long pad[7]; };
struct E {};

jmp_buf env;
volatile int n;

__attribute__ ((noipa)) long
g (long x)
{
  if (x)
    longjmp (env, 1);
  return 100;
}

__attribute__ ((noipa)) struct S
mk (long x)
{
  struct S r = { .a = 1, .b = 100, .c = 3 };
  if (x)
    {
      r.a = 50;
      longjmp (env, 1);
    }
  return r;
}

__attribute__ ((noipa)) struct E
fe (void)
{
  struct E e;
  n++;
  return e;
}

__attribute__ ((noipa)) void
check (struct S *p)
{
  if (p->a != 1 || p->b != 100 || p->c != 3)
    abort ();
}

#define LIT { .a = n + 1, .b = g (n), .c = 3 }

#define RERUN(init) \
  do \
    { \
      n = 0; \
    again:; \
      struct S v = init; \
      if (setjmp (env)) \
	{ \
	  check (&v); \
	  return; \
	} \
      if (n++ == 0) \
	goto again; \
      abort (); \
    } \
  while (0)

__attribute__ ((noipa)) void
cond (int c)
{
  RERUN (c ? (struct S) LIT : (struct S) { .a = 0 });
}

__attribute__ ((noipa)) void
cond_other (int c)
{
  RERUN (c ? (struct S) { .a = 0 } : (struct S) LIT);
}

__attribute__ ((noipa)) void
cond_nested (int c, int d)
{
  RERUN (c ? d ? (struct S) LIT : (struct S) { .a = 0 } : (struct S) { .b = 0 });
}

__attribute__ ((noipa)) void
cond_call (int c)
{
  RERUN (c ? mk (n) : (struct S) { .a = 0 });
}

__attribute__ ((noipa)) void
comma (void)
{
  RERUN ((n, (struct S) LIT));
}

__attribute__ ((noipa)) void
comma_call (void)
{
  RERUN ((n, mk (n)));
}

__attribute__ ((noipa)) void
comma_cond (int c)
{
  RERUN ((n, c ? (struct S) LIT : (struct S) { .a = 0 }));
}

__attribute__ ((noipa)) void
stmt_expr (void)
{
  RERUN (({ (struct S) LIT; }));
}

__attribute__ ((noipa)) void
stmt_expr_cond (int c)
{
  RERUN (({ c ? (struct S) LIT : (struct S) { .a = 0 }; }));
}

/* An empty aggregate has no storage to speak of, but it still gets its side
   effects.  */

__attribute__ ((noipa)) void
empty (int c)
{
  n = 0;
again:;
  struct E e = c ? fe () : (fe (), fe ());
  if (n == 1)
    goto again;
  if (setjmp (env))
    abort ();
  if (n != 2)
    abort ();
  __asm__ ("" : : "r" (&e));
}

int
main (void)
{
  cond (1);
  cond_other (0);
  cond_nested (1, 1);
  cond_call (1);
  comma ();
  comma_call ();
  comma_cond (1);
  stmt_expr ();
  stmt_expr_cond (1);
  empty (1);
  return 0;
}
