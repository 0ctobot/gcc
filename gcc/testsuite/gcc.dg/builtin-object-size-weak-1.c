/* A weak definition of an object with an initialized flexible array member
   can be replaced at link time by a strong definition with a different
   number of elements, so the size of the member mustn't be taken from the
   weak initializer.  The strong definitions are in
   builtin-object-size-weak-1a.c.  */
/* { dg-do run } */
/* { dg-require-weak "" } */
/* { dg-options "-O2" } */
/* { dg-additional-sources "builtin-object-size-weak-1a.c" } */

typedef __SIZE_TYPE__ size_t;

struct S { int n; int a[]; };

/* The prevailing W has four elements and the prevailing V has one.  */
__attribute__ ((weak)) struct S w = { 2, { 1, 2 } };
__attribute__ ((weak)) struct S v = { 4, { 1, 2, 3, 4 } };

#define SIZE(name, expr, type) \
  __attribute__ ((noipa)) size_t \
  name (void) \
  { \
    return __builtin_object_size (expr, type); \
  } \
  __attribute__ ((noipa)) size_t \
  dyn_##name (void) \
  { \
    return __builtin_dynamic_object_size (expr, type); \
  }

SIZE (w_0, w.a, 0)
SIZE (w_1, w.a, 1)
SIZE (w_obj, &w, 0)
SIZE (v_2, v.a, 2)
SIZE (v_3, v.a, 3)
SIZE (v_obj, &v, 2)

int
main (void)
{
  if (w.n != 4 || v.n != 1)
    __builtin_abort ();

  /* A maximum must cover the four elements of the prevailing W.  */
  if (w_0 () < 4 * sizeof (int) || dyn_w_0 () < 4 * sizeof (int)
      || w_1 () < 4 * sizeof (int) || dyn_w_1 () < 4 * sizeof (int)
      || w_obj () < sizeof w + 4 * sizeof (int)
      || dyn_w_obj () < sizeof w + 4 * sizeof (int))
    __builtin_abort ();

  /* A minimum mustn't exceed the one element of the prevailing V.  */
  if (v_2 () > sizeof (int) || dyn_v_2 () > sizeof (int)
      || v_3 () > sizeof (int) || dyn_v_3 () > sizeof (int)
      || v_obj () > sizeof v + sizeof (int)
      || dyn_v_obj () > sizeof v + sizeof (int))
    __builtin_abort ();

  return 0;
}
