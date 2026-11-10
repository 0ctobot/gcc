/* { dg-do compile } */
/* { dg-options "-O2 -fno-delete-null-pointer-checks -fdump-ipa-inline-details" } */

/* With null pointer checks kept, a call through a null function pointer
   must still be emitted so that it faults, rather than being treated as
   unreachable along with the code that leads to it.  */

typedef long (*fn_t) (long);

volatile int zero, one;
long acc[8];

static long
run_cb_if (fn_t cb, long x, int doit)
{
  long s = 0, r = 5;
  for (int i = 0; i < 8; i++)
    {
      acc[i] += x * (i + 1);
      s += acc[i] ^ (s << 1);
    }
  for (int i = 0; i < 8; i++)
    acc[i] ^= s >> i;
  if (doit)
    r = cb (x) + 100;
  return r + (s & zero);
}

__attribute__ ((noinline)) long
null_call (void)
{
  return run_cb_if ((fn_t) 0, 30, one);
}

/* { dg-final { scan-ipa-dump "leaving it indirect" "inline" } } */
/* { dg-final { scan-ipa-dump-not "making it __builtin_unreachable" "inline" } } */
/* { dg-final { scan-assembler "blr\t" { target aarch64*-*-* } } } */
