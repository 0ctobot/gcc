/* PR target/122453 */
/* This is apart from pr122453.c because once an error is reported, the
   functions after it no longer reach final and their operands go unchecked.  */
/* { dg-do compile } */
/* { dg-require-effective-target fpic } */
/* { dg-options "-O2 -fPIC" } */

void
f1 (void)
{
  __asm__ volatile (".long %p0" : : "p" (__func__)); /* { dg-error "invalid expression as operand" "" { target ia32 } } */
}
