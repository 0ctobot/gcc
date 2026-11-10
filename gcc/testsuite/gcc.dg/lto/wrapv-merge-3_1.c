/* { dg-options "-fno-strict-overflow" } */

extern void use (const char *);

__attribute__ ((noinline)) int
callee (const char *s)
{
  if (!s)
    return -1;
  use (s);
  return 0;
}
