/* PR middle-end/128302 */
/* __builtin_clear_padding on a struct or union with a _BitInt bit-field
   that spans more bytes than the buffer its padding is computed in.  */
/* { dg-do run { target bitint } } */
/* { dg-options "-std=c23 -O2" } */

#if __BITINT_MAXWIDTH__ >= 65535
struct S { unsigned _BitInt(65535) f : 65534; };
union U { unsigned _BitInt(65535) f : 65534; };
struct T { unsigned char c : 3; unsigned _BitInt(65535) f : 65530; };
union V { struct T t; unsigned char d; };

#define TEST(type, set) \
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
  __attribute__((noipa)) void \
  test_##type (void) \
  { \
    __builtin_memset (&type##1, 0, sizeof (type)); \
    __builtin_memset (&type##2, -1, sizeof (type)); \
    set_##type (&type##1); \
    clear_##type (&type##2); \
    if (__builtin_memcmp (&type##1, &type##2, sizeof (type))) \
      __builtin_abort (); \
  }

typedef struct S S;
typedef union U U;
typedef struct T T;
typedef union V V;
TEST (S, x->f = -1)
TEST (U, x->f = -1)
TEST (T, x->c = -1; x->f = -1)
TEST (V, x->t.c = -1; x->t.f = -1)
#endif

int
main ()
{
#if __BITINT_MAXWIDTH__ >= 65535
  test_S ();
  test_U ();
  test_T ();
  test_V ();
#endif
  return 0;
}
