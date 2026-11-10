/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 register names" { ! aarch64*-*-* } } */
/* { dg-lto-options {{-O2 -flto}} } */

/* Only the second file is compiled with -fcall-saved-x9.  keep calls ext
   from the third file, which is compiled without LTO and clobbers x9 as the
   default convention allows, so the link-time compiler must not keep a
   value in x9 across that call.  */

extern void ext (void);
extern long saved (long);

__attribute__ ((noipa)) long
keep (long a, long n)
{
  long s = 0;
  for (long i = 0; i < n; i++)
    {
      ext ();
      __asm__ volatile ("" : : : "x19", "x20", "x21", "x22", "x23", "x24",
			"x25", "x26", "x27", "x28");
      s += a ^ i;
    }
  return s;
}

int
main (void)
{
  if (saved (2) != 3 || keep (5, 4) != 5 + 4 + 7 + 6)
    __builtin_abort ();
  return 0;
}
