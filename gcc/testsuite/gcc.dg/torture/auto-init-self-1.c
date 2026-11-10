/* { dg-do run } */
/* { dg-additional-options "-ftrivial-auto-var-init=zero -Wno-uninitialized" } */

/* A variable whose initializer reads the variable itself still gets zeroed
   before the initializer runs.  */

struct s3 { int a, b, c; long d; };

__attribute__((noipa)) void
dirty (void)
{
  volatile unsigned char buf[512];
  int i;

  for (i = 0; i < 512; i++)
    buf[i] = 0xa5;
}

__attribute__((noipa)) int
check_zero (const void *p, unsigned long n)
{
  const unsigned char *q = p;
  unsigned long i;

  for (i = 0; i < n; i++)
    if (q[i])
      return 1;
  return 0;
}

__attribute__((noipa)) int
read_int (int *p)
{
  return *p;
}

__attribute__((noipa)) int
self_int (int c)
{
  int x = x;
  if (c)
    x = 1;
  return x;
}

__attribute__((noipa)) int
self_expr (int c)
{
  int x = x + 1;
  if (c)
    x = 5;
  return x;
}

__attribute__((noipa)) int
self_struct (void)
{
  struct s3 s = s;
  return check_zero (&s, sizeof s);
}

__attribute__((noipa)) int
self_addr (void)
{
  int x = read_int (&x);
  return x;
}

int
main (void)
{
  dirty ();
  if (self_int (0) != 0)
    __builtin_abort ();
  dirty ();
  if (self_expr (0) != 1)
    __builtin_abort ();
  dirty ();
  if (self_struct () != 0)
    __builtin_abort ();
  dirty ();
  if (self_addr () != 0)
    __builtin_abort ();
  return 0;
}
