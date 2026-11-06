/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=pattern -fdump-tree-gimple" } */

/* A jump back makes this initializer run again, and it gets built in a
   temporary that then gets copied into the variable.  Building it in the
   variable would have zeroed the padding under automatic variable
   initialization, which the pattern mode does as well, so the padding of the
   variable has to be cleared again after the copy.  Nothing else clears it,
   since the initializer covers every member and v is too large for a
   register.  */

struct B { char c; long l[3]; };

extern char g (void);
extern void use (void *);

void
plain (long n)
{
back:;
  struct B v = { .c = g (), .l = { n, n, n } };
  use (&v);
  if (n--)
    goto back;
}

/* { dg-final { scan-tree-dump-times "__builtin_clear_padding \\(&v" 1 "gimple" } } */
