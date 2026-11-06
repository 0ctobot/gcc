/* PR tree-optimization/128076 */
/* { dg-do compile } */
/* { dg-additional-options "-Wno-psabi" } */

/* Arithmetic following a call that can't reach the return value has no
   conversion to the type of the tail recursion accumulators.  */

typedef int v2si __attribute__ ((vector_size (8)));
typedef int v4si __attribute__ ((vector_size (16)));
typedef int v16si __attribute__ ((vector_size (64)));
struct s { int i; };
struct big { long l[8]; };

double getd (void);
int geti (void);
v2si getv2 (void);
v16si getv16 (void);
_Complex double getcd (void);

double sinkd;
int sinki;
v2si sinkv2;
v16si sinkv16;
_Complex double sinkcd;
v4si gv4;
struct big gbig;

int
real_in_int (void)
{
  double x = getd ();
  x *= 70;
  x = x < 70 ? x : 70;
  x += 30;
  return x;
}

void
negate_in_void (void)
{
  int x = geti ();
  sinki = -x;
}

void
mult_in_void (void)
{
  int x = geti ();
  sinki = x * 3;
}

struct s
int_in_small_struct (void)
{
  struct s r;
  int x = geti ();
  r.i = x * 3;
  return r;
}

struct s
negate_in_small_struct (void)
{
  struct s r;
  int x = geti ();
  r.i = -x;
  return r;
}

struct big
int_in_big_struct (void)
{
  int x = geti ();
  sinki = x + 3;
  return gbig;
}

void *
real_in_pointer (void)
{
  double x = getd ();
  sinkd = x * 2.0;
  return 0;
}

int
vector_in_int (void)
{
  v2si x = getv2 ();
  sinkv2 = x * 2;
  return 0;
}

v4si
vector_in_vector (void)
{
  v2si x = getv2 ();
  sinkv2 = x + 2;
  return gv4;
}

v4si
big_vector_in_vector (void)
{
  v16si x = getv16 ();
  sinkv16 = -x;
  return gv4;
}

_Complex int
complex_in_complex (void)
{
  _Complex double x = getcd ();
  sinkcd = x * 2.0;
  return 0;
}

double
int_in_real (void)
{
  int x = geti ();
  sinki = x + 1;
  return 1.0;
}
