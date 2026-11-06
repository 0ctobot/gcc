/* { dg-do compile } */
/* { dg-options "-O2 -std=gnu23 -fdump-tree-gimple" } */

/* A jump back makes these initializers run again, and each one gets built in
   a temporary that then gets copied into the variable.  The padding of the
   variable only gets cleared again after the copy where building the
   initializer in the variable would have zeroed it.  Without automatic
   variable initialization, that takes a clear of the whole variable ahead of
   the members.  The empty initializer of w.p asks for one, and so does s at
   -O2 since it has padding and fits in a register.  The arms of the
   conditional that initializes c each leave a member without an initializer,
   which clears the object that each arm gets built in.  The initializer of b
   covers every member and b is too large for a register, so b gets none.  */

struct B { char c; long l[3]; };
struct P { char c; long l; };
struct O { struct P p; long n; };

extern char g (void);
extern void use (void *);

void
big (long n)
{
back:;
  struct B b = { .c = g (), .l = { n, n, n } };
  use (&b);
  if (n--)
    goto back;
}

void
small (long n)
{
back:;
  struct P s = { .c = g (), .l = n };
  use (&s);
  if (n--)
    goto back;
}

void
cond (long n)
{
back:;
  struct B c = n ? (struct B) { .c = g () } : (struct B) { .l = { n } };
  use (&c);
  if (n--)
    goto back;
}

void
empty (long n)
{
back:;
  struct O w = { .p = {}, .n = g () };
  use (&w);
  if (n--)
    goto back;
}

/* { dg-final { scan-tree-dump-not "__builtin_clear_padding \\(&b" "gimple" } } */
/* { dg-final { scan-tree-dump-times "__builtin_clear_padding \\(&s" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "__builtin_clear_padding \\(&c" 1 "gimple" } } */
/* { dg-final { scan-tree-dump-times "__builtin_clear_padding \\(&w" 1 "gimple" } } */
