/* PR c/128335 */
/* { dg-do compile } */
/* { dg-options "-std=c23" } */

/* During the redefinition of an enum with a fixed underlying type, a
   redeclared enumerator has the same type as the tag.  */

enum E : unsigned char { a1 = 1, a2, a3 = _Generic(a1, enum E: 7, default: 8) };
static_assert(a2 == 2);
static_assert(a3 == 7);
enum E : unsigned char { a1 = 1, a2, a3 = _Generic(a1, enum E: 7, default: 8) };
static_assert(a2 == 2);
static_assert(a3 == 7);
static_assert(_Generic(a1, enum E: 1, default: 0));
static_assert(_Generic(a3, enum E: 1, default: 0));

/* It also selects the tag over another enum with a third definition.  */
enum F : long { b1 = 1 };
enum G : long { c1 = 1, c2 = _Generic(c1, enum G: 2, default: 3) };
enum G : long { c1 = 1, c2 = _Generic(c1, enum F: 3, enum G: 2) };
enum G : long { c1 = 1, c2 = _Generic(c1, enum F: 3, enum G: 2) };
static_assert(c2 == 2);
static_assert(_Generic(c1, enum F: 0, enum G: 1));

/* Negative values and the implicit next value.  */
enum H : short { d1 = -1, d2, d3 = sizeof (typeof (d1)) };
enum H : short { d1 = -1, d2, d3 = sizeof (typeof (d1)) };
static_assert(d2 == 0);
static_assert(d3 == sizeof (short));

/* A bool underlying type uses a different increment.  */
enum B : bool { e1, e2, e3 = 1 };
enum B : bool { e1, e2, e3 = _Generic(e2, enum B: 1, default: 0) };
static_assert(e2 == 1);

/* Same in function scope, including a tag shadowing a file-scope one.  */
void
f (void)
{
  enum E : unsigned char { a1 = 3, a3 = _Generic(a1, enum E: 4, default: 5) };
  static_assert(a3 == 4);
  enum E : unsigned char { a1 = 3, a3 = _Generic(a1, enum E: 4, default: 5) };
  static_assert(a3 == 4);
}

/* A different underlying type is still a conflict.  */
enum J : char { g1 = 1 };
enum J : short { g1 = 1 };	/* { dg-error "underlying type incompatible" } */
