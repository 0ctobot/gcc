/* { dg-do run { target c++11 } } */

[[gnu::noinline]] static unsigned
f (unsigned x)
{
  return x;
}

[[gnu::noinline]] static unsigned
g (int x)
{
  return f (x > 0 ? x : 0);
}

int
main ()
{
  if (g (1) + g (0) != 1)
    __builtin_abort ();
}
