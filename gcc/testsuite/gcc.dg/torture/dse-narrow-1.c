/* { dg-do run } */

/* RTL DSE narrows a two-word constant store to one word when a later
   store in the same block overwrites the other word.  Check that every
   byte still ends up with the right value.  */

struct s6 { unsigned long a, b, c, d, e, f; };

__attribute__((noipa)) void
check6 (struct s6 *p, unsigned long a, unsigned long b, unsigned long c,
	unsigned long d, unsigned long e, unsigned long f)
{
  if (p->a != a || p->b != b || p->c != c
      || p->d != d || p->e != e || p->f != f)
    __builtin_abort ();
}

/* The store to b overwrites the second word of the clear of a and b.  */

__attribute__((noipa)) void
tail_dead (unsigned long x)
{
  struct s6 s = { .b = x };
  check6 (&s, 0, x, 0, 0, 0, 0);
}

/* The store to c overwrites the first word of the clear of c and d.  */

__attribute__((noipa)) void
head_dead (unsigned long x)
{
  struct s6 s = { .c = x };
  check6 (&s, 0, 0, x, 0, 0, 0);
}

/* The same through a pointer.  */

__attribute__((noipa)) void
ptr_dead (struct s6 *p, unsigned long x, unsigned long y)
{
  *p = (struct s6) { .b = x, .e = y };
}

/* B is read between the clear and the store to b, so the clear has to
   store b as well.  */

__attribute__((noipa)) void
read_between (unsigned long x)
{
  struct s6 s = { .f = 1 };
  unsigned long old = *(volatile unsigned long *) &s.b;
  s.b = x;
  check6 (&s, 0, x, 0, 0, 0, 1);
  if (old != 0)
    __builtin_abort ();
}

#ifdef __SIZEOF_INT128__
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define LO 0
#else
#define LO 1
#endif
#define HI (1 - LO)
#define HIVAL 0x1122334455667788ULL
#define LOVAL 0x99aabbccddeeff00ULL

union u
{
  unsigned __int128 w;
  unsigned long long h[2];
};

__attribute__((noipa)) void
check_u (union u *p, unsigned long long h0, unsigned long long h1)
{
  if (p->h[0] != h0 || p->h[1] != h1)
    __builtin_abort ();
}

/* Overwrite the low half of a non-zero constant.  */

__attribute__((noipa)) void
lo_dead (unsigned long long x)
{
  union u u;
  u.w = ((unsigned __int128) HIVAL << 64) | LOVAL;
  u.h[LO] = x;
  if (LO == 0)
    check_u (&u, x, HIVAL);
  else
    check_u (&u, HIVAL, x);
}

/* Overwrite the high half of a non-zero constant.  */

__attribute__((noipa)) void
hi_dead (unsigned long long x)
{
  union u u;
  u.w = ((unsigned __int128) HIVAL << 64) | LOVAL;
  u.h[HI] = x;
  if (LO == 0)
    check_u (&u, LOVAL, x);
  else
    check_u (&u, x, LOVAL);
}
#endif

int
main ()
{
  struct s6 s;
  __builtin_memset (&s, 0x55, sizeof s);
  tail_dead (42);
  head_dead (43);
  ptr_dead (&s, 44, 45);
  check6 (&s, 0, 44, 0, 0, 45, 0);
  read_between (46);
#ifdef __SIZEOF_INT128__
  lo_dead (47);
  hi_dead (48);
#endif
  return 0;
}
