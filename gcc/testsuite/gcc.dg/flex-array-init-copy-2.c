/* An assignment from a const object copies the size of its type, without the
   data of a flexible array member in the object's initializer.  */
/* { dg-do run } */
/* { dg-options "-std=gnu11 -O2" } */

struct F { int a; int f[]; };
struct WF { struct F f; int z; };
struct W2 { struct F f; int y; int z; };

static const struct F cf = { 1, { 2, 3 } };

__attribute__ ((noipa)) void
assign_ptr (struct F *p)
{
  *p = cf;
}

__attribute__ ((noipa)) void
assign_member (struct WF *p)
{
  p->f = cf;
  p->z = 1;
}

__attribute__ ((noipa)) int
auto_init (void)
{
  struct W2 t = { .f = cf };
  return t.y + t.z;
}

__attribute__ ((noipa)) int
auto_init_z (void)
{
  struct WF t = { .f = cf, .z = 8 };
  return t.z;
}

__attribute__ ((noipa)) int
auto_copy (void)
{
  struct F t = cf;
  return t.a;
}

int
main (void)
{
  struct W2 w = { .y = 20, .z = 30 };
  struct WF wf = { .z = 40 };

  assign_ptr (&w.f);
  if (w.f.a != 1 || w.y != 20 || w.z != 30)
    __builtin_abort ();

  assign_member (&wf);
  if (wf.f.a != 1 || wf.z != 1)
    __builtin_abort ();

  if (auto_init () != 0 || auto_init_z () != 8 || auto_copy () != 1)
    __builtin_abort ();
  return 0;
}
