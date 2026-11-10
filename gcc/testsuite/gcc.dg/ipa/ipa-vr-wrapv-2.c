/* { dg-do run } */
/* { dg-options "-O2" } */

/* Only caller asks for wrapping pointer overflow.  Every call of caller
   passes a non-null pointer, so IPA-CP must still let cmd + 8 wrap, or it
   decides that callee's argument is never null and deletes callee's null
   check.  */

int used;
char buf[16];

__attribute__ ((noinline)) void
use (const char *s)
{
  used = (s != 0) + 1;
}

static __attribute__ ((noinline)) int
callee (const char *s)
{
  if (!s)
    return -1;
  use (s);
  return 0;
}

static __attribute__ ((noinline, optimize ("wrapv-pointer"))) int
caller (const char *cmd)
{
  return callee (cmd + 8);
}

/* Keep a call with a valid pointer, so caller's argument is known to be
   non-null without being a known constant.  */
__attribute__ ((noipa)) int
other (void)
{
  return caller (buf);
}

__attribute__ ((noipa)) int
wraps (void)
{
  /* cmd + 8 wraps to a null pointer.  */
  return caller ((const char *) -8);
}

int
main (void)
{
  if (wraps () != -1 || used)
    __builtin_abort ();
  if (other () != 0 || used != 2)
    __builtin_abort ();
  return 0;
}
