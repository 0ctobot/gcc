/* PR ipa/128644 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* Final value replacement turns "return p" into a pointer computed from
   the static chain after the modref IPA pass has recorded that the static
   chain isn't returned.  */

int sum;

int
main (void)
{
  char a[3] = { 0 };

  __attribute__ ((noinline)) char *
  walk (char *p)
  {
    int s = 0;
    for (; p != &a[1]; ++p)
      s += *p;
    sum = s;
    return p;
  }

  char *r = walk (a);
  if (r != &a[1])
    __builtin_abort ();
  return 0;
}
