/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 register names" { ! aarch64*-*-* } } */
/* { dg-lto-options {{-O2 -flto}} } */

/* The second file is compiled with -ffixed-x15 and the third one with
   -fcall-saved-x15.  The link-time compiler must keep x15 fixed, whatever
   the order in which the files name it.  */

extern long fixed (long, long);
extern long saved (long);

int
main (void)
{
  if (fixed (3, 5) != 3 * 5 + 3 + 5 || saved (2) != 3)
    __builtin_abort ();
  return 0;
}
