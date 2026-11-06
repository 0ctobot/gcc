/* A non-constant offset that may be negative and something added to it can put
   a pointer above where the offset took it, which stays below the member when
   the offset was negative enough.  That holds both for separate steps and for
   the single step that they get folded into.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

struct pkt { char tag[8]; char label[24]; unsigned w[3]; long z; };
struct pkt tbl[5];

#define bos(p) __builtin_object_size (p, 1)
#define bdos(p) __builtin_dynamic_object_size (p, 1)
#define barrier() __asm__ ("" : : : "memory")
#define check_at_least(p, n) \
  do \
    if (bos (p) < (n) || bdos (p) < (n)) \
      __builtin_abort (); \
  while (0)

__attribute__ ((noipa)) void
folded (long n)
{
  char *p = tbl[1].label + n + 7;
  check_at_least (p, 28);
}

__attribute__ ((noipa)) void
folded_scaled (long n)
{
  char *p = tbl[1].label + 4 * n + 8;
  check_at_least (p, 28);
}

__attribute__ ((noipa)) void
folded_two (long n, long m)
{
  char *p = tbl[1].label + n + m;
  check_at_least (p, 31);
}

__attribute__ ((noipa)) void
two_steps (long n)
{
  char *p = tbl[1].label;
  p += n;
  barrier ();
  p += 7;
  check_at_least (p, 28);
}

__attribute__ ((noipa)) void
two_steps_var (long n, long m)
{
  char *p = tbl[1].label;
  p += n;
  barrier ();
  p += m;
  check_at_least (p, 31);
}

/* These can't go below the member, or use a lone index as ever, so they keep
   the size of the member.  */

__attribute__ ((noipa)) void
index_only (long i)
{
  char *p = tbl[1].label + i;
  if (bos (p) != 24)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
size_t_plus_one (__SIZE_TYPE__ n)
{
  char *p = tbl[1].label + n + 1;
  if (bos (p) != 24)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
unsigned_plus_one (unsigned int n)
{
  char *p = tbl[1].label + n + 1;
  if (bos (p) != 24)
    __builtin_abort ();
}

__attribute__ ((noipa)) void
induction_plus_one (int k)
{
  for (int i = 0; i < k; i++)
    {
      char *p = tbl[1].label + i + 1;
      if (bos (p) != 24)
	__builtin_abort ();
    }
}

__attribute__ ((noipa)) void
walk_by_len (__SIZE_TYPE__ len, int k)
{
  char *p = tbl[1].label;
  for (int j = 0; j < k; j++)
    p += len;
  if (bos (p) != 24)
    __builtin_abort ();
}

int
main ()
{
  folded (-11);
  folded_scaled (-3);
  folded_two (-11, 4);
  two_steps (-11);
  two_steps_var (-11, 4);
  index_only (1);
  size_t_plus_one (1);
  unsigned_plus_one (1);
  induction_plus_one (2);
  walk_by_len (1, 2);
  return 0;
}
