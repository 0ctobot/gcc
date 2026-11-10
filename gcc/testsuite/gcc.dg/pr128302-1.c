/* PR middle-end/128302 */
/* With -fzero-init-padding-bits=all, every bit of an initialized union
   that the initialized member doesn't cover must be zero, also when that
   member is a bit-field whose declared type is as wide as the union.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2 -fzero-init-padding-bits=all" } */

typedef __SIZE_TYPE__ size_t;
extern int memcmp (const void *, const void *, size_t);
extern void *memset (void *, int, size_t);

union u2 { unsigned int raw; int f : 24; };
union u7 { unsigned long long raw; long long f : 40; };
union u3 { unsigned short m9 : 12; unsigned short m10; };
union u4 { unsigned int f : 24; unsigned int raw; };
union u5 { unsigned int raw; unsigned short a : 9; };
struct w2 { union u2 u; };
typedef union {	/* port_id_t in the Linux qla2xxx driver.  */
  unsigned int b24 : 24;
  struct {
    unsigned char al_pa;
    unsigned char area;
    unsigned char domain;
    unsigned char rsvd_1;
  } b;
} port_id_t;

static int wrong;

/* Fill the stack below the caller with nonzero bytes.  */

__attribute__((noipa)) void
dirty (void)
{
  volatile unsigned char b[4096];
  for (int i = 0; i < 4096; i++)
    b[i] = 0xa5;
}

__attribute__((noipa)) void
check (const void *p, const void *want, size_t n)
{
  if (memcmp (p, want, n))
    wrong++;
}

/* A designated initializer of a 24-bit field in a 4-byte union.  */

__attribute__((noipa)) void
t_u2 (void)
{
  union u2 d = { .f = 0x123456 };
  union u2 w;
  memset (&w, 0, sizeof w);
  w.f = 0x123456;
  check (&d, &w, sizeof d);
}

/* A designated initializer of a 40-bit field in an 8-byte union.  */

__attribute__((noipa)) void
t_u7 (long long v)
{
  union u7 d = { .f = v };
  union u7 w;
  memset (&w, 0, sizeof w);
  w.f = v;
  check (&d, &w, sizeof d);
}

/* {0} when the first member is a 12-bit field in a 2-byte union.  */

__attribute__((noipa)) void
t_u3 (void)
{
  union u3 d = { 0 };
  union u3 w;
  memset (&w, 0, sizeof w);
  check (&d, &w, sizeof d);
}

/* {0} when the first member is a 24-bit field in a 4-byte union.  */

__attribute__((noipa)) void
t_u4 (void)
{
  union u4 d = { 0 };
  union u4 w;
  memset (&w, 0, sizeof w);
  check (&d, &w, sizeof d);
}

__attribute__((noipa)) void
t_portid (void)
{
  port_id_t d = { 0 };
  port_id_t w;
  memset (&w, 0, sizeof w);
  check (&d, &w, sizeof d);
}

/* The union nested in a struct.  */

__attribute__((noipa)) void
t_w2 (void)
{
  struct w2 d = { .u.f = 0x123456 };
  struct w2 w;
  memset (&w, 0, sizeof w);
  w.u.f = 0x123456;
  check (&d, &w, sizeof d);
}

/* Controls: = {} and a bit-field whose declared type is narrower than the
   union.  */

__attribute__((noipa)) void
t_ctl (void)
{
  union u4 d = {};
  union u4 w;
  memset (&w, 0, sizeof w);
  check (&d, &w, sizeof d);
}

__attribute__((noipa)) void
t_ctl_u5 (void)
{
  union u5 d = { .a = 0x1ff };
  union u5 w;
  memset (&w, 0, sizeof w);
  w.a = 0x1ff;
  check (&d, &w, sizeof d);
}

int
main (void)
{
  dirty (); t_u2 ();
  dirty (); t_u7 (-1);
  dirty (); t_u3 ();
  dirty (); t_u4 ();
  dirty (); t_portid ();
  dirty (); t_w2 ();
  dirty (); t_ctl ();
  dirty (); t_ctl_u5 ();
  if (wrong)
    __builtin_abort ();
  return 0;
}
