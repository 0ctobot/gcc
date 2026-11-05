/* PR target/54412 */
/* { dg-do run } */
/* { dg-require-effective-target alloca } */

/* An over-aligned object returned or passed by value lives in a temporary
   stack slot chosen by the caller.  The address of the slot reaches the callee
   as a pointer to the over-aligned type, so the slot needs the full alignment
   even though the frame can't provide it statically.  Each call runs at every
   16-byte multiple of the alignment boundary, so a slot aligned only to the
   frame's alignment can't pass by luck.  */

struct S { long a[8]; } __attribute__ ((aligned (64)));

struct S g;
int fails;

__attribute__ ((noipa)) void
check (const void *p)
{
  if ((__UINTPTR_TYPE__) p & 63)
    fails++;
}

__attribute__ ((noipa)) struct S
ret (long v)
{
  struct S s = { { v } };
  check (&s);
  return s;
}

__attribute__ ((noipa)) long
by_value (struct S s)
{
  check (&s);
  return s.a[0];
}

__attribute__ ((noipa)) long
two_by_value (struct S a, struct S b)
{
  check (&a);
  check (&b);
  return a.a[0] + b.a[0];
}

/* The temporaries from different arms and statements share a slot, which must
   be aligned again wherever it's reused.  */

__attribute__ ((noipa)) long
reuse (long x)
{
  long r;
  if (x & 1)
    r = ret (x).a[0];
  else
    r = ret (x + 1).a[0];
  r += ret (x + 2).a[0];
  r += (x & 2) ? by_value (ret (x)) : by_value (g);
  r += two_by_value (ret (x), g);
  return r;
}

int
main (void)
{
  for (int i = 1; i <= 8; i++)
    {
      char *volatile p = __builtin_alloca (16 * i);
      p[0] = 0;
      reuse (i);
    }
  if (fails)
    __builtin_abort ();
  return 0;
}
