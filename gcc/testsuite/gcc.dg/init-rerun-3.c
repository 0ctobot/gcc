/* { dg-do compile } */
/* { dg-options "-O2 -fexceptions -Wframe-larger-than=3000" } */

/* The initializer of a variable only runs again within the variable's lifetime
   if a jump can come back to a label above the declaration, so nothing else may
   cost the variable a temporary as large as itself and with it twice the stack
   space.  A warning from -Wframe-larger-than is a failure here.  */

struct B { char a[2000]; };

extern long g (long);
extern void use (void *);
extern struct B mk (long);

/* A forward goto to a label above the declaration.  */

void
forward_braces (int n)
{
  if (n)
    goto skip;
  g (0);
skip:;
  struct B v = { .a[7] = g (n) };
  use (&v);
}

void
forward_comma (int n)
{
  if (n)
    goto skip;
  g (0);
skip:;
  struct B v = (g (3), (struct B) { .a[7] = g (n) });
  use (&v);
}

void
forward_conditional (int n)
{
  if (n)
    goto skip;
  g (0);
skip:;
  struct B v = n ? (struct B) { .a[7] = g (n) } : (struct B) { .a[8] = g (2) };
  use (&v);
}

/* A label with its address taken is no target without a computed goto.  */

void
addr_taken (int n)
{
  long here = ({ __label__ l; l: (long) &&l; });
  struct B v = { .a[7] = g (here) };
  use (&v);
}

/* A cleanup can't read a variable that never had its address taken.  */

__attribute__ ((noipa)) static void
cl (int *p)
{
  use (p);
}

void
cleanup_no_addr (int n)
{
  __attribute__ ((cleanup (cl))) int c = 0;
back:;
  struct B d = mk (n);
  if (n-- > 0)
    goto back;
  g (d.a[0]);
}
