/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-sra-details" } */

/* A reference only guarantees that the object it's bound to is readable, so
   IPA-SRA must not move an access past the end of that object into the callers,
   neither from the function taking the reference nor from a function it passes
   the reference to.  */

struct S
{
  int a[4];
};

struct Base
{
  int kind;
};

struct Derived : Base
{
  int pad[40];
  int y;
};

int sink;

static __attribute__ ((noinline)) void
direct (const S &r, int c)
{
  if (__builtin_expect (c, 1))
    sink = r.a[1 << 28];
  sink += r.a[0];
}

static __attribute__ ((noinline)) int
callee (Derived *p)
{
  return p->y + p->kind;
}

static __attribute__ ((noinline)) void
import (const Base &r, int c)
{
  if (__builtin_expect (c, 1))
    sink = callee ((Derived *) &r);
}

void
test_direct (const S *p, int c)
{
  direct (*p, c);
}

void
test_import (const Base *p, int c)
{
  import (*p, c);
}

/* { dg-final { scan-ipa-dump "Encountered an access past the end of the object" "sra" } } */
/* { dg-final { scan-ipa-dump "callee access outsize size boundary" "sra" } } */
