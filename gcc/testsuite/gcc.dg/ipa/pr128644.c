/* PR ipa/128644 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* After IPA-CP specializes find for K == 1000, final value replacement
   turns the second "return f" into "return l".  L must not keep its IPA-time
   "not returned" flag once the late body returns it.  */

static char *
find (char *f, char *l, int k)
{
  for (; f != l; ++f)
    {
      int c = *f;
      if (c == k || c == k + 1 || c == k * 3 || c == k - 7
	  || c == k * 5 + 2 || c == k / 3 || c == (k ^ 5) || c == k * 7 - 1)
	return f;
    }
  return f;
}

int
main (void)
{
  char x[2] = { 0 };
  char *r = find (x, x + 1, 1000);
  char *s = find (x, x + 2, 1000);
  if (r != x + 1 || s != x + 2)
    __builtin_abort ();
  return 0;
}
