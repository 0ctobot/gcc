/* PR c/105430 */
/* { dg-do run } */
/* { dg-options "" } */

/* A range designator gives every element of the range the same compound
   literal or braced list.  Initializing an empty union member of one of
   those elements with a call used to store the call in that shared list,
   so it ran for every element using the list or crashed the compiler.  The
   call runs once, the element keeps the value the range gave it, as in
   pr105430-4.c, and the other elements aren't affected.  */

extern void abort (void);

struct E { };
union U { int i; struct E e; short s; };
struct P { int a; struct E e; int b; };
struct W { union U u; int k; };
int calls;

__attribute__ ((noipa)) int
mi (void)
{
  calls++;
  return 7;
}

__attribute__ ((noipa)) struct E
me (void)
{
  struct E e;
  calls += 10;
  return e;
}

__attribute__ ((noipa)) void
test_unions (int n)
{
  calls = 0;
  union U u1[2] = { [0 ... 1] = (union U) { .i = n }, [0].e = me () };
  if (calls != 10 || u1[0].i != n || u1[1].i != n)
    abort ();

  calls = 0;
  union U u2[2] = { [0 ... 1] = { .i = n }, [0].e = me () };
  if (calls != 10 || u2[0].i != n || u2[1].i != n)
    abort ();

  calls = 0;
  union U u3[3] = { [0 ... 2] = (union U) { .i = n }, [1].e = me () };
  if (calls != 10 || u3[0].i != n || u3[1].i != n || u3[2].i != n)
    abort ();

  calls = 0;
  union U u4[2] = { [0 ... 1] = (union U) { .i = n }, [0 ... 1].e = me () };
  if (calls != 10 || u4[0].i != n || u4[1].i != n)
    abort ();

  calls = 0;
  union U u5[2] = { [0 ... 1] = (union U) { .i = mi () }, [0].e = me () };
  if (calls != 11 || u5[0].i != 7 || u5[1].i != 7)
    abort ();

  /* An empty member initialized without side effects.  */
  union U u6[2] = { [0 ... 1] = (union U) { .i = n }, [0].e = (struct E) { } };
  union U u7[2] = { [0 ... 1] = { .i = n }, [0].e = { } };
  if (u6[0].i != n || u6[1].i != n || u7[0].i != n || u7[1].i != n)
    abort ();

  /* A member that holds bytes replaces the value.  */
  union U u8[2] = { [0 ... 1] = (union U) { .i = n }, [0].s = 5 };
  if (u8[0].s != 5 || u8[1].i != n)
    abort ();

  calls = 0;
  union U u9[1] = { [0] = (union U) { .i = n }, [0].e = me () };
  if (calls != 10 || u9[0].i != n)
    abort ();
}

__attribute__ ((noipa)) void
test_structs (int n)
{
  calls = 0;
  struct W w[2]
    = { [0 ... 1] = (struct W) { .u = { .i = n }, .k = 4 }, [0].u.e = me () };
  if (calls != 10
      || w[0].u.i != n || w[0].k != 4 || w[1].u.i != n || w[1].k != 4)
    abort ();

  calls = 0;
  struct P p[2] = { [0 ... 1] = (struct P) { .a = n, .b = 1 }, [0].e = me () };
  if (calls != 10
      || p[0].a != n || p[0].b != 1 || p[1].a != n || p[1].b != 1)
    abort ();
}

int
main (void)
{
  test_unions (3);
  test_structs (3);
  return 0;
}
