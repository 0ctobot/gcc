/* { dg-lto-do run } */
/* { dg-skip-if "uses aarch64 assembly" { ! aarch64*-*-* } } */
/* { dg-require-effective-target elf } */
/* { dg-lto-options {{-O2 -flto}} } */

/* Only the second file is compiled with -fcall-used-x19.  call_check from
   the third file is compiled without LTO and keeps a value in x19 across
   its call to clobber, as the default convention allows, so the link-time
   compiler must still make clobber save and restore x19.  */

extern int call_check (void);
extern long used (long);

__attribute__ ((noipa, used)) void
clobber (void)
{
  __asm__ volatile ("mov x19, #0" : : : "x19");
}

int
main (void)
{
  if (used (2) != 3 || !call_check ())
    __builtin_abort ();
  return 0;
}
