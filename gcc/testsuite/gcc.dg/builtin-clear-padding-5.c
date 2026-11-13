/* __builtin_clear_padding on _BitInt members of structs, arrays and unions
   with a scalar_storage_order attribute.  One of the two orders is the
   reverse of the target's, which swaps the bytes of each _BitInt that has
   an integer mode, so its padding bits move to the other end.  */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23 -O2" } */

#if __BITINT_MAXWIDTH__ >= 100
#define WIDE(order, sfx) \
  struct __attribute__((scalar_storage_order (order))) M##sfx \
  { \
    unsigned char tag; \
    unsigned _BitInt(100) a; \
    _BitInt(70) b[2]; \
  }; \
  typedef struct M##sfx M##sfx;
#else
#define WIDE(order, sfx)
#endif

#define TYPES(order, sfx) \
  struct __attribute__((scalar_storage_order (order))) S##sfx \
  { \
    unsigned _BitInt(20) a; \
    unsigned char b; \
    _BitInt(12) c; \
    unsigned _BitInt(7) d; \
  }; \
  struct __attribute__((scalar_storage_order (order))) A##sfx \
  { \
    unsigned _BitInt(12) d[2]; \
    unsigned _BitInt(20) e[20]; \
  }; \
  union __attribute__((scalar_storage_order (order))) U##sfx \
  { \
    unsigned _BitInt(20) f; \
    unsigned char g; \
  }; \
  typedef struct S##sfx S##sfx; \
  typedef struct A##sfx A##sfx; \
  typedef union U##sfx U##sfx; \
  WIDE (order, sfx)

TYPES ("big-endian", be)
TYPES ("little-endian", le)

/* A struct in one order holding a struct in the other order.  */
typedef struct __attribute__((scalar_storage_order ("big-endian"))) Nbe
{
  Sle s;
  unsigned _BitInt(20) y;
} Nbe;
typedef struct __attribute__((scalar_storage_order ("little-endian"))) Nle
{
  Sbe s;
  unsigned _BitInt(20) y;
} Nle;

#define TEST(type, set, check) \
  type type##1, type##2; \
  __attribute__((noipa)) void \
  set_##type (type *x) \
  { \
    set; \
  } \
  __attribute__((noipa)) void \
  clear_##type (type *x) \
  { \
    __builtin_clear_padding (x); \
  } \
  __attribute__((noipa)) int \
  check_##type (type *x) \
  { \
    return check; \
  } \
  __attribute__((noipa)) void \
  test_##type (void) \
  { \
    __builtin_memset (&type##1, 0, sizeof (type)); \
    __builtin_memset (&type##2, -1, sizeof (type)); \
    set_##type (&type##1); \
    set_##type (&type##2); \
    clear_##type (&type##2); \
    if (!check_##type (&type##2) \
	|| __builtin_memcmp (&type##1, &type##2, sizeof (type))) \
      __builtin_abort (); \
  }

#define SET_S x->a = 0xabcde; x->b = 0x5a; x->c = 0x123; x->d = 0x55
#define CHECK_S \
  x->a == 0xabcde && x->b == 0x5a && x->c == 0x123 && x->d == 0x55
#define SET_A \
  x->d[0] = 0xabc; x->d[1] = 0x123; \
  for (int i = 0; i < 20; i++) \
    x->e[i] = 0xabcde + i
#define CHECK_A \
  ({ int ok = x->d[0] == 0xabc && x->d[1] == 0x123; \
     for (int i = 0; i < 20; i++) \
       ok &= x->e[i] == 0xabcde + i; \
     ok; })
#define SET_N \
  x->s.a = 0xabcde; x->s.b = 0x5a; x->s.c = 0x123; x->s.d = 0x55; \
  x->y = 0x12345
#define CHECK_N \
  x->s.a == 0xabcde && x->s.b == 0x5a && x->s.c == 0x123 \
  && x->s.d == 0x55 && x->y == 0x12345
#define SET_M \
  x->tag = 42; x->a = 0x123456789abcdef0123456789uwb; \
  x->b[0] = 0x123456789abcdef012wb; x->b[1] = 0x3edcba9876543210fwb
#define CHECK_M \
  x->tag == 42 && x->a == 0x123456789abcdef0123456789uwb \
  && x->b[0] == 0x123456789abcdef012wb \
  && x->b[1] == 0x3edcba9876543210fwb

TEST (Sbe, SET_S, CHECK_S)
TEST (Sle, SET_S, CHECK_S)
TEST (Abe, SET_A, CHECK_A)
TEST (Ale, SET_A, CHECK_A)
TEST (Ube, x->f = 0xabcde, x->f == 0xabcde)
TEST (Ule, x->f = 0xabcde, x->f == 0xabcde)
TEST (Nbe, SET_N, CHECK_N)
TEST (Nle, SET_N, CHECK_N)
#if __BITINT_MAXWIDTH__ >= 100
TEST (Mbe, SET_M, CHECK_M)
TEST (Mle, SET_M, CHECK_M)
#endif

int
main ()
{
  test_Sbe ();
  test_Sle ();
  test_Abe ();
  test_Ale ();
  test_Ube ();
  test_Ule ();
  test_Nbe ();
  test_Nle ();
#if __BITINT_MAXWIDTH__ >= 100
  test_Mbe ();
  test_Mle ();
#endif
  return 0;
}
