/* A bitwise AND can't give more than an operand that isn't negative, so it
   only goes forward when one of its operands can't be negative.  Masking an
   unsigned index down to a power-of-two size, which is how a ring buffer is
   indexed, and aligning a length up to a multiple must keep the member's
   size.  An AND of two values that may wrap around or be negative must still
   allow for the pointer to go below it, even when one of them is a signed
   index that could be used on its own.  */
/* { dg-do run { target lp64 } } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;
typedef unsigned long ul;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];

#define L tbl[1].label
#define NOIPA __attribute__ ((noipa)) void
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

FORWARD (mask, (ul n, ul m), char *p = L + (n & (m - 1)))
FORWARD (mask_plus, (ul n, ul m), char *p = L + (n & (m - 1)) + 1)
FORWARD (mask_uint, (unsigned n, unsigned m), char *p = L + (n & (m - 1)))
FORWARD (ring, (ul h, ul sz), ul i = h & (sz - 1); char *p = L + i)
FORWARD (align_up, (ul n, ul m), char *p = L + ((n + m - 1) & -m))
FORWARD (align_up_not, (ul n, ul m), char *p = L + ((n + (m - 1)) & ~(m - 1)))
FORWARD (mask_diff, (ul a, ul b, ul n), char *p = L + ((a - b) & n))
FORWARD (loop_mask, (int n, ul len, ul m),
	 char *p = L; for (int i = 0; i < n; i++) p += len & (m - 1))

BACK (diff_diff, (ul a, ul b, ul c, ul d), char *p = L + ((a - b) & (c - d)))
BACK (diff_not, (ul a, ul b, ul m), char *p = L + ((a - b) & ~m))
BACK (diff_neg, (ul a, ul b, ul m), char *p = L + ((a - b) & -m))
BACK (align_diff, (ul a, ul b, ul m), char *p = L + ((a - b + m - 1) & -m))
BACK (align_diff_not, (ul a, ul b, ul m),
      char *p = L + ((a - b + (m - 1)) & ~(m - 1)))
BACK (mask_int, (int n, int m), char *p = L + (n & (m - 1)))
BACK (and_sum, (int n, int k), char *p = L + ((n + 5) & k))
BACK (and_sum_swap, (int n, int k), char *p = L + (k & (n + 5)))
BACK (and_sum_long, (long n, long k), char *p = L + ((n + 5) & k))

int
main (void)
{
  mask (3, 8);
  mask_plus (3, 8);
  mask_uint (3, 8);
  ring (11, 8);
  align_up (3, 4);
  align_up_not (3, 4);
  mask_diff (1, 2, 7);
  loop_mask (2, 3, 4);
  diff_diff (1, 9, 1, 5);
  diff_not (1, 9, 3);
  diff_neg (1, 9, 4);
  align_diff (1, 17, 8);
  align_diff_not (1, 17, 8);
  mask_int (-1, -4);
  and_sum (-10, -1);
  and_sum_swap (-10, -1);
  and_sum_long (-10, -1);
  return 0;
}
