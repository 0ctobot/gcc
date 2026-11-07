/* An offset that wraps around to something negative takes a pointer below the
   member no matter what's done to it afterward: shifting, masking with high
   bits set, taking the larger of it and something else, scaling by a variable
   and narrowing to a signed type all leave it negative.  The same goes for a
   signed index under a step that repeats on a cycle.  On the other hand, a
   value that's zero extended or masked down to a small one can't be negative
   whatever it was before, and must keep the member's size.  */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;
typedef unsigned long ul;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];

#define L tbl[1].label
#define NOIPA __attribute__ ((noipa)) void
#define barrier() __asm__ ("" : : : "memory")
#define bos(p) __builtin_object_size (p, 1)
#define bdos(p) __builtin_dynamic_object_size (p, 1)

/* P must be below the member, and the size can't be less than what's left of
   the member from there.  */
#define check_back(p) \
  do \
    { \
      size_t want = L + sizeof (L) - (p); \
      if (want <= sizeof (L) || bos (p) < want || bdos (p) < want) \
	__builtin_abort (); \
    } \
  while (0)

/* P only goes forward, so the size must remain that of the member.  */
#define check_member(p) \
  do \
    if (bos (p) > sizeof (L) || bdos (p) > sizeof (L)) \
      __builtin_abort (); \
  while (0)

#define BACK(name, args, ...) \
  NOIPA name args { __VA_ARGS__; check_back (p); }
#define FORWARD(name, args, ...) \
  NOIPA name args { __VA_ARGS__; check_member (p); }

BACK (shl_index, (int n), char *p = L + ((size_t) n << 2) + 8)
BACK (or_plus, (ul a, ul b), char *p = L + ((a - b) | 1) + 1)
BACK (xor_plus, (ul a, ul b), char *p = L + ((a - b) ^ 1) + 1)
BACK (shl_plus, (ul a, ul b), char *p = L + ((a - b) << 1) + 2)
BACK (and_plus, (ul a, ul b), char *p = L + ((a - b) & ~1UL) + 2)
BACK (max_plus, (ul a, ul b, ul c),
      ul x = a - b; ul o = x > c ? x : c; char *p = L + o + 8)
BACK (or_alone, (ul a, ul b), char *p = L + ((a - b) | 1))
BACK (xor_alone, (ul a, ul b), char *p = L + ((a - b) ^ 1))
BACK (shl_alone, (ul a, ul b), char *p = L + ((a - b) << 1))
BACK (and_alone, (ul a, ul b), char *p = L + ((a - b) & ~1UL))
BACK (max_alone, (ul a, ul b, ul c),
      ul x = a - b; ul o = x > c ? x : c; char *p = L + o)
BACK (not_alone, (ul u), char *p = L + ~u)
BACK (sdiv_alone, (long a, long b), char *p = L + (a - b) / 2)
BACK (sshr_alone, (long a, long b), char *p = L + ((a - b) >> 1))
BACK (smod_alone, (long a, long b), char *p = L + (a - b) % 8)

BACK (index_then_walk, (int n, int k, size_t len),
      char *p = L + n; barrier (); while (k--) { p += len; barrier (); })
BACK (sub_in_walk, (ul a, ul b, size_t len, int k),
      char *p = L;
      while (k--) { p += a - b; barrier (); p += len; barrier (); })

BACK (ptrdiff_char, (char *q, char *r), char *p = L + (q - r))
BACK (ptrdiff_scaled, (char *q, char *r), char *p = L + (q - r) * 2)
BACK (ptrdiff_int, (int *q, int *r), char *p = L + (q - r))
BACK (ptrdiff_walk, (char *q, char *r, int k),
      char *p = L; while (k--) { p += q - r; barrier (); })

BACK (sub_times_var, (ul a, ul b, ul k), char *p = L + (a - b) * k)
BACK (var_times_sub, (ul a, ul b, ul k), char *p = L + k * (a - b))

