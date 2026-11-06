/* { dg-do run } */
/* { dg-require-effective-target label_values } */
/* { dg-require-effective-target indirect_jumps } */

#include <setjmp.h>

/* K occurs in an abnormal PHI, which hides it from ranger.  The PHI analyzer
   must not count "K op X" as a modifier of the group containing X, since
   folding that statement would apply the iteration range to K instead of X.  */

__attribute__((noipa)) int
div_goto (int n, int sel)
{
  static const void *tab[] = { &&L0, &&L1 };
  int k = 3;
  if (sel & 2)
    {
      k = 4096;
      goto *tab[sel & 1];
    }
 L0:
  {
    int x = 1;
    int i;
    for (i = 0; i < n; i++)
      x = k / x;
    if (x > 1000)
      return 1;
    return 2;
  }
 L1:
  return 5;
}

__attribute__((noipa)) int
rshift_goto (int n, int sel)
{
  static const void *tab[] = { &&L0, &&L1 };
  int k = 3;
  if (sel & 2)
    {
      k = 4096;
      goto *tab[sel & 1];
    }
 L0:
  {
    int x = 1;
    int i;
    for (i = 0; i < n; i++)
      x = k >> x;
    if (x > 1000)
      return 1;
    return 2;
  }
 L1:
  return 5;
}

__attribute__((noipa)) int
mod_goto (int n, int sel)
{
  static const void *tab[] = { &&L0, &&L1 };
  int k = 3;
  if (sel & 2)
    {
      k = -4096;
      goto *tab[sel & 1];
    }
 L0:
  {
    int x = 1000;
    int i;
    for (i = 0; i < n; i++)
      x = k % x;
    if (x > 0)
      return 1;
    return 2;
  }
 L1:
  return 5;
}

static jmp_buf buf;

__attribute__((noipa)) void
maybe_longjmp (int sel)
{
  if (sel & 2)
    longjmp (buf, 1);
}

__attribute__((noipa)) int
div_setjmp (int n, int sel, int big)
{
  int k = 3;
  if (big)
    k = 4096;
  if (setjmp (buf))
    return 7;
  maybe_longjmp (sel);
  {
    int x = 1;
    int i;
    for (i = 0; i < n; i++)
      x = k / x;
    if (x > 1000)
      return 1;
    return 2;
  }
}

int
main (void)
{
  if (div_goto (1, 2) != 1 || div_goto (1, 0) != 2)
    __builtin_abort ();
  if (rshift_goto (1, 2) != 1 || rshift_goto (1, 0) != 2)
    __builtin_abort ();
  if (mod_goto (1, 2) != 2 || mod_goto (1, 0) != 1)
    __builtin_abort ();
  if (div_setjmp (1, 0, 1) != 1 || div_setjmp (1, 0, 0) != 2
      || div_setjmp (1, 2, 0) != 7)
    __builtin_abort ();
  return 0;
}
