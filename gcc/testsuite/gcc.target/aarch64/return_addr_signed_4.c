/* With return address signing, each read of the return address should load
   it from its save slot and strip it where it's used, rather than keeping a
   stripped copy in a callee-saved register across calls.  The macro mimics
   the ptrauth_clear_pac () wrapper that older arm64 kernels put around
   __builtin_return_address (), which names its argument three times.  */
/* { dg-do compile } */
/* { dg-options "-O2 -march=armv8.3-a -mgeneral-regs-only -mbranch-protection=pac-ret -fno-omit-frame-pointer -fno-optimize-sibling-calls" } */
/* { dg-final { check-function-bodies "**" "" "" } } */

#define GENMASK_ULL(h, l)						\
  (((~0ULL) - (1ULL << (l)) + 1) & (~0ULL >> (63 - (h))))
#define ptrauth_clear_pac(ptr)						\
  (((ptr) & (1ULL << 55)) ? ((ptr) | GENMASK_ULL (63, 39))		\
   : ((ptr) & ~GENMASK_ULL (54, 39)))
#define RET_IP								\
  ptrauth_clear_pac ((unsigned long) __builtin_return_address (0))

extern long work (long);
extern void *alloc_track (unsigned long, unsigned int, unsigned long);
extern void release (void *);

/*
** two_paths:
**	...
**	bl	alloc_track
**	...
**	bl	release
**	...
**	ldr	(x[0-9]+), \[sp, 8\]
**	...
**	xpaci	\1
**	...
**	bl	alloc_track
**	...
*/
void *
two_paths (unsigned long n, long f)
{
  void *p = alloc_track (n, 0xcc0, RET_IP);
  if (!p)
    return 0;
  if (work (f))
    {
      release (p);
      p = alloc_track (n * 2, 0xcc0, RET_IP);
    }
  return p;
}

/* { dg-final { scan-assembler-not {\tx2[3-8]} } } */
/* { dg-final { scan-assembler-not {xpaclri} } } */
