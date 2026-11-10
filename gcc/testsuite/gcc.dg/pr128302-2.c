/* PR middle-end/128302 */
/* More unions initialized through a bit-field narrower than the union:
   every bit outside the bit-field must be zero with
   -fzero-init-padding-bits=all.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2 -fzero-init-padding-bits=all" } */

typedef __SIZE_TYPE__ size_t;
extern int memcmp (const void *, const void *, size_t);
extern void *memcpy (void *, const void *, size_t);
extern void *memset (void *, int, size_t);

#define NI __attribute__((noipa))

static int wrong;

NI void
poison (void)
{
  volatile unsigned char b[512];
  for (int i = 0; i < 512; i++)
    b[i] = 0xff;
}

NI void
sink (unsigned char *o, const void *s, size_t n)
{
  memcpy (o, s, n);
}

NI void
check (const void *p, const void *want, size_t n)
{
  if (memcmp (p, want, n))
    wrong++;
}

/* A union nested in a struct, every member initialized.  */
struct s1 { union { int f : 24; } u; int x; };
NI void
t1 (unsigned char *o, int v)
{
  struct s1 d = { .u = { .f = v }, .x = 1 };
  sink (o, &d, sizeof d);
}

/* An anonymous union with a wider sibling.  */
struct s2 { union { long long f : 40; long long l; }; int x; int y; };
NI void
t2 (unsigned char *o, long long v)
{
  struct s2 d = { .f = v, .x = 1, .y = 2 };
  sink (o, &d, sizeof d);
}

/* A member struct that holds the bit-field.  */
union u3 { struct { int f : 24; } s; int x; };
NI void
t3 (unsigned char *o, int v)
{
  union u3 d = { .s = { .f = v } };
  sink (o, &d, sizeof d);
}

/* A one-byte union with a three-bit field.  */
union u4 { unsigned char c : 3; };
NI void
t4 (unsigned char *o, int v)
{
  union u4 d = { .c = v };
  sink (o, &d, sizeof d);
}

/* An array of unions.  */
union u5 { int f : 24; };
NI void
t5 (unsigned char *o, int v)
{
  union u5 d[2] = { { .f = v }, { .f = v } };
  sink (o, d, sizeof d);
}

/* A compound literal assigned through a pointer.  */
union u6 { unsigned long long f : 56; char c; };
NI void
t6 (union u6 *p, unsigned long long v)
{
  *p = (union u6) { .f = v };
}

/* A constant initializer.  */
NI void
t7 (unsigned char *o)
{
  union u5 d = { .f = 0x123456 };
  sink (o, &d, sizeof d);
}

/* A 17-bit field beside an 8-bit one.  */
union u8 { struct { int pad : 8; } p; int f : 17; };
NI void
t8 (unsigned char *o, int v)
{
  union u8 d = { .f = v };
  sink (o, &d, sizeof d);
}

int
main (void)
{
  unsigned char b[32];

  poison ();
  t1 (b, 0x123456);
  {
    struct s1 w;
    memset (&w, 0, sizeof w);
    w.u.f = 0x123456;
    w.x = 1;
    check (b, &w, sizeof w);
  }
  poison ();
  t2 (b, 0x1122334455LL);
  {
    struct s2 w;
    memset (&w, 0, sizeof w);
    w.f = 0x1122334455LL;
    w.x = 1;
    w.y = 2;
    check (b, &w, sizeof w);
  }
  poison ();
  t3 (b, 0x123456);
  {
    union u3 w;
    memset (&w, 0, sizeof w);
    w.s.f = 0x123456;
    check (b, &w, sizeof w);
  }
  poison ();
  t4 (b, 5);
  {
    union u4 w;
    memset (&w, 0, sizeof w);
    w.c = 5;
    check (b, &w, sizeof w);
  }
  poison ();
  t5 (b, 0x123456);
  {
    union u5 w[2];
    memset (w, 0, sizeof w);
    w[0].f = 0x123456;
    w[1].f = 0x123456;
    check (b, w, sizeof w);
  }
  {
    union u6 g, w;
    memset (&g, 0xff, sizeof g);
    t6 (&g, 0x11223344556677ULL);
    memset (&w, 0, sizeof w);
    w.f = 0x11223344556677ULL;
    check (&g, &w, sizeof w);
  }
  poison ();
  t7 (b);
  {
    union u5 w;
    memset (&w, 0, sizeof w);
    w.f = 0x123456;
    check (b, &w, sizeof w);
  }
  poison ();
  t8 (b, 0xffff);
  {
    union u8 w;
    memset (&w, 0, sizeof w);
    w.f = 0xffff;
    check (b, &w, sizeof w);
  }
  if (wrong)
    __builtin_abort ();
  return 0;
}
