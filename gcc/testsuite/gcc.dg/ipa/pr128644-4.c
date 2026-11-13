/* PR ipa/128644 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Final value replacement turns "*f = 5" into "*l = 5" after the modref
   IPA pass has recorded that memory pointed to by L isn't clobbered.  */

int sum;

static __attribute__ ((noinline)) void
walk (char *f, char *l)
{
  int s = 0;
  for (; f != l; ++f)
    s += *f;
  sum = s;
  *f = 5;
}

int
main (void)
{
  char x[3] = { 0 };
  walk (x, x + 1);
  if (x[1] != 5)
    __builtin_abort ();
  return 0;
}
