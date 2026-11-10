/* Check byte loops vectorized with vectors held in X registers when the
   source and destination are misaligned, at every pair of offsets from 0
   to 7 and for lengths that cover the vector loop and its epilogues.  */
/* { dg-do run } */
/* { dg-options "-O3 -mgeneral-regs-only -fdump-tree-vect-details" } */

typedef unsigned char u8;

#define MAXLEN 70
#define BUFSZ (MAXLEN + 16)

__attribute__ ((noipa)) void
cpy8a (u8 *d, const u8 *s, long n)
{
  for (long i = 0; i < n; i++)
    d[i] = s[i];
}

__attribute__ ((noipa)) void
xor8 (u8 *restrict d, const u8 *restrict s, long n)
{
  for (long i = 0; i < n; i++)
    d[i] ^= s[i];
}

static u8 dst[BUFSZ] __attribute__ ((aligned (16)));
static u8 src[BUFSZ] __attribute__ ((aligned (16)));

static u8
dval (int i)
{
  return i * 7 + 1;
}

static u8
sval (int i)
{
  return i * 13 + 5;
}

static void
init (void)
{
  for (int i = 0; i < BUFSZ; i++)
    {
      dst[i] = dval (i);
      src[i] = sval (i);
    }
}

int
main (void)
{
  for (int doff = 0; doff < 8; doff++)
    for (int soff = 0; soff < 8; soff++)
      for (int n = 0; n <= MAXLEN; n++)
	{
	  init ();
	  cpy8a (dst + doff, src + soff, n);
	  for (int i = 0; i < BUFSZ; i++)
	    {
	      u8 want = (i >= doff && i < doff + n)
			? sval (i - doff + soff) : dval (i);
	      if (dst[i] != want)
		__builtin_abort ();
	    }

	  init ();
	  xor8 (dst + doff, src + soff, n);
	  for (int i = 0; i < BUFSZ; i++)
	    {
	      u8 want = (i >= doff && i < doff + n)
			? dval (i) ^ sval (i - doff + soff) : dval (i);
	      if (dst[i] != want)
		__builtin_abort ();
	    }
	}
  return 0;
}

/* { dg-final { scan-tree-dump-not "versioned for vectorization to enhance alignment" "vect" } } */
