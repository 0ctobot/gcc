/* PR tree-optimization/128312 */
/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */
/* { dg-require-effective-target indirect_jumps } */

/* Automatic variable initialization promises zeroed padding in a variable with
   an initializer.  The padding must be zero whether or not the function can
   regain control after a longjmp abandons the initializer, which makes the
   gimplifier build the value in a temporary first.  */

#include <setjmp.h>

extern void abort (void);
extern void *memset (void *, int, __SIZE_TYPE__);

struct P { char c; long l; };
struct A { struct P p[2]; };

jmp_buf env;
void *bbuf[5];

__attribute__ ((noipa)) void
dirty (void)
{
  volatile unsigned char buf[4096];
  for (unsigned i = 0; i < sizeof buf; i++)
    buf[i] = 0xaa;
}

__attribute__ ((noipa)) char
f (void)
{
  return 1;
}

__attribute__ ((noipa)) void
check (const struct P *p, long l)
{
  const volatile unsigned char *b = (const volatile unsigned char *) p;
  if (p->c != 1 || p->l != l)
    abort ();
  for (unsigned i = 1; i < __builtin_offsetof (struct P, l); i++)
    if (b[i])
      abort ();
}

/* The value and padding of each run of the declaration are checked, and then
   the padding is scribbled on since a goto back doesn't end the lifetime of the
   variable.  */
#define AGAIN(v) \
  do \
    { \
      memset ((void *) &(v), 0xaa, sizeof (v)); \
      if (n++ < 1) \
	goto again; \
    } \
  while (0)

__attribute__ ((noipa)) void
label (long x)
{
  int n = 0;
again:;
  struct P v = { .c = f (), .l = x };
  check (&v, x);
  AGAIN (v);
}

__attribute__ ((noipa)) void
no_label (long x)
{
  struct P v = { .c = f (), .l = x };
  check (&v, x);
}

__attribute__ ((noipa)) void
receiver (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  struct P v = { .c = f (), .l = x };
  check (&v, x);
  AGAIN (v);
}

/* The returns_twice call can come after the declaration.  */
__attribute__ ((noipa)) void
receiver_below (long x)
{
  volatile int n = 0;
again:;
  struct P v = { .c = f (), .l = x };
  check (&v, x);
  memset (&v, 0xaa, sizeof v);
  if (n++ < 1)
    {
      if (setjmp (env))
	abort ();
      goto again;
    }
}

__attribute__ ((noipa)) void
array (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  struct P a[2] = { { f (), x }, { f (), x } };
  check (&a[0], x);
  check (&a[1], x);
  AGAIN (a);
}

__attribute__ ((noipa)) void
member_array (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  struct A a = { { { f (), x }, { f (), x } } };
  check (&a.p[0], x);
  check (&a.p[1], x);
  AGAIN (a);
}

__attribute__ ((noipa)) void
literal (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  struct P v = (struct P) { .c = f (), .l = x };
  check (&v, x);
  AGAIN (v);
}

__attribute__ ((noipa)) void
constant (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  const struct P v = { .c = f (), .l = x };
  check (&v, x);
  if (n++ < 1)
    goto again;
}

__attribute__ ((noipa)) void
aligned (long x)
{
  volatile int n = 0;
  if (setjmp (env))
    abort ();
again:;
  struct P v __attribute__ ((aligned (64))) = { .c = f (), .l = x };
  if ((__UINTPTR_TYPE__) &v % 64)
    abort ();
  check (&v, x);
  AGAIN (v);
}

__attribute__ ((noipa)) void
builtin (long x)
{
  volatile int n = 0;
  if (__builtin_setjmp (bbuf))
    abort ();
again:;
  struct P v = { .c = f (), .l = x };
  check (&v, x);
  AGAIN (v);
}

__attribute__ ((noipa)) void
nested (long x)
{
  __label__ out;
  volatile int n = 0;
  __attribute__ ((noinline)) void
  leave (void)
  {
    goto out;
  }
again:;
  struct P v = { .c = f (), .l = x };
  check (&v, x);
  if (x < 0)
    leave ();
  AGAIN (v);
out:;
}

typedef void (*test_fn) (long);

test_fn tests[] = {
  label, no_label, receiver, receiver_below, array, member_array, literal,
  constant, aligned, builtin, nested
};

int
main (void)
{
  for (unsigned i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
      dirty ();
      tests[i] (i + 100);
    }
  return 0;
}
