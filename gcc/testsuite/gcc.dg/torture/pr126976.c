/* PR tree-optimization/126976 */
/* { dg-do run } */
/* { dg-additional-options "-Wno-psabi" } */

/* A vector CONSTRUCTOR whose lanes are extracted through different signs
   mustn't be merged into a permutation followed by one conversion, unless
   that conversion doesn't care about the sign of its source.  */

typedef signed char v8qi __attribute__ ((vector_size (8)));
typedef unsigned char v8uqi __attribute__ ((vector_size (8)));
typedef signed char v16qi __attribute__ ((vector_size (16)));
typedef unsigned char v16uqi __attribute__ ((vector_size (16)));
typedef short v8hi __attribute__ ((vector_size (16)));
typedef unsigned short v8uhi __attribute__ ((vector_size (16)));
typedef int v4si __attribute__ ((vector_size (16)));
typedef unsigned int v4usi __attribute__ ((vector_size (16)));
typedef float v4sf __attribute__ ((vector_size (16)));

/* Two sources.  */
__attribute__ ((noipa)) v8hi
two_src (v8qi a, v8qi b)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[0], au[1], au[2], au[3], b[4], b[5], b[6], b[7] };
}

/* Two sources, signed lanes first.  */
__attribute__ ((noipa)) v8hi
two_src_rev (v8qi a, v8qi b)
{
  v8uqi bu = (v8uqi) b;
  return (v8hi) { a[0], a[1], a[2], a[3], bu[4], bu[5], bu[6], bu[7] };
}

/* One source at the identity positions.  */
__attribute__ ((noipa)) v8hi
ident (v8qi a)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[0], a[1], au[2], a[3], au[4], a[5], au[6], a[7] };
}

/* One source at the identity positions, signed lanes first.  */
__attribute__ ((noipa)) v8hi
ident_rev (v8qi a)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { a[0], au[1], a[2], au[3], a[4], au[5], a[6], au[7] };
}

/* One source, permuted.  */
__attribute__ ((noipa)) v8hi
perm (v8qi a)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[7], a[6], au[5], a[4], au[3], a[2], au[1], a[0] };
}

/* Source with more lanes than the output, identity positions.  */
__attribute__ ((noipa)) v8hi
lowpart (v16qi a)
{
  v16uqi au = (v16uqi) a;
  return (v8hi) { au[0], a[1], au[2], a[3], au[4], a[5], au[6], a[7] };
}

/* Source with more lanes than the output, permuted.  */
__attribute__ ((noipa)) v8hi
highpart (v16qi a)
{
  v16uqi au = (v16uqi) a;
  return (v8hi) { au[8], a[9], au[10], a[11], au[12], a[13], au[14], a[15] };
}

/* Constant lanes next to the extracted ones.  */
__attribute__ ((noipa)) v8hi
with_const (v8qi a)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[0], a[1], au[2], a[3], 1000, 1000, 1000, 1000 };
}

/* One variable lane next to the extracted ones.  */
__attribute__ ((noipa)) v8hi
with_var (v8qi a, short x)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[0], a[1], au[2], a[3], x, x, x, x };
}

/* Integer to float conversion reads the source sign as well.  */
__attribute__ ((noipa)) v4sf
to_float (v4si a)
{
  v4usi au = (v4usi) a;
  return (v4sf) { au[0], a[1], au[2], a[3] };
}

/* Truncation keeps the low bits, so mixed signs are fine here.  */
__attribute__ ((noipa)) v8qi
narrow (v8hi a)
{
  v8uhi au = (v8uhi) a;
  return (v8qi) { au[0], a[1], au[2], a[3], au[4], a[5], au[6], a[7] };
}

__attribute__ ((noipa)) v8qi
narrow_perm (v8hi a)
{
  v8uhi au = (v8uhi) a;
  return (v8qi) { au[7], a[6], au[5], a[4], au[3], a[2], au[1], a[0] };
}

/* All lanes agree on a sign that differs from the source vector's.  */
__attribute__ ((noipa)) v8hi
uniform_unsigned (v8qi a)
{
  v8uqi au = (v8uqi) a;
  return (v8hi) { au[0], au[1], au[2], au[3], au[4], au[5], au[6], au[7] };
}

__attribute__ ((noipa)) v8hi
uniform_signed (v8uqi a)
{
  v8qi as = (v8qi) a;
  return (v8hi) { as[7], as[6], as[5], as[4], as[3], as[2], as[1], as[0] };
}

#define CHECK(n, res, ...)						\
  do									\
    {									\
      short want_[n] = { __VA_ARGS__ };					\
      __typeof__ (res) got_ = (res);					\
      for (int i_ = 0; i_ < n; i_++)					\
	if (got_[i_] != want_[i_])					\
	  __builtin_abort ();						\
    }									\
  while (0)

#define S(x) ((signed char) (x))
#define U(x) ((unsigned char) (x))

int
main (void)
{
  v8qi a = { -1, -2, -3, -4, -5, -6, -7, -8 };
  v8qi b = { -10, -20, -30, -40, -50, -60, -70, -128 };
  v16qi c = { -1, -2, -3, -4, -5, -6, -7, -8,
	      -9, -10, -11, -12, -13, -14, -15, -16 };
  v8hi h = { 0x7f80, 0x7f81, 0x7f82, 0x7f83,
	     0x7f84, 0x7f85, 0x7f86, 0x7f87 };
  v4si f = { -5, -6, -7, -8 };

  CHECK (8, two_src (a, b),
	 U (-1), U (-2), U (-3), U (-4), S (-50), S (-60), S (-70), S (-128));
  CHECK (8, two_src_rev (a, b),
	 S (-1), S (-2), S (-3), S (-4), U (-50), U (-60), U (-70), U (-128));
  CHECK (8, ident (a),
	 U (-1), S (-2), U (-3), S (-4), U (-5), S (-6), U (-7), S (-8));
  CHECK (8, ident_rev (a),
	 S (-1), U (-2), S (-3), U (-4), S (-5), U (-6), S (-7), U (-8));
  CHECK (8, perm (a),
	 U (-8), S (-7), U (-6), S (-5), U (-4), S (-3), U (-2), S (-1));
  CHECK (8, lowpart (c),
	 U (-1), S (-2), U (-3), S (-4), U (-5), S (-6), U (-7), S (-8));
  CHECK (8, highpart (c),
	 U (-9), S (-10), U (-11), S (-12), U (-13), S (-14), U (-15), S (-16));
  CHECK (8, with_const (a),
	 U (-1), S (-2), U (-3), S (-4), 1000, 1000, 1000, 1000);
  CHECK (8, with_var (a, -999),
	 U (-1), S (-2), U (-3), S (-4), -999, -999, -999, -999);
  CHECK (8, narrow (h),
	 S (0x80), S (0x81), S (0x82), S (0x83),
	 S (0x84), S (0x85), S (0x86), S (0x87));
  CHECK (8, narrow_perm (h),
	 S (0x87), S (0x86), S (0x85), S (0x84),
	 S (0x83), S (0x82), S (0x81), S (0x80));
  CHECK (8, uniform_unsigned (a),
	 U (-1), U (-2), U (-3), U (-4), U (-5), U (-6), U (-7), U (-8));
  CHECK (8, uniform_signed ((v8uqi) a),
	 S (-8), S (-7), S (-6), S (-5), S (-4), S (-3), S (-2), S (-1));

  v4sf r = to_float (f);
  if (r[0] != (float) (unsigned int) -5 || r[1] != -6.0f
      || r[2] != (float) (unsigned int) -7 || r[3] != -8.0f)
    __builtin_abort ();

  return 0;
}
