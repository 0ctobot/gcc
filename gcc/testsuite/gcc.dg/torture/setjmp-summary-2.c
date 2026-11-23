/* { dg-do run } */
/* { dg-require-effective-target indirect_jumps } */

/* A callee that always stores to memory before it returns normally kills
   what the memory held before the call, but it can longjmp back into its
   caller before the store.  The caller then still sees the stores it did
   before the call once setjmp returns again, so they can't be deleted.  */

#include <setjmp.h>

static jmp_buf buf;
volatile int c = 1;

__attribute__((noinline, noclone, noreturn)) static void
jump (void)
{
  longjmp (buf, 1);
}

/* The summary of this records a kill of *Q.  */

__attribute__((noinline, noclone)) static void
set_int (int *q)
{
  if (c)
    jump ();
  *q = 2;
}

__attribute__((noinline, noclone)) static int
f1 (void)
{
  int *x = __builtin_malloc (sizeof (int));

  *x = 5;
  if (setjmp (buf))
    return *x;
  *x = 1;
  set_int (x);
  return *x + 100;
}

/* DSE doesn't track the bytes of a single byte store individually, so this
   checks the store against the kill as a whole.  */

__attribute__((noinline, noclone)) static void
set_char (char *q)
{
  if (c)
    jump ();
  *q = 2;
}

__attribute__((noinline, noclone)) static int
f2 (void)
{
  char *x = __builtin_malloc (1);

  *x = 5;
  if (setjmp (buf))
    return *x;
  *x = 1;
  set_char (x);
  return *x + 100;
}

/* A call that longjmps doesn't store its result either.  */

struct S { int a[8]; };
struct T { char a[8]; };

__attribute__((noinline, noclone)) static struct S
get_s (int k)
{
  if (c)
    jump ();
  struct S s = { { k } };
  return s;
}

__attribute__((noinline, noclone)) static int
f3 (void)
{
  struct S *x = __builtin_malloc (sizeof *x);

  x->a[0] = 5;
  if (setjmp (buf))
    return x->a[0];
  x->a[0] = 1;
  *x = get_s (7);
  return 100;
}

__attribute__((noinline, noclone)) static struct T
get_t (void)
{
  if (c)
    jump ();
  struct T t = { { 7 } };
  return t;
}

__attribute__((noinline, noclone)) static int
f4 (void)
{
  struct T *x = __builtin_malloc (sizeof *x);

  x->a[0] = 5;
  if (setjmp (buf))
    return x->a[0];
  x->a[0] = 1;
  *x = get_t ();
  return 100;
}

int
main (void)
{
  if (f1 () != 1)
    __builtin_abort ();
  if (f2 () != 1)
    __builtin_abort ();
  if (f3 () != 1)
    __builtin_abort ();
  if (f4 () != 1)
    __builtin_abort ();
  return 0;
}
