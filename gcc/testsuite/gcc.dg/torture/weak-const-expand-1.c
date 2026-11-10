/* A weak definition of a const object can be replaced at link time by a
   strong definition from another translation unit, so RTL expansion
   mustn't materialize the object from the weak initializer when it's read
   or passed as an argument.  The strong definitions are in
   weak-const-expand-1a.c.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-additional-sources "weak-const-expand-1a.c" } */

struct s2 { int a, b; };
struct s3 { char a, b, c; };
struct s12 { int a, b, c; };

__attribute__ ((weak)) const struct s2 w2 = { 7, 8 };
__attribute__ ((weak)) const struct s3 w3 = { 1, 2, 3 };
__attribute__ ((weak)) const struct s12 w12 = { 4, 5, 6 };

__attribute__ ((noipa)) int
get_b (void)
{
  return w2.b;
}

__attribute__ ((noipa)) int
sum2 (struct s2 v)
{
  return v.a + v.b;
}

__attribute__ ((noipa)) int
sum3 (struct s3 v)
{
  return v.a + v.b + v.c;
}

/* The eight scalar arguments use up the argument registers on most
   targets, so V is passed on the stack.  */

__attribute__ ((noipa)) int
sum12 (long r0, long r1, long r2, long r3, long r4, long r5, long r6,
       long r7, struct s12 v)
{
  return r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + v.a + v.b + v.c;
}

__attribute__ ((noipa)) int
sum3_stack (long r0, long r1, long r2, long r3, long r4, long r5, long r6,
	    long r7, struct s3 v)
{
  return r0 + r1 + r2 + r3 + r4 + r5 + r6 + r7 + v.a + v.b + v.c;
}

__attribute__ ((noipa)) int
pass2 (void)
{
  return sum2 (w2);
}

__attribute__ ((noipa)) int
pass3 (void)
{
  return sum3 (w3);
}

__attribute__ ((noipa)) int
pass12 (void)
{
  return sum12 (0, 0, 0, 0, 0, 0, 0, 0, w12);
}

__attribute__ ((noipa)) int
pass3_stack (void)
{
  return sum3_stack (0, 0, 0, 0, 0, 0, 0, 0, w3);
}

int
main (void)
{
  if (get_b () != 80)
    __builtin_abort ();
  if (pass2 () != 150)
    __builtin_abort ();
  if (pass3 () != 60)
    __builtin_abort ();
  if (pass12 () != 150)
    __builtin_abort ();
  if (pass3_stack () != 60)
    __builtin_abort ();
  return 0;
}
