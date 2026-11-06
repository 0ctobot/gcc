/* A compound literal with static storage can hold data for a flexible array
   member, and the literal or the object it initializes must have room for that
   data.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -O2" } */

struct F { int a; int f[]; };
struct S { int a; char s[]; };

struct F t0 = (struct F) { 1, { 2, 3 } };
int after0 = 10;
struct F *p0 = &(struct F) { 4, { 5, 6 } };
int after1 = 11;
struct S t2 = (struct S) { 1, "abc" };
int after2 = 12;
struct S t3 = (struct S) { 1, { 'a', 'b', 0 } };
int after3 = 13;
const struct F t4 = (const struct F) { 7, { 8, 9 } };
int after4 = 14;
struct F t5 = t4;
int after5 = 15;

int
main (void)
{
  if (t0.a != 1 || t0.f[0] != 2 || t0.f[1] != 3 || after0 != 10)
    __builtin_abort ();
  if (p0->a != 4 || p0->f[0] != 5 || p0->f[1] != 6 || after1 != 11)
    __builtin_abort ();
  if (__builtin_strcmp (t2.s, "abc") || after2 != 12)
    __builtin_abort ();
  if (__builtin_strcmp (t3.s, "ab") || after3 != 13)
    __builtin_abort ();
  if (t4.f[1] != 9 || after4 != 14)
    __builtin_abort ();
  if (t5.a != 7 || after5 != 15)
    __builtin_abort ();
  return 0;
}
