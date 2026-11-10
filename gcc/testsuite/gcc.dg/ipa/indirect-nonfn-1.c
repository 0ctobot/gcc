/* { dg-do run { target { aarch64*-*-linux* && aarch64_little_endian } } } */
/* { dg-options "-O2" } */

/* Once IPA propagates the callers' arguments into run_cb_if and run_cb,
   the indirect call's target is the integer constant FIXED.  That's a
   valid call target, and the call must not be treated as unreachable.  */

#include <string.h>
#include <sys/mman.h>

typedef long (*fn_t) (long);

#define FIXED 0x40000000UL

volatile int zero = 0, one = 1;
long acc[8];

/* Too large for the early inliner, so the indirect call is still there
   when the IPA passes learn its target.  */
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

static long
run_cb (fn_t cb, long x)
{
  long s = 0;
  for (int i = 0; i < 8; i++)
    {
      acc[i] += x * (i + 1);
      s += acc[i] ^ (s << 1);
    }
  for (int i = 0; i < 8; i++)
    acc[i] ^= s >> i;
  return cb (x) + 100 + (s & zero);
}

__attribute__ ((noinline)) long
conditional (void)
{
  return run_cb_if ((fn_t) FIXED, 30, one);
}

__attribute__ ((noinline)) long
unconditional (void)
{
  return run_cb ((fn_t) FIXED, 20);
}

int
main (void)
{
  /* add x0, x0, #1; ret */
  static const unsigned int code[] = { 0x91000400, 0xd65f03c0 };
  void *p = mmap ((void *) FIXED, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
		  MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);

  if (p != (void *) FIXED)
    return 0;
  memcpy (p, code, sizeof code);
  __builtin___clear_cache ((char *) p, (char *) p + sizeof code);

  if (conditional () != 131)
    __builtin_abort ();
  if (unconditional () != 121)
    __builtin_abort ();
  return 0;
}
