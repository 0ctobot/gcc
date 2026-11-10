/* With -ftrivial-auto-var-init=zero, a store of zeros right after the
   zero-init marker of the same variable is redundant and is deleted once
   the late uninitialized warnings have run.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-zinit1-details -fdump-tree-optimized" } */

struct k { long a[7]; long temp; };
extern int sink (void *, unsigned long);

/* A partial memset like the one in the kernel's tcp_call_bpf.  */

int
f1 (long v)
{
  struct k s;
  __builtin_memset (&s, 0, __builtin_offsetof (struct k, temp));
  s.a[1] = v;
  return sink (&s, sizeof s);
}

int
f2 (long v)
{
  struct k s;
  s.a[3] = 0;
  s.a[1] = v;
  return sink (&s, sizeof s);
}

/* { dg-final { scan-tree-dump-times "Deleted redundant call: __builtin_memset" 1 "zinit1" } } */
/* { dg-final { scan-tree-dump-times "Deleted redundant store: s.a\\\[3\\\] = 0" 1 "zinit1" } } */
/* { dg-final { scan-tree-dump-not "__builtin_memset" "optimized" } } */
/* { dg-final { scan-tree-dump-not "s.a\\\[3\\\] = 0" "optimized" } } */
/* { dg-final { scan-tree-dump-times "\\.DEFERRED_INIT" 2 "optimized" } } */
