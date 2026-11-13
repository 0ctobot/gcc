/* PR ipa/128469 */
/* { dg-do run } */
/* { dg-options "-O2" } */
/* { dg-require-effective-target indirect_jumps } */

/* Stores done before a longjmp back into a function that called setjmp
   must stay visible to that function's caller.  */

#include <setjmp.h>

jmp_buf buf;
volatile int c = 1;

__attribute__((noinline)) void
jump (int *q)
{
  *q = 0;
  longjmp (buf, 1);
}

__attribute__((noinline)) void
maybe_jump (int *q)
{
  if (c)
    jump (q);
}

__attribute__((noinline)) void
f1 (int *p)
{
  if (setjmp (buf))
    return;
  jump (p);
}

__attribute__((noinline)) void
f2 (int *p)
{
  if (setjmp (buf))
    return;
  maybe_jump (p);
}

int
main (void)
{
  int i;

  i = 1;
  f1 (&i);
  if (i != 0)
    __builtin_abort ();

  i = 1;
  f2 (&i);
  if (i != 0)
    __builtin_abort ();

  return 0;
}
