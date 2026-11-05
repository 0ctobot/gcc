/* PR c/125973 */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O2" } */
/* { dg-require-effective-target tls_runtime } */
/* { dg-add-options tls } */

/* The thread storage duration variant of pr125973.c.  */

extern void abort (void);

__attribute__ ((noinline)) static int
bool_predec (void)
{
  return --(static thread_local bool){};
}

__attribute__ ((noinline)) static int
int_add (void)
{
  return (static thread_local int){} += 1;
}

__attribute__ ((noinline)) static int
gnu_tls_bool_predec (void)
{
  return --(static __thread bool){};
}

int
main (void)
{
  for (int i = 0; i < 4; i++)
    if (bool_predec () != !(i & 1)
	|| int_add () != i + 1
	|| gnu_tls_bool_predec () != !(i & 1))
      abort ();
  return 0;
}
