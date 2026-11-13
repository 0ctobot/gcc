/* PR ipa/128644 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Final value replacement turns "return f" into "return l" after the
   modref IPA pass has recorded that L isn't returned.  */

int sum;

static __attribute__ ((noinline)) char *
walk (char *f, char *l)
{
  int s = 0;
  for (; f != l; ++f)
    s += *f;
  sum = s;
  return f;
}

int
main (void)
{
  char x[2] = { 0 };
  char *r = walk (x, x + 1);
  if (r != x + 1)
    __builtin_abort ();
  return 0;
}
