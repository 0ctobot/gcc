/* { dg-do run } */
/* { dg-options "-O2" } */

/* A PHI of two addresses at the same offset in one object must keep each
   member's subobject size, in either argument order.  CCP used to give the
   PHI the value of its first union member and FRE the value of its first
   argument for a struct and its first member.  */

typedef __SIZE_TYPE__ size_t;

union addr { char v4[16]; char v6[46]; };
struct rec { char name[8]; char rest[56]; };
union addr g;
struct rec r;

#define TEST(fn, init1, init2)					\
  __attribute__ ((noipa)) size_t				\
  fn (int sel, size_t *smax)					\
  {								\
    char *p;							\
    if (!sel)							\
      p = (char *) (init1);					\
    else							\
      p = (char *) (init2);					\
    *smax = __builtin_object_size (p, 1);			\
    return __builtin_dynamic_object_size (p, 1);		\
  }

TEST (union_v4_first, g.v4, g.v6)
TEST (union_v6_first, g.v6, g.v4)
TEST (struct_name_first, r.name, &r)
TEST (struct_whole_first, &r, r.name)

#define CHECK(fn, s0, s1, smax_expect)				\
  do								\
    {								\
      size_t smax;						\
      if (fn (0, &smax) != (s0) || smax != (smax_expect))	\
	__builtin_abort ();					\
      if (fn (1, &smax) != (s1) || smax != (smax_expect))	\
	__builtin_abort ();					\
    }								\
  while (0)

int
main (void)
{
  CHECK (union_v4_first, 16, 46, 46);
  CHECK (union_v6_first, 46, 16, 46);
  CHECK (struct_name_first, 8, sizeof (r), sizeof (r));
  CHECK (struct_whole_first, sizeof (r), 8, sizeof (r));
  return 0;
}
