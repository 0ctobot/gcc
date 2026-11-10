/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto -fno-strict-overflow}} } */
/* { dg-suppress-ld-options {-fno-strict-overflow} } */

/* Both files are compiled with -fno-strict-overflow, but the link command
   line doesn't repeat it, which is how the Linux kernel links.  IPA-CP must
   still let x + 1 wrap when it computes the range of g's parameter, or it
   drops INT_MIN from that range and folds g's y > 0 test to true.  */

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
