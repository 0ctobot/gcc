/* An offset into a member steps back if any value it may take does, so each
   argument of a PHI must be followed, as must the dividend of a division, a
   shift or a remainder by a variable amount and the result of a subtraction
   that's checked for overflow.  An index that counts up on a cycle, a
   rotated index and an unsigned value divided, shifted or reduced by a
   constant only go forward, so they must keep the member's size, unless the
   value is wider than the offset and gets narrowed.  A constant step on a
   cycle that narrows the index goes by the bits the narrowing keeps, so
   adding 65535 to a short counts down.  */
/* { dg-do run { target { lp64 && int128 } } } */
/* { dg-options "-O2" } */

typedef __SIZE_TYPE__ size_t;
typedef unsigned long ul;

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];
long sink;

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

BACK (phi_sub, (ul a, ul b, int k),
      ul off = 0; if (k) off = a - b; char *p = L + off)
BACK (phi_const, (int k),
      long off; if (k) off = -3; else off = 4; char *p = L + off)
BACK (phi_neg, (long n, int k),
      long off; if (k) off = -n; else off = n; char *p = L + off)
BACK (phi_mul, (ul a, ul b, int k),
      ul off = 0; if (k) off = a - b; char *p = L + off * 2)
BACK (phi_or, (ul a, ul b, int k),
      ul off = 0; if (k) off = a - b; char *p = L + (off | 1))
BACK (loop_phi, (ul a, ul b, int n, int k),
      char *p = L; ul d = 0; if (k) d = a - b;
      for (int i = 0; i < n; i++) p += d)
BACK (count_down, (int n),
      char *p = L; ul d = 5; for (int i = 0; i < n; i++) { p += d; d--; })
BACK (udiv_var, (ul a, ul b, ul k), char *p = L + (a - b) / k)
BACK (ushr_var, (ul a, ul b, int s), char *p = L + ((a - b) >> s))
BACK (umod_var, (ul a, ul b, ul m), char *p = L + (a - b) % m)
BACK (rot_var, (ul a, ul b, int s),
      char *p = L + __builtin_stdc_rotate_left (a - b, s))
BACK (sub_ovf, (ul a, ul b),
      ul r; __builtin_sub_overflow (a, b, &r); char *p = L + r)
BACK (sub_ovf_or, (ul a, ul b),
      ul r; __builtin_sub_overflow (a, b, &r); char *p = L + (r | 1))
BACK (shr_wide, (ul a, ul b),
      unsigned __int128 x = (unsigned __int128) a - b;
      char *p = L + (ul) (x >> 1))
BACK (count_narrow, (int n),
      long l = 0;
      for (int i = 0; i < n; i++) { long t = l + 65535; sink = t; l = (short) t; }
      char *p = L + l)

FORWARD (phi_add, (ul a, ul b, int k),
	 ul off = a; if (k) off = a + b; char *p = L + off)
FORWARD (phi_pos, (int k),
	 long off; if (k) off = 3; else off = 7; char *p = L + off)
FORWARD (count_up, (int n),
	 char *p = L; ul d = 0; for (int i = 0; i < n; i++) { p += d; d++; })
FORWARD (count_up_narrow, (int n),
	 long l = 0;
	 for (int i = 0; i < n; i++) { long t = l + 1; sink = t; l = (short) t; }
	 char *p = L + l)
FORWARD (ushr_const, (ul a, ul b), char *p = L + ((a - b) >> 1))
FORWARD (udiv_const, (ul a, ul b), char *p = L + (a - b) / 3)
FORWARD (umod_const, (ul a, ul b), char *p = L + (a - b) % 24)
FORWARD (rot_fwd, (ul n, int s),
	 char *p = L + __builtin_stdc_rotate_left (n, s))
FORWARD (add_ovf, (ul a, ul b),
	 ul r; __builtin_add_overflow (a, b, &r); char *p = L + r)
FORWARD (mul_ovf, (ul a, ul b),
	 ul r; __builtin_mul_overflow (a, b, &r); char *p = L + r)

int
main (void)
{
  phi_sub (0, 3, 1);
  phi_const (1);
  phi_neg (3, 1);
  phi_mul (0, 3, 1);
  phi_or (0, 4, 1);
  loop_phi (0, 1, 3, 1);
  count_down (12);
  udiv_var (0, 3, 1);
  ushr_var (0, 3, 0);
  umod_var (0, 3, -1);
  rot_var (0, 3, 64);
  sub_ovf (0, 3);
  sub_ovf_or (0, 4);
  shr_wide (0, 3);
  count_narrow (1);
  phi_add (3, 4, 1);
  phi_pos (1);
  count_up (5);
  count_up_narrow (5);
  ushr_const (9, 3);
  udiv_const (9, 3);
  umod_const (9, 3);
  rot_fwd (3, 0);
  add_ovf (3, 4);
  mul_ovf (3, 4);
  return 0;
}
