/* Check that __builtin_return_address (0) returns the exact, unsigned
   return address in frames that return_addr_signed_run_1.c doesn't cover:
   separately saved callee-saved registers, the EH data registers, outgoing
   stack arguments, over-aligned locals, large frames with and without a
   frame pointer, loops, sibling calls and the paths out of setjmp,
   __builtin_setjmp and a nonlocal goto.  return_addr_signed_run_3.c to
   return_addr_signed_run_6.c run the same checks with other options.
   Define SCS to skip the cases that use X18 or leave through longjmp.  */
/* { dg-do run } */
/* { dg-options "-O2 -march=armv8.3-a -mbranch-protection=pac-ret -save-temps" } */

#include <setjmp.h>
#include <sys/auxv.h>

#ifndef HWCAP_PACA
#define HWCAP_PACA (1UL << 30)
#endif

#define NOIPA __attribute__ ((noipa, used))
#define RA __builtin_return_address (0)

extern void abort (void);
extern void exit (int);

void *volatile sink;
volatile long sinkl;

NOIPA void
take (void *p)
{
  sink = p;
}

NOIPA void *
ident (void *p)
{
  return p;
}

NOIPA void
take10 (long a, long b, long c, long d, long e, long f, long g, long h,
	long i, long j)
{
  sinkl = a + b + c + d + e + f + g + h + i + j;
}

/* Separately shrink-wrapped callee-saved registers on the cold path, and a
   second read on a path without a call.  */
NOIPA void *
sep_cs (long c)
{
  if (__builtin_expect (c > 1, 0))
    {
      long a = sinkl * 3, b = sinkl * 5, d = sinkl * 7, e = sinkl * 11;
      take (0);
      sinkl = a + b + d + e;
      take (RA);
      sinkl += a * b * d * e;
      return sink;
    }
  if (c == 1)
    return RA;
  return 0;
}

/* Ten callee-saved registers live across a call, read on the cold path.  */
NOIPA void *
many_cs (long c)
{
  long a0 = sinkl, a1 = sinkl * 2, a2 = sinkl * 3, a3 = sinkl * 4;
  long a4 = sinkl * 5, a5 = sinkl * 6, a6 = sinkl * 7, a7 = sinkl * 8;
  long a8 = sinkl * 9, a9 = sinkl * 10;
  take (0);
  sinkl = a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9;
  if (__builtin_expect (c != 0, 0))
    take (RA);
  sinkl = a0 * a1 * a2 * a3 * a4 * a5 * a6 * a7 * a8 * a9;
  return sink;
}

/* __builtin_unwind_init and an untaken __builtin_eh_return, which add the
   EH data registers to the save area.  The epilogue reloads X0-X3 on the
   normal return path too, which clobbers the return value, so the result
   is passed back through eh_ra.  */
void *eh_ra;
NOIPA void *
eh_ret (long c)
{
  __builtin_unwind_init ();
  void *ra1 = RA;
  take (0);
  void *ra2 = RA;
  if (ra1 != ra2)
    abort ();
  if (__builtin_expect (c == 12345, 0))
    __builtin_eh_return (c, ra1);
  eh_ra = ra2;
  return ra2;
}

/* A variable-length array, which needs a frame pointer, and outgoing stack
   arguments.  */
NOIPA void *
vla_args (long c)
{
  char buf[c + 8];
  buf[c] = 1;
  take (buf);
  take10 (c, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  return RA;
}

/* An over-aligned local.  */
NOIPA void *
overaligned (long c)
{
  char buf[64] __attribute__ ((aligned (256)));
  buf[c & 63] = 1;
  take (buf);
  if (__builtin_expect (c != 0, 1))
    return RA;
  return 0;
}

/* Outgoing stack arguments, read on the cold path.  */
NOIPA void *
args_cold (long c)
{
  take10 (c, 1, 2, 3, 4, 5, 6, 7, 8, 9);
  if (__builtin_expect (c != 0, 0))
    take (RA);
  return sink;
}

/* A large frame with a frame pointer and an alloca.  */
NOIPA void *
big_fp (long c)
{
  char buf[70000];
  char *p = __builtin_alloca (c + 16);
  buf[c] = 1;
  p[0] = 1;
  take (buf);
  take (p);
  if (__builtin_expect (c != 0, 0))
    take (RA);
  return sink;
}

/* A large frame that needs no frame pointer.  */
NOIPA void *
big_sp (long c)
{
  char buf[100000];
  long a = sinkl, b = sinkl * 3;
  buf[c] = 1;
  take (buf);
  sinkl = a + b;
  if (__builtin_expect (c != 0, 0))
    take (RA);
  return sink;
}

/* A read in a loop.  */
NOIPA void *
loop (long c)
{
  void *r = 0;
  for (long i = 0; i < c; i++)
    {
      take (RA);
      r = sink;
    }
  return r;
}

/* __builtin_frame_address (0) in the same function.  */
NOIPA void *
fa (long c)
{
  take (__builtin_frame_address (0));
  return RA;
}

/* A read followed by a sibling call.  */
NOIPA void *
sib (long c)
{
  take (RA);
  return ident (sink);
}

/* The ptrauth_clear_pac () wrapper of older arm64 kernels, which names its
   argument three times.  A user address has bit 55 and bits 39-54 clear.  */
#define PAC_MASK_K (~0UL << 39)
#define PAC_MASK_U ((~0UL << 39) & (~0UL >> 9))
#define CLEAR_PAC(p) (((p) & (1UL << 55)) ? ((p) | PAC_MASK_K)	\
		      : ((p) & ~PAC_MASK_U))
