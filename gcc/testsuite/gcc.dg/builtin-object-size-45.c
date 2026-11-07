/* { dg-do run } */
/* { dg-require-effective-target int128 } */
/* { dg-options "-O2" } */

/* An index wider than an offset is truncated to one, so even an unsigned one
   can take the address below the start of its array, and with it below the
   start of the subobject.  The maximum dynamic size must then be at least
   what's left, while an index whose range keeps it in the non-negative half
   of an offset still gets the member's exact size.  The minimum stays at most
   what's left.  */

typedef __SIZE_TYPE__ size_t;
typedef unsigned __int128 u128;
typedef __int128 s128;

#define NOIPA __attribute__ ((noipa))
#define BDOS __builtin_dynamic_object_size

struct e
{
  char pad[12];
  char nm[20];
};

struct s
{
  long h[2];
  struct e a[3];
};

struct s gs[2];
char buf[256];

NOIPA size_t gi (u128 x) { return BDOS (&gs[1].a[1].nm[x], 1); }
NOIPA size_t gi3 (u128 x) { return BDOS (&gs[1].a[1].nm[x], 3); }
NOIPA size_t go (u128 x) { return BDOS (&gs[1].a[x].nm[0], 1); }
NOIPA size_t gm (u128 x) { return BDOS (&gs[1].a[1].nm[x & 7], 1); }
NOIPA size_t gs1 (s128 x) { return BDOS (&gs[1].a[1].nm[x], 1); }
NOIPA size_t pi (struct s *p, u128 x) { return BDOS (&p->a[1].nm[x], 1); }
NOIPA size_t pi3 (struct s *p, u128 x) { return BDOS (&p->a[1].nm[x], 3); }
NOIPA size_t po (struct s *p, u128 x) { return BDOS (&p->a[x].nm[0], 1); }

/* Offsets from the start of a struct s.  */
#define NM1 (__builtin_offsetof (struct s, a[1].nm))
#define NM_1 (__builtin_offsetof (struct s, a) - sizeof (struct e)	\
	      + __builtin_offsetof (struct e, nm))

int
main (void)
{
  struct s *p = (struct s *) (buf + 32);
  /* -3 and -1 once truncated to an offset.  */
  u128 m3 = ((u128) 1 << (__SIZEOF_SIZE_T__ * 8 + 2)) - 3;
  u128 m1 = ((u128) 5 << (__SIZEOF_SIZE_T__ * 8)) - 1;
  size_t gleft = sizeof gs - sizeof gs[0] - NM1 + 3;
  size_t gleft_o = sizeof gs - sizeof gs[0] - NM_1;
  size_t pleft = sizeof buf - 32 - NM1 + 3;
  size_t pleft_o = sizeof buf - 32 - NM_1;

  if (gi (m3) < gleft || gi3 (m3) > gleft)
    __builtin_abort ();
  if (go (m1) < gleft_o)
    __builtin_abort ();
  if (gs1 (-3) < gleft)
    __builtin_abort ();
  if (pi (p, m3) < pleft || pi3 (p, m3) > pleft)
    __builtin_abort ();
  if (po (p, m1) < pleft_o)
    __builtin_abort ();
  /* Indexes that stay in the member.  */
  if (gi (2) != 18 || gi3 (2) != 18 || gm (m3) != 15 || pi (p, 2) != 18
      || pi3 (p, 2) != 18)
    __builtin_abort ();
  return 0;
}
