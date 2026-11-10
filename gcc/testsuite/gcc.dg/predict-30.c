/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-ethread-details -fdump-tree-threadfull1-details -fdump-tree-profile_estimate" } */

/* Early jump threading runs before the profile is estimated and would
   thread every edge into a branch on a hinted PHI node of constants,
   removing the PHI node before profile estimation can pass its hint to the
   branches that pick the constants.  The branch is left to the jump
   threader that runs after profile estimation instead.  */

#define likely(x) __builtin_expect (!!(x), 1)
#define unlikely(x) __builtin_expect (!!(x), 0)

struct s { int a, b; };
void warn (void);
void fast (void);

/* WARN_ON (a || b).  */

void
f1 (struct s *p)
{
  int c = p->a < 0 || p->b > 7;
  if (unlikely (c))
    warn ();
  fast ();
}

/* static_branch_likely (), which tests likely () on the negated result of
   an asm goto.  */

static inline __attribute__ ((always_inline)) _Bool
key_false (void)
{
  asm goto ("" : : : : l_yes);
  return 0;
l_yes:
  return 1;
}

void
f2 (void)
{
  if (likely (!key_false ()))
    fast ();
  else
    warn ();
}

/* { dg-final { scan-tree-dump-not "Registering jump thread" "ethread" } } */
/* { dg-final { scan-tree-dump-times "__builtin_expect heuristics of edge" 4 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-times "Lowered guessed probability" 1 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-times "Registering jump thread" 4 "threadfull1" } } */
