/* { dg-lto-do run } */
/* { dg-lto-options {{-O2 -flto -fno-strict-overflow}} } */
/* { dg-suppress-ld-options {-fno-strict-overflow} } */

/* Both files are compiled with -fno-strict-overflow, but the link command
   line doesn't repeat it.  Every call of caller passes a non-null pointer,
   so IPA-CP must still let cmd + 8 wrap, or it decides that callee's
   argument is never null and deletes callee's null check.  */

extern int callee (const char *);

int used;
char buf[16];

__attribute__ ((noinline)) void
use (const char *s)
{
  used = (s != 0) + 1;
}

static __attribute__ ((noinline)) int
caller (const char *cmd)
{
  return callee (cmd + 8);
}

/* Keep a call with a valid pointer, so caller's argument is known to be
   non-null without being a known constant.  */
__attribute__ ((externally_visible)) int
other (void)
{
  return caller (buf);
}

int
main (void)
{
  /* cmd + 8 wraps to a null pointer.  */
  if (caller ((const char *) -8) != -1 || used)
    __builtin_abort ();
  return 0;
}
