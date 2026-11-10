/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto -fstrict-overflow}} } */
/* { dg-suppress-ld-options {-fstrict-overflow} } */

/* Only the second file is compiled with -fwrapv, and the link command line
   names neither option.  The link-time compiler must get the union of the
   two settings, which is -fwrapv, even though the first object doesn't
   wrap.  Otherwise IPA-CP drops INT_MIN from the range of g's parameter and
   folds g's y > 0 test to true.  */

extern int h1 (void), h2 (void);

int
main (void)
{
  if (h1 () != 1)
    __builtin_abort ();
  if (h2 () != 2)
    __builtin_abort ();
  return 0;
}
