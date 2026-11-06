/* PR c/65673 */
/* A compound literal of array type with no upper bound, written inside another
   initializer, used to wipe out that whole initializer with no diagnostic:
   static objects came out zeroed, while assignments, returns and function
   calls using the enclosing initializer disappeared from the code.  */
/* { dg-do run } */
/* { dg-options "-w" } */

struct s { int y; unsigned long *x; };
struct t { int y; unsigned long *x; int z; };
struct u { struct t in; int w; };
struct v { unsigned long *p[2]; int z; };
struct w { int y; unsigned long *x; int a[0]; int z; };
struct f { int n; int a[]; };
struct o { int a[0]; int b; };

extern struct f ff;

/* Static initializers, zero-length array literal.  */
struct s s1 = { .y = 1, .x = (unsigned long [0]){} };
struct t s2 = { .y = 2, .x = (unsigned long [0]){}, .z = 3 };
struct t s3 = { 4, (unsigned long [0]){}, 5 };
struct u s4 = { .in = { .y = 6, .x = (unsigned long [0]){}, .z = 7 }, .w = 8 };
struct v s5 = { .p = { (unsigned long [0]){}, 0 }, .z = 9 };
struct t s6[2] = { { 10, (unsigned long [0]){}, 11 }, { 12, 0, 13 } };
struct w s7 = { .y = 14, .x = (unsigned long [0]){}, .a = {}, .z = 15 };
struct t s8 = { .x = (unsigned long [0]){}, .z = 16, .y = 17 };
struct t s9 = { .y = 18, .x = (__typeof__ (unsigned long [0])){}, .z = 19 };
struct t s10 = { .y = 20, .x = (unsigned long [0]){ 1 }, .z = 21 };
struct t s11 = { .y = sizeof ((unsigned long [0]){}), .z = 22 };
struct t *s12 = &(struct t){ .y = 23, .x = (unsigned long [0]){}, .z = 24 };
struct t s13 = { .y = 25, .x = (unsigned long *)(struct t [0]){}, .z = 26 };

/* Static initializers, literal of a flexible array member's type.  */
struct t s14 = { .y = 27, .x = (unsigned long *)(__typeof__ (ff.a)){}, .z = 28 };
struct t s15 = { .y = 29, .x = (unsigned long *)(__typeof__ (ff.a)){ 30, 31 },
		 .z = 32 };

/* These always worked and have to keep working.  */
struct t s16 = { .y = 33, .x = (unsigned long *)(int [2][0]){ {}, {} }, .z = 34 };
struct t s17 = { .y = 35, .x = (unsigned long *)(char [0]){ "x" }, .z = 36 };
struct t s18 = { .y = 37, .x = (unsigned long *)&(struct o){ .a = {}, .b = 38 },
		 .z = 39 };
void *s19 = (int [0]){};
struct t s20 = { .y = 40, .x = (unsigned long [1]){ 41 }, .z = 42 };
struct t s21 = { .y = 43, .x = (unsigned long [3]){ [1] = 44 }, .z = 45 };

int calls;

__attribute__ ((noipa)) void
use_val (struct t v)
{
  calls += v.y == 50 && v.z == 51;
}

__attribute__ ((noipa)) void
use_ptr (struct t *v)
{
  calls += v->y == 52 && v->z == 53;
}

__attribute__ ((noipa)) void
use_ul (unsigned long *p, int y)
{
  calls += y == 54;
}

__attribute__ ((noipa)) struct t
ret_val (void)
{
  return (struct t){ .y = 55, .x = (unsigned long [0]){}, .z = 56 };
}

__attribute__ ((noipa)) int
auto_init (int a, int b)
{
  struct t l = { .y = a, .x = (unsigned long [0]){}, .z = b };
  return l.y == 57 && l.z == 58;
}

__attribute__ ((noipa)) int
assign (int a, int b)
{
  struct t l;
  l = (struct t){ .y = a, .x = (unsigned long [0]){}, .z = b };
  return l.y == 59 && l.z == 60;
}

__attribute__ ((noipa)) int
stmt_expr (int a)
{
  struct t l = { .y = a, .x = ({ unsigned long q[0] = {}; q; }), .z = a + 1 };
  return l.y == 61 && l.z == 62;
}

__attribute__ ((noipa)) int
auto_sizeof (int a)
{
  struct t l = { .y = a, .x = 0, .z = sizeof ((int [0]){}) };
  return l.y == 63 && l.z == 0;
}

__attribute__ ((noipa)) int
auto_flex (int a)
{
  struct t l = { .y = a, .x = (unsigned long *)(__typeof__ (ff.a)){ a, a + 2 },
		 .z = a + 1 };
  return l.y == 64 && l.z == 65 && ((int *) l.x)[1] == 66;
}

#define CHECK(c) do { if (!(c)) __builtin_abort (); } while (0)

int
main (void)
{
  CHECK (s1.y == 1);
  CHECK (s2.y == 2 && s2.z == 3);
  CHECK (s3.y == 4 && s3.z == 5);
  CHECK (s4.in.y == 6 && s4.in.z == 7 && s4.w == 8);
  CHECK (s5.z == 9);
  CHECK (s6[0].y == 10 && s6[0].z == 11 && s6[1].y == 12 && s6[1].z == 13);
  CHECK (s7.y == 14 && s7.z == 15);
  CHECK (s8.z == 16 && s8.y == 17);
  CHECK (s9.y == 18 && s9.z == 19);
  CHECK (s10.y == 20 && s10.z == 21);
  CHECK (s11.y == 0 && s11.z == 22);
  CHECK (s12 && s12->y == 23 && s12->z == 24);
  CHECK (s13.y == 25 && s13.z == 26);
  CHECK (s14.y == 27 && s14.z == 28);
  CHECK (s15.y == 29 && s15.z == 32 && ((int *) s15.x)[1] == 31);
  CHECK (s16.y == 33 && s16.z == 34);
  CHECK (s17.y == 35 && s17.z == 36);
  CHECK (s18.y == 37 && s18.z == 39 && ((struct o *) s18.x)->b == 38);
  CHECK (s19 != 0);
  CHECK (s20.y == 40 && s20.z == 42 && *s20.x == 41);
  CHECK (s21.y == 43 && s21.z == 45 && s21.x[1] == 44);

  use_val ((struct t){ .y = 50, .x = (unsigned long [0]){}, .z = 51 });
  CHECK (calls == 1);
  use_ptr (&(struct t){ .y = 52, .x = (unsigned long [0]){}, .z = 53 });
  CHECK (calls == 2);
  use_ul ((unsigned long [0]){}, 54);
  CHECK (calls == 3);

  struct t rv = ret_val ();
  CHECK (rv.y == 55 && rv.z == 56);
  CHECK (auto_init (57, 58));
  CHECK (assign (59, 60));
  CHECK (stmt_expr (61));
  CHECK (auto_sizeof (63));
  CHECK (auto_flex (64));
  return 0;
}
