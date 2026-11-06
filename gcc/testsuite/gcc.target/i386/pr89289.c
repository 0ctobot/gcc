/* PR target/89289 */
/* { dg-do compile { target { fpic && *-*-linux* } } } */
/* { dg-options "-O2 -fpic -mforce-indirect-call" } */

extern __thread int a;
static __thread int b;

int
f (void)
{
  return a;
}

int
g (void)
{
  return ++b;
}

/* Linker TLS relaxation only recognizes the direct call.  */
/* { dg-final { scan-assembler-times "call\[ \t\]+_*tls_get_addr@PLT" 2 } } */
/* { dg-final { scan-assembler-not "call\[ \t\]+\\*" } } */
