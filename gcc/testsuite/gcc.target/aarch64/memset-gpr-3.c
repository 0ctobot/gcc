/* Check the bytes written by memsets that are expanded inline with general
   registers, including the overlapping tail stores, at every destination
   offset from 0 to 8.  The fill value passed to the variable fills has
   bits set above the low byte, which memset must ignore.  */
/* { dg-do run } */
/* { dg-options "-O2 -mgeneral-regs-only -fno-tree-loop-distribute-patterns -save-temps" } */

#define BUFSZ 288
#define POISON 0x11

static unsigned char buf[BUFSZ] __attribute__ ((aligned (16)));

#define FILLS(n)							\
  __attribute__ ((noipa)) void z##n (unsigned char *p)		\
  { __builtin_memset (p, 0, n); }					\
  __attribute__ ((noipa)) void c##n (unsigned char *p)		\
  { __builtin_memset (p, 0xa5, n); }					\
  __attribute__ ((noipa)) void v##n (unsigned char *p, int c)		\
  { __builtin_memset (p, c, n); }

#define SIZES(X) X (7) X (8) X (9) X (15) X (16) X (17) X (23) X (24) \
		 X (25) X (255) X (256)

SIZES (FILLS)

static void
check (unsigned int off, unsigned int n, unsigned char fill)
{
  for (unsigned int i = 0; i < BUFSZ; i++)
    {
      unsigned char want = (i >= off && i < off + n) ? fill : POISON;
      if (buf[i] != want)
	__builtin_abort ();
    }
}

static void
poison (void)
{
  for (unsigned int i = 0; i < BUFSZ; i++)
    buf[i] = POISON;
}

#define RUN(n)						\
  poison (); z##n (buf + off); check (off, n, 0);	\
  poison (); c##n (buf + off); check (off, n, 0xa5);	\
  poison (); v##n (buf + off, 0x13c); check (off, n, 0x3c);

int
main (void)
{
  for (unsigned int off = 0; off <= 8; off++)
    {
      SIZES (RUN)
    }
  return 0;
}

/* { dg-final { scan-assembler-not {\tbl?\tmemset} } } */
