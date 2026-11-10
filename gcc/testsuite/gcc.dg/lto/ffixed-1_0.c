/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 register names" { ! aarch64*-*-* } } */
/* { dg-lto-options {{-O2 -flto -ffixed-x18}} } */
/* { dg-suppress-ld-options {-ffixed-x18} } */

/* Both files are compiled with -ffixed-x18 and the second one also with
   -ffixed-x15, but the link command line has neither, which is how the
   Linux kernel links with the shadow call stack enabled.  The link-time
   compiler must still leave both registers alone.  */

extern long many (long, long);

__attribute__ ((noipa)) static long
check (long a, long b)
{
  long r, x15, x18;

  __asm__ volatile ("mov x15, %0\n\tmov x18, %1"
		    : : "r" (0x1515L), "r" (0x1818L));
  r = many (a, b);
  __asm__ volatile ("mov %0, x15\n\tmov %1, x18" : "=r" (x15), "=r" (x18));
  if (x15 != 0x1515 || x18 != 0x1818)
    __builtin_abort ();
  return r;
}

int
main (void)
{
  if (check (3, 5) != 3 * 5 + 3 + 5)
    __builtin_abort ();
  return 0;
}
