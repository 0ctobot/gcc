/* { dg-do run } */

/* A nested function reads the variables of its parent through the static
   chain, so the address of such a variable needn't be taken for a call in a
   second run of its initializer to read the old value.  */

struct S { long a, b, c[6]; };
typedef long A[8];

__attribute__ ((noipa)) void
opaque (void)
{
}

__attribute__ ((noipa)) int
yes (void)
{
  return 1;
}

/* MK reads the old value after writing to its result, which is the variable
   itself if the variable is used as the return slot.  */

__attribute__ ((noipa)) long
t_call (void)
{
  int n = 0;
  auto struct S mk (void);
again:;
  struct S v = mk ();
  __attribute__ ((noinline)) struct S
  mk (void)
  {
    struct S r;
    r.a = n + 1;
    for (int i = 0; i < 6; i++)
      r.c[i] = n + 1;
    opaque ();
    r.b = n ? v.c[0] : -1;
    return r;
  }
  if (n++ < 1)
    goto again;
  return v.b;
}

/* RD reads the old value while the new one is being built.  */

#define RUN(name, type, old, get, ...)					\
  __attribute__ ((noipa)) long						\
  name (void)								\
  {									\
    int n = 0;								\
    auto long rd (void);						\
  again:;								\
    type v = __VA_ARGS__;						\
    __attribute__ ((noinline)) long					\
    rd (void)								\
    {									\
      return n ? old : -1;						\
    }									\
    if (n++ < 1)							\
      goto again;							\
    return get;								\
  }

RUN (t_lit, struct S, v.a, v.b, (struct S) { .a = n + 1, .b = rd () })
RUN (t_cond, struct S, v.a, v.b,
     yes () ? (struct S) { .a = n + 1, .b = rd () } : (struct S) { .a = 0 })
RUN (t_comma, struct S, v.a, v.b,
     (yes (), (struct S) { .a = n + 1, .b = rd () }))
RUN (t_brace, struct S, v.a, v.b, { .a = n + 1, .b = rd () })
RUN (t_brace_stmt, struct S, v.a, v.b,
     { .a = n + 1, .b = ({ long r = rd (); r; }) })
RUN (t_array, A, v[0], v[1], { n + 1, rd () })

int
main (void)
{
  if (t_call () != 1)
    __builtin_abort ();
  if (t_lit () != 1)
    __builtin_abort ();
  if (t_cond () != 1)
    __builtin_abort ();
  if (t_comma () != 1)
    __builtin_abort ();
  if (t_brace () != 1)
    __builtin_abort ();
  if (t_brace_stmt () != 1)
    __builtin_abort ();
  if (t_array () != 1)
    __builtin_abort ();
  return 0;
}
