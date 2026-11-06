/* PR rtl-optimization/125728 */
/* { dg-do compile } */
/* { dg-options "-O2 -m68000" } */

_Alignas (int) unsigned char foo;

const unsigned char bar[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };

unsigned char
baz (void)
{
  return bar[foo & 7];
}

/* foo must be read as a byte at its own address.  A longword read of foo-3
   touches bytes outside of foo and starts at an odd address, which faults on
   a 68000.  */
/* { dg-final { scan-assembler-not "foo-" } } */
