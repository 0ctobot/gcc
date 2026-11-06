/* { dg-do run } */
/* { dg-require-effective-target indirect_jumps } */

/* A goto back to a label in the block of a variable doesn't end its lifetime,
   so a call in a second run of its initializer can read the old value through
   a pointer.  This only depends on the address of the variable being
   taken.  */

struct S { long a, b, c, d, e, f, g, h; };
struct T { struct S s; long t; };

struct S *volatile gp;

__attribute__ ((noipa)) long
rd (void)
{
  struct S *p = gp;
  return p ? p->a : -1;
}

__attribute__ ((noipa)) int
yes (void)
{
  return 1;
}

/* Both of these read the old value after writing to their result, which is the
   variable itself if the variable is used as the return slot.  */

__attribute__ ((noipa)) struct S
mk (long v)
{
  struct S r = { .a = v, .b = v, .c = v, .d = v, .e = v, .f = v, .g = v, .h = v };
  __asm__ ("" : "+m" (r));
  r.b = rd ();
  return r;
}

__attribute__ ((noipa)) struct S
mknrv (struct S *p, long v)
{
  struct S r;
  r.a = v;
  r.b = p ? p->a : -1;
  r.c = r.d = r.e = r.f = r.g = r.h = v;
  return r;
}

#define RUN(name, get, ...)						\
  __attribute__ ((noipa)) long						\
  name (void)								\
  {									\
    int n = 0;								\
    gp = 0;								\
  again:;								\
    __VA_ARGS__;							\
    gp = (struct S *) &v;						\
    if (n++ < 1)							\
      goto again;							\
    return get;								\
  }

/* The value of a call or a compound literal is complete before the variable
   gets it.  */
RUN (t_call, v.b, struct S v = mk (n + 1))
RUN (t_nrv, v.b, struct S v = mknrv (gp, n + 1))
RUN (t_lit, v.b, struct S v = (struct S) { .a = n + 1, .b = rd () })
RUN (t_lit_member, v.s.b + v.t * 10,
     struct T v = { .s = (struct S) { .a = n + 1, .b = rd () }, .t = rd () })
RUN (t_cond, v.b,
     struct S v = (yes () ? (struct S) { .a = n + 1, .b = rd () }
		   : (struct S) { .a = 0 }))
RUN (t_comma, v.b, struct S v = (yes (), (struct S) { .a = n + 1, .b = rd () }))

/* C doesn't sequence these stores against the call, but building the value in
   a temporary is the same rule for all side-effecting aggregates.  */
RUN (t_brace, v.b, struct S v = { .a = n + 1, .b = rd () })
RUN (t_brace_stmt, v.b, struct S v = { .a = n + 1, .b = ({ long r = rd (); r; }) })
RUN (t_array, v[1], long v[8] = { n + 1, rd () })

int
main (void)
{
  if (t_call () != 1
      || t_nrv () != 1
      || t_lit () != 1
      || t_lit_member () != 11
      || t_cond () != 1
      || t_comma () != 1
      || t_brace () != 1
      || t_brace_stmt () != 1
      || t_array () != 1)
    __builtin_abort ();
  return 0;
}
