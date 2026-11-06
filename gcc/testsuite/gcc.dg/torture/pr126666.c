/* { dg-do run } */

/* veclower visited the first block below before lowering the VEC_COND_EXPR
   that defines c, folded ~w through its definition and built a new
   VEC_COND_EXPR that was never lowered, causing an ICE in isel.  */

typedef signed char vs __attribute__ ((vector_size (2)));
typedef unsigned char vu __attribute__ ((vector_size (2)));

static vu buf = { 0, 3 };
static int count;

__attribute__ ((noipa)) int
tick (void)
{
  return ++count;
}

__attribute__ ((noipa)) void
check (void)
{
  if (buf[0] != 0 || buf[1] != 1)
    __builtin_abort ();
  __builtin_exit (0);
}

__attribute__ ((noipa)) void
f (vu *p)
{
  vs c;
  goto head;
side:
  check ();
body:
  {
    unsigned short s = (unsigned short) c;
    s = ~s;
    vu w = (vu) s;
    *p = ~w;
  }
head:
  c = (*p != (vu) {}) & (vs) { 1, 1 };
  if (tick () + c[0] == 2)
    goto side;
  goto body;
}

int
main (void)
{
  f (&buf);
}
