/* PR target/122453 */
/* { dg-do compile } */
/* { dg-require-effective-target fpic } */
/* { dg-require-effective-target tls_native } */
/* { dg-options "-O2 -fPIC" } */

extern __thread char tb[64] __attribute__ ((tls_model ("initial-exec")));

void
f2 (unsigned long *p)
{
  __asm__ volatile ("mov %0, %1" : : "p" (&tb[8]), "m" (*p)); /* { dg-error "invalid expression as operand" } */
}
