/* PR tree-optimization/126976 */
/* { dg-do run } */
/* { dg-options "-O2 -fgimple -Wno-psabi" } */

/* Same as gcc.dg/torture/pr126976.c, but with lanes that are vectors
   themselves.  C rejects vector-typed elements in a vector initializer, hence
   the GIMPLE input.  */

typedef int v2si __attribute__ ((vector_size (8)));
typedef unsigned int v2usi __attribute__ ((vector_size (8)));
typedef int v4si __attribute__ ((vector_size (16)));
typedef float v2sf __attribute__ ((vector_size (8)));
typedef float v4sf __attribute__ ((vector_size (16)));

__attribute__ ((noipa)) v4sf __GIMPLE (ssa, startwith ("forwprop1"))
f (v4si a)
{
  v2usi _1;
  v2si _2;
  v2sf _3;
  v2sf _4;
  v4sf _5;

  __BB(2):
  _1 = __BIT_FIELD_REF <v2usi> (a_6(D), 64, 0);
  _2 = __BIT_FIELD_REF <v2si> (a_6(D), 64, 64);
  _3 = (v2sf) _1;
  _4 = (v2sf) _2;
  _5 = _Literal (v4sf) { _3, _4 };
  return _5;
}

int
main (void)
{
  v4si a = { -1, -2, -3, -4 };
  v4sf r = f (a);
  if (r[0] != (float) (unsigned int) -1 || r[1] != (float) (unsigned int) -2
      || r[2] != -3.0f || r[3] != -4.0f)
    __builtin_abort ();
  return 0;
}
