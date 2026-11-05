/* A literal gimplified twice must not get a second initialization, which
   would be a spurious store for a volatile one.  */
/* { dg-do compile } */
/* { dg-options "-std=gnu23 -O0 -ftrivial-auto-var-init=zero -fdump-tree-gimple" } */

extern int f (void);

struct P
{
  int k;
  int *buf __attribute__ ((counted_by (k)));
};

int
add_call (void)
{
  return (int) { f () } += 1;
}

int
add_vol (void)
{
  return (volatile int) { 5 } += 1;
}

bool
dec_bool (void)
{
  return --(bool) { f () };
}

int *
counted (int *q)
{
  int *p = (struct P) { 3, q }.buf;
  return p;
}

/* { dg-final { scan-tree-dump-not "DEFERRED_INIT" "gimple" } } */
