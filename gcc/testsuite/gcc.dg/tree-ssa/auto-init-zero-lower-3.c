/* A pattern-init marker doesn't store zeros, so the memset after it must
   stay.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=pattern -fdump-tree-zinit1-details" } */

struct k { __INT64_TYPE__ a[7]; __INT64_TYPE__ temp; };
extern int sink (void *, unsigned long);

int
f (__INT64_TYPE__ v)
{
  struct k s;
  __builtin_memset (&s, 0, __builtin_offsetof (struct k, temp));
  s.a[1] = v;
  return sink (&s, sizeof s);
}

/* { dg-final { scan-tree-dump-not "Deleted redundant" "zinit1" } } */
/* { dg-final { scan-tree-dump-times "\\.DEFERRED_INIT \\(64, 1," 1 "zinit1" } } */
/* { dg-final { scan-tree-dump-times "__builtin_memset \\(&s, 0, 56\\)" 1 "zinit1" } } */
