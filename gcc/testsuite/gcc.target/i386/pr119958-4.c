/* PR target/119958 */
/* { dg-do assemble { target { *-*-linux* && ia32 } } } */
/* { dg-options "-O2 -fno-pic" } */

/* These 64-bit adds of a TP offset read from the GOT used to match patterns
   whose splits add the GOT slot with its flags exposed, which then failed to
   match in extract_insn.  */

typedef unsigned long long u64;

extern __thread char tb[64] __attribute__ ((tls_model ("initial-exec")));

#define OFF ((unsigned) &tb[0] - (unsigned) __builtin_thread_pointer ())

__attribute__ ((regparm (3))) u64
f1 (unsigned lo, unsigned hi)
{
  return (((u64) hi << 32) | lo) + OFF;
}

__attribute__ ((regparm (3))) u64
f2 (u64 x, unsigned hi)
{
  return (((u64) hi << 32) | OFF) + x;
}
