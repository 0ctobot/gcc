/* __builtin_clear_padding on structs and unions with bit-fields and a
   scalar_storage_order attribute.  One of the two orders is the reverse
   of the target's, which numbers the bits of a bit-field the other way
   around within each byte.  */
/* { dg-do run } */
/* { dg-require-effective-target int32plus } */
/* { dg-options "-O2" } */

#define TYPES(order, sfx) \
  struct __attribute__((scalar_storage_order (order))) S##sfx \
  { \
    unsigned char a : 3; \
    unsigned int b : 9; \
    unsigned int c : 13; \
  }; \
  union __attribute__((scalar_storage_order (order))) U##sfx \
  { \
    unsigned int f : 3; \
  }; \
  union __attribute__((scalar_storage_order (order))) V##sfx \
  { \
    unsigned int f : 20; \
    unsigned short g : 11; \
  }; \
  struct W##sfx \
  { \
    unsigned char tag; \
    union U##sfx u[2]; \
  }; \
  typedef struct S##sfx S##sfx; \
  typedef union U##sfx U##sfx; \
  typedef union V##sfx V##sfx; \
  typedef struct W##sfx W##sfx;

TYPES ("big-endian", be)
TYPES ("little-endian", le)

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

#define SET_S x->a = 5; x->b = 0x155; x->c = 0x1555
#define CHECK_S x->a == 5 && x->b == 0x155 && x->c == 0x1555
#define SET_W x->tag = 42; x->u[0].f = 5; x->u[1].f = 3
#define CHECK_W x->tag == 42 && x->u[0].f == 5 && x->u[1].f == 3

TEST (Sbe, SET_S, CHECK_S)
TEST (Sle, SET_S, CHECK_S)
TEST (Ube, x->f = 5, x->f == 5)
TEST (Ule, x->f = 5, x->f == 5)
TEST (Vbe, x->f = 0xaaaaa, x->f == 0xaaaaa)
TEST (Vle, x->f = 0xaaaaa, x->f == 0xaaaaa)
TEST (Wbe, SET_W, CHECK_W)
TEST (Wle, SET_W, CHECK_W)

int
main ()
{
  test_Sbe ();
  test_Sle ();
  test_Ube ();
  test_Ule ();
  test_Vbe ();
  test_Vle ();
  test_Wbe ();
  test_Wle ();
  return 0;
}
