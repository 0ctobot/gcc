/* PR target/119958 */
/* { dg-do assemble { target { *-*-linux* && lp64 } } } */
/* { dg-options "-O2 -fno-pic -mcmodel=kernel" } */

/* These 128-bit adds of a TP offset read from the GOT used to match patterns
   whose splits add the GOT slot with its flags exposed, which then failed to
   match in extract_insn.  */

typedef unsigned __int128 u128;
typedef unsigned long ul;

extern __thread char tb[64] __attribute__ ((tls_model ("initial-exec")));

#define OFF ((ul) &tb[0] - (ul) __builtin_thread_pointer ())

u128
f1 (u128 x)
{
  return x + OFF;
}

u128
f2 (ul lo, ul hi)
{
  return (((u128) hi << 64) | lo) + OFF;
}

u128
f3 (u128 *p, ul hi)
{
  return (((u128) hi << 64) | OFF) + *p;
}

u128
f4 (u128 *p, ul lo)
{
  return (((u128) OFF << 64) | lo) + *p;
}
