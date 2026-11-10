/* { dg-options "-fno-lto" } */

void
ext (void)
{
  __asm__ volatile ("mov x9, #0" : : : "x9");
}
