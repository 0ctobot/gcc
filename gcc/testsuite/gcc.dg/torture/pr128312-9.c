/* PR middle-end/128312 */
/* { dg-do run } */

/* When a jump back runs an initializer again, the variable holds its old value
   until the new one has been evaluated.  Storing the new one in place, which
   clears the variable first, is only wrong if the old value can be seen, but
   neither a longjmp nor a variable with its address taken is needed for that.  */

struct T { int k; int pad0; long a[6]; };

__attribute__ ((noipa)) long
g (long x)
{
  return x;
}

__attribute__ ((noipa)) long
result (struct T *v)
{
  return v->k * 10000L + v->a[2] * 100 + v->a[5];
}

/* A goto out of a compound literal abandons it.  */

__attribute__ ((noipa)) long
goto_literal (long n)
{
  int first = 1;
back:;
  struct T v = (struct T) { .k = (int) n + 1, .a[2] = ({ if (!first) goto out; 55; }), .a[5] = 66 };
  first = 0;
  n += 7;
  goto back;
out:
  return v.k * 10000L + v.a[2] * 100 + v.a[5];
}

__attribute__ ((noipa)) long
goto_braces (long n)
{
  int first = 1;
back:;
  struct T v = { .k = (int) n + 1, .a[2] = ({ if (!first) goto out; 55; }), .a[5] = 66 };
  first = 0;
  n += 7;
  goto back;
out:
  return v.k * 10000L + v.a[2] * 100 + v.a[5];
}

#if defined (__aarch64__) || defined (__x86_64__)
__attribute__ ((noipa)) long
asm_goto_literal (long n)
{
  int again = 0;
back:;
  struct T v = (struct T) { .k = (int) n + 1,
#ifdef __aarch64__
			    .a[2] = ({ __asm__ goto ("cbnz %w0, %l1" : : "r" (again) : : out); 55; }),
#else
			    .a[2] = ({ __asm__ goto ("testl %0, %0\n\tjnz %l1" : : "r" (again) : "cc" : out); 55; }),
#endif
			    .a[5] = 66 };
  again = 1;
  n += 7;
  goto back;
out:
  return v.k * 10000L + v.a[2] * 100 + v.a[5];
}
#endif

/* The initializer reads the variable by name.  */

__attribute__ ((noipa)) long
by_name_call (long n)
{
  int i = 0;
back:;
  struct T v = (struct T) { .k = (int) n + 1, .a[0] = g (i ? v.k : 0), .a[5] = 3 };
  if (i++ < 1)
    {
      n += 5;
      goto back;
    }
  return v.k * 1000L + v.a[0];
}

__attribute__ ((noipa)) long
by_name (long n)
{
  int i = 0;
back:;
  struct T v = (struct T) { .k = (int) n + 1, .a[0] = i ? v.k : 0 };
  if (i++ < 1)
    {
      n += 5;
      goto back;
    }
  return v.k * 1000L + v.a[0];
}

/* The literal has no side effects but reads the variable through a pointer.  */

__attribute__ ((noipa)) long
by_pointer (long n)
{
  struct T *p = 0;
again:;
  struct T v = (struct T) { .k = (int) n + 1, .a[3] = p ? p->k : -1 };
  p = &v;
  if (n++ < 1)
    goto again;
  return v.a[3];
}

int
main (void)
{
  if (goto_literal (1) != 25566)
    __builtin_abort ();
  if (goto_braces (1) != 25566)
    __builtin_abort ();
#if defined (__aarch64__) || defined (__x86_64__)
  if (asm_goto_literal (1) != 25566)
    __builtin_abort ();
#endif
  if (by_name_call (1) != 7002)
    __builtin_abort ();
  if (by_name (1) != 7002)
    __builtin_abort ();
  if (by_pointer (0) != 1)
    __builtin_abort ();
  return 0;
}
