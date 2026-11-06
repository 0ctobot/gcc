/* Reading a local without an initializer must give zero with
   -ftrivial-auto-var-init=zero, rather than whichever value is convenient
   for the optimizers.  */
/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized -Wno-maybe-uninitialized" } */

#define NOIPA __attribute__ ((noipa))

/* A single conditional assignment must not become unconditional.  */
NOIPA int
one_const (int c)
{
  int x;
  if (c)
    x = 5;
  return x;
}

/* The kernel's usual error return.  */
NOIPA int
err_const (int bad)
{
  int ret;
  if (bad)
    ret = -22;
  return ret;
}

NOIPA _Bool
search (const int *p, int n, int v)
{
  _Bool found;
  for (int i = 0; i < n; i++)
    if (p[i] == v)
      found = 1;
  return found;
}

/* A comparison of the variable must not fold.  */
NOIPA int
check (int c)
{
  return c + 100;
}

NOIPA int
cmp_unset (int c)
{
  int s;
  if (c)
    s = 3;
  if (s == 3)
    return check (c);
  return 0;
}

/* Scalar replacements of an aggregate get their own marker.  */
struct pair
{
  int a, b;
};

NOIPA int
member (int c)
{
  struct pair p;
  if (c)
    p.a = 7;
  return p.a;
}

/* The two parts of a complex variable are read separately.  */
NOIPA int
complex_part (int c)
{
  _Complex int z;
  __imag__ z = 1;
  if (c)
    __real__ z = 9;
  return __real__ z;
}

/* A value merged around a loop back edge.  */
NOIPA int
loop_merge (int n)
{
  int x;
  for (int i = 0; i < n; i++)
    x = 11;
  return x;
}

int
main (void)
{
  static const int a[2] = { 1, 2 };

  if (one_const (0) != 0 || one_const (1) != 5)
    __builtin_abort ();
  if (err_const (0) != 0 || err_const (1) != -22)
    __builtin_abort ();
  if (search (a, 2, 3) != 0 || search (a, 2, 2) != 1)
    __builtin_abort ();
  if (cmp_unset (0) != 0 || cmp_unset (1) != 101)
    __builtin_abort ();
  if (member (0) != 0 || member (1) != 7)
    __builtin_abort ();
  if (complex_part (0) != 0 || complex_part (1) != 9)
    __builtin_abort ();
  if (loop_merge (0) != 0 || loop_merge (1) != 11)
    __builtin_abort ();
  return 0;
}
