/* A VLA compound literal can be the object of a compound assignment or of ++
   and --, and the expression for its size must be evaluated just once.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23" } */

int calls;

__attribute__ ((noipa)) int
g (int x)
{
  calls++;
  return x;
}

__attribute__ ((noipa)) int
add (int n)
{
  return (int[n]) {}[0] += 1;
}

__attribute__ ((noipa)) int
add_size_call (void)
{
  return (int[g (2)]) {}[1] += 3;
}

__attribute__ ((noipa)) bool
bool_dec (int n)
{
  return (bool[n]) {}[0]--;
}

__attribute__ ((noipa)) int
complex_inc (int n)
{
  return __real__ ++(_Complex int[n]) {}[0];
}

__attribute__ ((noipa)) int
atomic_add (int n)
{
  return (_Atomic int[n]) {}[0] += 1;
}

__attribute__ ((noipa)) int
member_add (int n)
{
  return (struct { int a[n]; int b; }) {}.b += 1;
}

__attribute__ ((noipa)) int
two_dim_add (int n)
{
  return (int[n][n]) {}[n - 1][0] += 7;
}

__attribute__ ((noipa)) int
real_add (int n)
{
  return __real__ (_Complex int[n]) {}[0] += 1;
}

__attribute__ ((noipa)) int
pointer_to_vla (int n)
{
  int a[n];
  a[0] = 5;
  typedef int (*p)[n];
  p q = (p[2]) {}[1] += 0;
  return q == 0 ? a[0] : 0;
}

int
main (void)
{
  if (add (2) != 1)
    __builtin_abort ();
  if (add_size_call () != 3 || calls != 1)
    __builtin_abort ();
  if (bool_dec (2) != 0)
    __builtin_abort ();
  if (complex_inc (2) != 1)
    __builtin_abort ();
  if (atomic_add (2) != 1)
    __builtin_abort ();
  if (member_add (2) != 1)
    __builtin_abort ();
  if (two_dim_add (2) != 7)
    __builtin_abort ();
  if (real_add (2) != 1)
    __builtin_abort ();
  if (pointer_to_vla (2) != 5)
    __builtin_abort ();
  return 0;
}
