/* A volatile store of zeros after a zero-init marker must stay, while the
   same store to a non-volatile member is deleted.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-zinit1-details -fdump-tree-optimized" } */

struct k { long a[4]; };
struct v { volatile long a[4]; };
extern int sink (void *, unsigned long);

int
f (long x)
{
  struct k s;
  s.a[3] = 0;
  s.a[1] = x;
  return sink (&s, sizeof s);
}

int
g (long x)
{
  struct v s;
  s.a[3] = 0;
  s.a[1] = x;
  return sink (&s, sizeof s);
}

/* { dg-final { scan-tree-dump-times "Deleted redundant store" 1 "zinit1" } } */
/* { dg-final { scan-tree-dump-times "s.a\\\[3\\\] ={v} 0" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-not "s.a\\\[3\\\] = 0" "optimized" } } */
