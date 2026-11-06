/* PR c/105430 */
/* { dg-do run } */
/* { dg-additional-options "-Wno-psabi" } */

/* Brace elision into a generic vector keeps the elements of the whole
   initializer that the new initializers don't reach.  */

extern void abort (void);

#define CK(c) do { if (!(c)) abort (); } while (0)

typedef int v4si __attribute__((vector_size (16)));
typedef long long v2di __attribute__((vector_size (16)));

struct V { v4si v; int z; };
struct L { v2di v; };

struct V s1 = { .v = (v4si) { 1, 2, 3, 4 }, .v = 9 };
static struct L s2 = { .v = { 11, 2 }, .v = 9 };

__attribute__((noipa)) void
autos (v4si w, int a)
{
  struct V l1 = { .v = w, .v = 9 };
  CK (l1.v[0] == 9 && l1.v[1] == 2 && l1.v[2] == 3 && l1.v[3] == 4);
  struct V l2 = { .v = { 1, 2, 3, 4 }, .v = 9 };
  CK (l2.v[0] == 9 && l2.v[1] == 2 && l2.v[2] == 3 && l2.v[3] == 4);
  struct V l3 = { .v = { a, 2, 3, 4 }, .v = 9 };
  CK (l3.v[0] == 9 && l3.v[1] == 2 && l3.v[2] == 3 && l3.v[3] == 4);
  struct V l4 = { .v = (v4si) { 1, 2, 3, 4 }, .v = 9 };
  CK (l4.v[0] == 9 && l4.v[1] == 2 && l4.v[2] == 3 && l4.v[3] == 4);
  v4si a1[2] = { [0] = w, [0] = 9 };
  CK (a1[0][0] == 9 && a1[0][1] == 2 && a1[0][2] == 3 && a1[0][3] == 4);
  CK (a1[1][0] == 0);
}

int
main (void)
{
  v4si w = { 1, 2, 3, 4 };
  CK (s1.v[0] == 9 && s1.v[1] == 2 && s1.v[2] == 3 && s1.v[3] == 4);
  CK (s2.v[0] == 9 && s2.v[1] == 2);
  autos (w, 1);
  return 0;
}