NOIPA void *
wrapped (long c)
{
  take (0);
  if (__builtin_expect (c != 0, 0))
    take ((void *) CLEAR_PAC ((unsigned long) RA));
  return sink;
}

#ifndef SCS
/* setjmp and longjmp, with reads before and after the second return.  */
static jmp_buf jb;

NOIPA void
jump (void)
{
  longjmp (jb, 1);
}

NOIPA void *
sj (long c)
{
  void *before = RA;
  if (setjmp (jb) == 0)
    {
      take (before);
      jump ();
    }
  void *after = RA;
  if (after != before)
    abort ();
  return after;
}

/* __builtin_setjmp and __builtin_longjmp.  */
static void *bjb[5];

NOIPA void
bjump (void)
{
  __builtin_longjmp (bjb, 1);
}

NOIPA void *
bsj (long c)
{
  if (__builtin_setjmp (bjb) == 0)
    {
      take (0);
      bjump ();
    }
  return RA;
}

/* A nonlocal goto out of a nested function, which passes its static chain
   in X18.  */
NOIPA void *
nlg (long c)
{
  __label__ out;
  __attribute__ ((noinline)) void inner (void) { take (0); goto out; }
  inner ();
  return 0;
 out:
  return RA;
}

/* A noreturn function that reads its return address and leaves through
   longjmp.  */
static jmp_buf njb;
void *nret_ra, *nret_exp;

NOIPA __attribute__ ((noreturn)) void
nret (long c)
{
  nret_ra = RA;
  take (nret_ra);
  longjmp (njb, 1);
}
#endif

#ifdef __ARM_NEON
#define FP_CLOBBERS							\
  , "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10",	\
  "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20",	\
  "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30",	\
  "v31"
#else
#define FP_CLOBBERS
#endif

#ifdef SCS
#define X18_CLOBBER
#else
#define X18_CLOBBER , "x18"
#endif

/* Call FN with ARG and check that RES is the address right after the
   call, or EXP computed from that address in ra_.  */
#define CHECK(FN, ARG) CHECK_RES (FN, ARG, (void *) x0_)
#define CHECK_RES(FN, ARG, RES) CHECK_EXP (FN, ARG, RES, ra_)
#define CHECK_EXP(FN, ARG, RES, EXP)					\
  do									\
    {									\
      register long x0_ __asm__ ("x0") = (ARG);				\
      void *ra_;							\
      __asm__ volatile ("bl\t" #FN "\n\tadr\t%1, ."			\
			: "+r" (x0_), "=r" (ra_)			\
			:						\
			: "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", \
			  "x9", "x10", "x11", "x12", "x13", "x14", "x15", \
			  "x16", "x17", "x30", "cc", "memory"		\
			  X18_CLOBBER FP_CLOBBERS);			\
      if ((RES) != (EXP))						\
	abort ();							\
    }									\
  while (0)

#ifdef SCS
static unsigned long scs_buf[4096];
__attribute__ ((no_sanitize ("shadow-call-stack")))
#endif
int
main (void)
{
  /* Unlike PACIASP and AUTIASP, XPACI and RETAA aren't in the hint space,
     so they fault on a CPU without pointer authentication.  Leave through
     exit, since returning from main could use RETAA.  */
  if (!(getauxval (AT_HWCAP) & HWCAP_PACA))
    exit (0);
#ifdef SCS
  __asm__ volatile ("mov\tx18, %0" : : "r" (scs_buf) : "memory");
#endif
  for (int i = 0; i < 2; i++)
    {
      CHECK (sep_cs, 1);
      CHECK (sep_cs, 2);
      CHECK (many_cs, 1);
      CHECK_RES (eh_ret, 1, eh_ra);
      CHECK (vla_args, 40);
      CHECK (overaligned, 3);
      CHECK (args_cold, 1);
      CHECK (big_fp, 5);
      CHECK (big_sp, 5);
      CHECK (loop, 3);
      CHECK (fa, 0);
      CHECK (sib, 0);
      /* The wrapper alters a high user address, as in a PIE.  */
      CHECK_EXP (wrapped, 1, (void *) x0_,
		 (void *) CLEAR_PAC ((unsigned long) ra_));
#ifndef SCS
      CHECK (sj, 0);
      CHECK (bsj, 0);
      CHECK (nlg, 0);
      if (setjmp (njb) == 0)
	__asm__ volatile ("adr\tx9, 1f\n\tstr\tx9, [%0]\n\tmov\tx0, 1\n\t"
			  "bl\tnret\n1:"
			  : : "r" (&nret_exp) : "x0", "x9", "x30", "memory");
      if (nret_ra != nret_exp)
	abort ();
#endif
    }
  return 0;
}

/* { dg-final { scan-assembler {\txpaci\t} } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
