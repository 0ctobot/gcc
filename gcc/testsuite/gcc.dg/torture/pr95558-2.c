/* PR ipa/95558 */
/* { dg-do run } */
/* { dg-require-weak-override "" } */
/* { dg-require-alias "" } */
/* { dg-additional-sources "pr95558-2a.c" } */
/* { dg-additional-options "-fno-ipa-pure-const" } */

/* A call through a weak alias runs whichever definition the linker picks, which
   means the flags of the function that the alias names in this unit say nothing
   about the call.  The definition in pr95558-2a.c overrides the weak symbol
   here and writes to v.  Disabling ipa-pure-const leaves the use of those flags
   to ipa-reference.  */

extern void abort (void);

static int v;

void
set_v (void)
{
  v = 1;
}

static __attribute__ ((const)) int
const_body (void)
{
  return 0;
}
extern int weak_const (void) __attribute__ ((weak, alias ("const_body")));

/* The asm keeps this function from getting the flags itself.  */
static __attribute__ ((noinline)) void
caller (void)
{
  weak_const ();
  __asm__ ("" : : : "memory");
}

__attribute__ ((noinline)) int
test (void)
{
  v = 0;
  caller ();
  return v;
}

int
main (void)
{
  if (test () != 1)
    abort ();
  return 0;
}
