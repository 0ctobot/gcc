/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* The table entry for a zero input fits the table's unsigned type, but it
   doesn't survive being subtracted from 31 in int.  */

static const unsigned tab[64] = {
  0x80000000U, 29, 0, 6, 0, 12, 0, 0, 2, 0, 5, 0, 1, 0, 0, 0,
  0, 31, 0, 0, 25, 0, 16, 26, 0, 22, 0, 17, 8, 27, 0, 0,
  0, 23, 14, 20, 0, 18, 0, 9, 0, 28, 0, 11, 0, 4, 0, 0,
  0, 30, 24, 15, 0, 21, 7, 0, 0, 13, 19, 0, 0, 0, 10, 3 };

static inline unsigned
g (unsigned x)
{
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  return tab[(x * 0xbb968a43U) >> 26];
}

__attribute__((noipa)) int t1 (unsigned x) { return g (x) == 0x80000000U; }
__attribute__((noipa)) int t2 (unsigned x) { return g (x) > 31; }
__attribute__((noipa)) int t3 (unsigned x) { return (g (x) >> 31) & 1; }
__attribute__((noipa)) int t4 (unsigned x) { return (int) g (x) < 0; }

int
main (void)
{
  if (t1 (0) != 1 || t2 (0) != 1 || t3 (0) != 1 || t4 (0) != 1)
    __builtin_abort ();
  if (t1 (5) != 0 || t2 (5) != 0 || t3 (5) != 0 || t4 (5) != 0)
    __builtin_abort ();
  return 0;
}
