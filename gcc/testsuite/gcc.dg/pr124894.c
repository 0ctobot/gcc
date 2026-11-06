/* PR target/124894 */
/* { dg-do run } */
/* { dg-options "-O2 -fno-strict-aliasing" } */
/* { dg-require-effective-target int128 } */
/* { dg-require-effective-target lp64 } */

short s;
__int128 z;
long g;

long
foo (short a)
{
  long t = 0;
  char c = *(char *) __builtin_memset (&z, 2, 6);
  __builtin_memset (&s, c, 2);
  __builtin_memmove (&t, &g, 4);
  long u = -t;
  long v = *(long *) __builtin_memset (&t, a | 6, 8);
  __int128 w = z % s;
  long r = w + t + u + v;
  return r;
}

int
main ()
{
  long x = foo (0);
  if (x != 0x0c0c0c0c0c0c0c0c)
    __builtin_abort();
}
