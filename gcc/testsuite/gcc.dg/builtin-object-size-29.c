/* A pointer into a member is bounded by that member no matter how the pointer
   to the enclosing object was computed.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

struct s { char a[8]; char name[24]; unsigned w[3]; long z; };
struct s tbl[5], heap[5];
volatile long v1 = 1;

#define CHECK(p, left)							\
  do									\
    {									\
      if (__builtin_object_size (p, 1) != (left)			\
	  || __builtin_dynamic_object_size (p, 1) != (left))		\
	__builtin_abort ();						\
    }									\
  while (0)

__attribute__ ((noinline)) void
index_global (long i)
{
  struct s *e = tbl + i;
  char *p = e->name + 5;
  CHECK (p, 19);
}

__attribute__ ((noinline)) void
index_param (struct s *b, long i)
{
  struct s *e = b + i;
  char *p = e->name + 5;
  CHECK (p, 19);
}

__attribute__ ((noinline)) void
index_other_member (struct s *b, long i)
{
  struct s *e = b + i;
  unsigned *p = e->w + 1;
  CHECK (p, 8);
}

__attribute__ ((noinline)) void
walk_back_global (long n)
{
  struct s *e = tbl + 3;
  while (n--)
    e--;
  char *p = e->name + 5;
  CHECK (p, 19);
}

__attribute__ ((noinline)) void
walk_back_param (struct s *b, long n)
{
  struct s *e = b + 3;
  while (n--)
    e--;
  char *p = e->name + 5;
  CHECK (p, 19);
}

__attribute__ ((noinline)) void
from_header (char *buf, long n)
{
  char *pos = buf + n;
  struct s *e = (struct s *) (pos - sizeof (struct s));
  char *p = e->name + 5;
  CHECK (p, 19);
}

int
main (void)
{
  index_global (v1);
  index_param (heap, v1);
  index_other_member (heap, v1);
  walk_back_global (v1);
  walk_back_param (heap, v1);
  from_header ((char *) heap, sizeof (struct s) * v1);
  return 0;
}
