/* A copy of a static compound literal copies the size of its type, without the
   data the literal holds for a flexible array member.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O2" } */

struct F { int a; int f[]; };
struct W2 { struct F f; int y; int z; };

__attribute__ ((noipa)) void
assign_ptr (struct F *p)
{
  *p = (static struct F) { 1, { 2, 3 } };
}

__attribute__ ((noipa)) void
assign_const_ptr (struct F *p)
{
  *p = (static const struct F) { 1, { 2, 3 } };
}

__attribute__ ((noipa)) void
assign_through_ptr (struct F *p)
{
  struct F *q = &(static struct F) { 1, { 2, 3 } };
  *p = *q;
}

__attribute__ ((noipa)) int
read_through_ptr (int i)
{
  struct F *q = &(static struct F) { 1, { 2, 3 } };
  return q->f[i];
}

__attribute__ ((noipa)) int
auto_init (void)
{
  struct W2 t = { .f = (static struct F) { 1, { 2, 3 } } };
  return t.f.a * 100 + t.y * 10 + t.z;
}

int
main (void)
{
  struct W2 w = { .y = 20, .z = 30 };

  assign_ptr (&w.f);
  if (w.f.a != 1 || w.y != 20 || w.z != 30)
    __builtin_abort ();

  w.f.a = 0;
  assign_const_ptr (&w.f);
  if (w.f.a != 1 || w.y != 20 || w.z != 30)
    __builtin_abort ();

  w.f.a = 0;
  assign_through_ptr (&w.f);
  if (w.f.a != 1 || w.y != 20 || w.z != 30)
    __builtin_abort ();

  if (read_through_ptr (1) != 3)
    __builtin_abort ();

  if (auto_init () != 100)
    __builtin_abort ();
  return 0;
}
