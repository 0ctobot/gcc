/* A store through a pointer taken from one name of a variable must be seen by
   a direct read through any other name, even if that name is static.  */
/* { dg-do run } */
/* { dg-require-alias "" } */
/* { dg-options "-O2" } */

/* Pointer taken from the target, static alias read directly.  */
static int s1;
static int s1a __attribute__ ((alias ("s1")));

__attribute__ ((noipa)) int *
get1 (void)
{
  return &s1;
}

__attribute__ ((noinline)) int
test1 (int *p)
{
  s1a = 1;
  *p = 2;
  return s1a;
}

/* The same with a public target.  */
int s2;
static int s2a __attribute__ ((alias ("s2")));

__attribute__ ((noinline)) int
test2 (int *p)
{
  s2a = 1;
  *p = 2;
  return s2a;
}

/* Pointer taken from one alias, its sibling read directly.  */
static int s3;
static int s3a __attribute__ ((alias ("s3")));
static int s3b __attribute__ ((alias ("s3")));

__attribute__ ((noipa)) int *
get3 (void)
{
  return &s3a;
}

__attribute__ ((noinline)) int
test3 (int *p)
{
  s3b = 1;
  *p = 2;
  return s3b;
}

/* Pointer taken from the end of an alias chain, its start read directly.  */
static int s4;
static int s4a __attribute__ ((alias ("s4")));
static int s4b __attribute__ ((alias ("s4a")));

__attribute__ ((noipa)) int *
get4 (void)
{
  return &s4b;
}

__attribute__ ((noinline)) int
test4 (int *p)
{
  s4a = 1;
  *p = 2;
  return s4a;
}

int
main (void)
{
  if (test1 (get1 ()) != 2)
    __builtin_abort ();
  if (test2 (&s2) != 2)
    __builtin_abort ();
  if (test3 (get3 ()) != 2)
    __builtin_abort ();
  if (test4 (get4 ()) != 2)
    __builtin_abort ();
  return 0;
}
