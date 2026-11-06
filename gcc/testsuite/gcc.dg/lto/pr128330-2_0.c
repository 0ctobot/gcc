/* PR middle-end/128330: -fstack-protector-strong always adds a guard here, but
   it must also place the plain char array above the int array, next to the
   guard, so that overflowing the character array cannot reach the other array
   first.  Under LTO the plain char array used to be placed like any other.  */
/* { dg-lto-do run } */
/* { dg-require-effective-target fstack_protector } */
/* { dg-skip-if "needs a downward growing frame" { ! { aarch64*-*-* i?86-*-* x86_64-*-* } } } */
/* { dg-lto-options { { -O2 -flto -fstack-protector-strong } { -O2 -flto -fstack-protector-strong -fsigned-char } { -O2 -flto -fstack-protector-strong -funsigned-char } } } */

__attribute__ ((noipa)) void
sink (void *a, void *b)
{
  __asm__ volatile ("" : : "r" (a), "r" (b) : "memory");
}

__attribute__ ((noipa)) int
char_array_above_int_array (void)
{
  int ia[8];
  char cb[8];

  sink (ia, cb);
  return (unsigned long) cb > (unsigned long) ia;
}

int
main (void)
{
  return !char_array_above_int_array ();
}
