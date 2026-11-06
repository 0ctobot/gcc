/* { dg-do run } */

/* A pure nested function reads the variables of its parent through the static
   chain, so the address of such a variable needn't be taken for a call to it
   in a second run of the variable's initializer to read the old value.  The
   call has no side effects to show for that read.  */

struct S { long a, b, c[6]; };
typedef long A[8];

#define RUN(name, type, old, get, ...)					\
  __attribute__ ((noipa)) long						\
  name (void)								\
  {									\
    int n = 0;								\
    auto long rd (void);						\
  again:;								\
    type v = __VA_ARGS__;						\
    __attribute__ ((noinline, pure)) long				\
    rd (void)								\
    {									\
      return n ? old : -1;						\
    }									\
    if (n++ < 1)							\
      goto again;							\
    return get;								\
  }

RUN (t_lit, struct S, v.a, v.b, (struct S) { .a = n + 1, .b = rd () })
RUN (t_brace, struct S, v.a, v.b, { .a = n + 1, .b = rd () })
RUN (t_array, A, v[0], v[1], { n + 1, rd () })

int
main (void)
{
  if (t_lit () != 1)
    __builtin_abort ();
  if (t_brace () != 1)
    __builtin_abort ();
  if (t_array () != 1)
    __builtin_abort ();
  return 0;
}
