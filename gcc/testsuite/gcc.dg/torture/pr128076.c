/* PR tree-optimization/128076 */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* Tail recursion elimination evaluates the operations in the opposite order of
   the recursion.  None of these functions overflow in the order written, so
   the loops they turn into must not rely on signed overflow being undefined.  */

extern void exit (int);

static int
add (int n)
{
  if (n == 0)
    return 0;
  if (n == 1)
    return add (n - 1) + __INT_MAX__;
  return add (n - 1) - __INT_MAX__;
}

static int
mult (int n)
{
  if (n == 0)
    return 1;
  return -__INT_MAX__ + 1 + __INT_MAX__ * mult (n - 1);
}

/* The additions are signed even though the return type isn't.  */

static unsigned
uadd (int n)
{
  if (n == 0)
    return 0;
  if (n == 1)
    return (int) uadd (n - 1) + __INT_MAX__;
  return (int) uadd (n - 1) - __INT_MAX__;
}

enum e { E_NEG = -1, E_MAX = __INT_MAX__ };

static enum e
eadd (int n)
{
  if (n == 0)
    return (enum e) 0;
  if (n == 1)
    return (enum e) ((int) eadd (n - 1) + __INT_MAX__);
  return (enum e) ((int) eadd (n - 1) - __INT_MAX__);
}

static _Complex int
cadd (int n)
{
  _Complex int k = __INT_MAX__;
  if (n == 0)
    return 0;
  if (n == 1)
    return cadd (n - 1) + k;
  return cadd (n - 1) - k;
}

static int visits;

__attribute__ ((noipa)) void
visit (void)
{
  visits++;
}

__attribute__ ((noipa)) void
leave (void)
{
  if (visits != 10)
    __builtin_abort ();
  exit (0);
}

/* The multiplication is only reached once the recursive call returns, which
   never happens.  Moving it ahead of the call mustn't let its overflow bound
   the number of iterations.  */

__attribute__ ((noipa)) int
hoist (int n, int m)
{
  if (n == m)
    {
      leave ();
      return 0;
    }
  visit ();
  int y = hoist (n + 1, m);
  int z = n * 1000000000;
  return y + z;
}

volatile int three = 3;

int
main (void)
{
  if (add (three) != -__INT_MAX__)
    __builtin_abort ();
  if (mult (2) != 1)
    __builtin_abort ();
  if (uadd (three) != -(unsigned) __INT_MAX__)
    __builtin_abort ();
  if ((int) eadd (three) != -__INT_MAX__)
    __builtin_abort ();
  _Complex int c = cadd (three);
  if (__real__ c != -__INT_MAX__ || __imag__ c != 0)
    __builtin_abort ();
  hoist (0, 10);
  __builtin_abort ();
}
