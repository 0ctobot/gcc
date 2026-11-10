/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-profile_estimate" } */

/* A __builtin_expect () hint on a PHI node of constants, as in
   unlikely (a || b), also describes the branches that pick the constants.  */

#define unlikely(x) __builtin_expect (!!(x), 0)

struct s { int a, b; };
struct ti { unsigned long preempt_count; };
extern struct ti *cur;
void warn (void);
void fast (void);
void sched (void);

/* WARN_ON (a || b) followed by an error return on the same value.  */

int
f1 (struct s *p)
{
  int ret = !!(p->a < 0 || p->b > 7);
  if (unlikely (ret))
    warn ();
  if (unlikely (ret))
    return -1;
  fast ();
  return 0;
}

/* An inlined helper returning a || b that is tested with unlikely (), as
   in the arm64 preempt_enable ().  */

static inline __attribute__ ((always_inline)) _Bool
dec_and_test (void)
{
  struct ti *ti = cur;
  unsigned long pc = *(volatile unsigned long *) &ti->preempt_count;
  *(volatile unsigned long *) &ti->preempt_count = --pc;
  return !pc || !*(volatile unsigned long *) &ti->preempt_count;
}

void
f2 (struct s *p)
{
  p->a++;
  if (unlikely (dec_and_test ()))
    sched ();
  if (p->b & 0x700000)
    {
      p->b++;
      if (unlikely (dec_and_test ()))
	sched ();
    }
}

/* { dg-final { scan-tree-dump-times "__builtin_expect heuristics of edge" 6 "profile_estimate" } } */
/* { dg-final { scan-tree-dump-times "Lowered guessed probability" 4 "profile_estimate" } } */
