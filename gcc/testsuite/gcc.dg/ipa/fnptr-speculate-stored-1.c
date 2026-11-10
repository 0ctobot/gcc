/* A function pointer field that is only ever written with the address of the
   same function, by assignments, static initializers of a record, an array
   of records and a nested record, a null pointer and copies of the whole
   record, is still speculated to that function.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-ipa-devirt" } */

typedef int (*fn_t) (int);
volatile int sink;

struct ops { fn_t f; fn_t g; };
struct outer { int k; struct ops o; };

int fa (int x) { sink = x; return 1; }

const struct ops tbl = { fa, 0 };
const struct ops arr[2] = { { fa, 0 }, { fa, 0 } };
const struct outer nest = { 1, { fa, 0 } };

void set (struct ops *p) { p->f = fa; }
void clear (struct ops *p) { p->f = 0; }
void copy (struct ops *p, const struct ops *q) { *p = *q; }

int __attribute__ ((noinline))
call (const struct ops *o, int x)
{
  return o->f (x);
}

/* { dg-final { scan-ipa-dump-times "-> speculative call" 1 "devirt" } } */
/* { dg-final { scan-ipa-dump "-> speculative call call/\[0-9\]+ => fa/" "devirt" } } */
