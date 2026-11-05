/* PR rtl-optimization/128427 */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-code-hoisting -fno-tree-pre -fno-caller-saves" } */

/* A division may trap as far as the RTL passes know, even though sdiv never
   does.  Head merging must not move it above anything that may keep the
   program from reaching the division, so each function keeps two copies.  */

/* -fno-caller-saves keeps the operands and the result in call-saved registers.
   Otherwise the two copies can use different registers depending on the tuning,
   and they wouldn't be candidates for head merging in the first place.  */

extern volatile int sink, vload, vstore;
extern void g (void);
extern void maybe_exit (void);

int
across_call (int c, int x, int y)
{
  int r;

  if (c)
    goto out;
  maybe_exit ();
  if (vload)
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

/* Everything here is unsigned since a signed induction variable would let the
   compiler prove that this loop terminates, and the call to it wouldn't be
   marked as looping.  */
__attribute__ ((noinline, noclone)) static int
collatz (unsigned int n)
{
  unsigned int i = 0;

  while (n != 1)
    {
      n = (n & 1) ? 3 * n + 1 : n / 2;
      i++;
    }
  return i;
}

int
across_looping_const (int c, int d, int x, int y, int n)
{
  int r;

  if (c)
    goto out;
  d += collatz (n);
  if (d)
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

int
across_volatile_load (int c, int x, int y)
{
  int r;

  if (c)
    goto out;
  if (vload)
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

int
across_volatile_store (int c, int d, int x, int y)
{
  int r;

  if (c)
    goto out;
  vstore = 1;
  if (d)
    {
      r = x / y;
      g ();
      sink += r + x + y;
      return r - 1;
    }
out:
  r = x / y;
  g ();
  return r * 3 + x + y;
}

/* { dg-final { scan-assembler-times {\tsdiv\t} 8 } } */