BACK (narrow_div, (int x), char *p = L + (short) ((x & 0xfffff) / 3) + 7)
BACK (narrow_mod, (int x), char *p = L + (short) ((x & 0xfffff) % 70000) + 7)
BACK (narrow_char, (int x), char *p = L + (signed char) ((x & 0xffff) / 2) + 7)
BACK (narrow_int, (long x), char *p = L + (int) ((x & 0xffffffffffL) / 3) + 5)

FORWARD (zext_u32, (int x), char *p = L + (unsigned) x + 1)
FORWARD (zext_u16, (short s), char *p = L + (unsigned short) s + 1)
FORWARD (zext_sub_plus, (unsigned a, unsigned b), char *p = L + (a - b) + 1)
FORWARD (zext_sub_alone, (unsigned a, unsigned b), char *p = L + (a - b))
FORWARD (mask, (int x), char *p = L + (x & 0xff))
FORWARD (mask_plus, (int x), char *p = L + (x & 0xff) + 1)
FORWARD (mask_long, (long x), char *p = L + (x & 0xff) + 1)
FORWARD (mask_narrow, (long x), char *p = L + (int) (x & 0xff) + 1)
FORWARD (mask_unarrow, (ul x), char *p = L + (int) (x & 0xff) + 1)
FORWARD (mask_sub, (ul a, ul b), char *p = L + ((a - b) & 0xff) + 1)
FORWARD (scale, (ul n), char *p = L + 4 * n + 8)
FORWARD (shift, (ul n), char *p = L + (n << 2) + 8)
FORWARD (ior, (ul n), char *p = L + (n | 1) + 1)
FORWARD (xor, (ul n), char *p = L + (n ^ 1) + 1)
FORWARD (align, (ul n), char *p = L + (n & ~3UL) + 4)
FORWARD (align_var, (ul n, ul m), char *p = L + (n & ~m) + 4)
FORWARD (align_neg, (ul n, ul m), char *p = L + (n & -m) + 4)
FORWARD (umin_sub, (ul a, ul b),
	 ul x = a - b; ul o = x < 16 ? x : 16; char *p = L + o + 1)
FORWARD (umin, (ul n, ul m), ul o = n < m ? n : m; char *p = L + o + 1)
FORWARD (smax, (long i), long o = i > 0 ? i : 0; char *p = L + o + 1)
FORWARD (index_shl, (long i), char *p = L + (i << 2))
FORWARD (index_ior, (long i), char *p = L + (i | 1))
FORWARD (unsigned_then_walk, (ul u, ul len, int k),
	 char *p = L + u; barrier (); while (k--) { p += len; barrier (); })

int
main (void)
{
  char c[8];
  int ii[4];

  shl_index (-3);
  or_plus (0, 3);
  xor_plus (0, 3);
  shl_plus (0, 2);
  and_plus (0, 4);
  max_plus (0, 12, 0);
  or_alone (0, 3);
  xor_alone (0, 3);
  shl_alone (0, 2);
  and_alone (0, 4);
  max_alone (0, 4, 0);
  not_alone (2);
  sdiv_alone (0, 6);
  sshr_alone (0, 6);
  smod_alone (0, 6);

  index_then_walk (-5, 1, 3);
  sub_in_walk (0, 4, 1, 1);

  ptrdiff_char (c, c + 1);
  ptrdiff_scaled (c, c + 1);
  ptrdiff_int (ii, ii + 1);
  ptrdiff_walk (c, c + 1, 1);

  sub_times_var (0, 1, 3);
  var_times_sub (0, 1, 3);

  narrow_div (3 * 65525);
  narrow_mod (65525);
  narrow_char (2 * 245);
  narrow_int (3 * 0xFFFFFFF9L);

  zext_u32 (1);
  zext_u16 (1);
  zext_sub_plus (3, 1);
  zext_sub_alone (3, 1);
  mask (1);
  mask_plus (1);
  mask_long (1);
  mask_narrow (1);
  mask_unarrow (1);
  mask_sub (0, 3);
  scale (1);
  shift (1);
  ior (1);
  xor (1);
  align (1);
  align_var (1, 3);
  align_neg (1, 4);
  umin_sub (3, 1);
  umin (1, 2);
  smax (1);
  index_shl (1);
  index_ior (1);
  unsigned_then_walk (1, 1, 1);
  return 0;
}
