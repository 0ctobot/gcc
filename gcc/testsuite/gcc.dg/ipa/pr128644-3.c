/* PR ipa/128644 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Final value replacement turns "g = f" into "g = l" after the modref IPA
   pass has recorded that L doesn't escape.  */

char *g;
int sum;

static __attribute__ ((noinline)) void
walk (char *f, char *l)
{
  int s = 0;
  for (; f != l; ++f)
    s += *f;
  sum = s;
  g = f;
}

int
main (void)
{
  char x[2] = { 0 };
  walk (x, x + 1);
  if (g != x + 1)
    __builtin_abort ();
  return 0;
}
