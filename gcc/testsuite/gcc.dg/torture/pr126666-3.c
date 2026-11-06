/* { dg-do run } */

/* Like pr126666.c, but with wider vectors and VEC_COND_EXPR arms that targets
   can't turn into a plain AND with the mask.  Each test ends by running the
   next one, since leaving the loop in f normally would change the block layout
   that this depends on.  */

#define TEST(NAME, ELT, SCALAR, SIZE, NEXT)				\
  typedef signed ELT NAME##_vs __attribute__ ((vector_size (SIZE)));	\
  typedef unsigned ELT NAME##_vu __attribute__ ((vector_size (SIZE)));	\
									\
  static NAME##_vu NAME##_buf = { 0, 3 };				\
  static int NAME##_count;						\
									\
  __attribute__ ((noipa)) int						\
  NAME##_tick (void)							\
  {									\
    return ++NAME##_count;						\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_check (void)							\
  {									\
    for (unsigned i = 0; i < SIZE / sizeof (ELT); i++)			\
      if (NAME##_buf[i] != 1)						\
	__builtin_abort ();						\
    NEXT ();								\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_f (NAME##_vu *p)						\
  {									\
    NAME##_vs c;							\
    goto head;								\
  side:									\
    NAME##_check ();							\
  body:									\
    {									\
      SCALAR s = (SCALAR) c;						\
      s = ~s;								\
      NAME##_vu w = (NAME##_vu) s;					\
      *p = ~w;								\
    }									\
  head:									\
    c = ((*p != (NAME##_vu) {}) & 3) ^ 2;				\
    if (NAME##_tick () + c[0] == 4)					\
      goto side;							\
    goto body;								\
  }									\
									\
  __attribute__ ((noipa)) void						\
  NAME##_run (void)							\
  {									\
    NAME##_f (&NAME##_buf);						\
  }

__attribute__ ((noipa)) void
done (void)
{
  __builtin_exit (0);
}

TEST (i2, int, __UINT64_TYPE__, 8, done)
TEST (s4, short, __UINT64_TYPE__, 8, i2_run)
TEST (c8, char, __UINT64_TYPE__, 8, s4_run)
TEST (s2, short, __UINT32_TYPE__, 4, c8_run)
TEST (c4, char, __UINT32_TYPE__, 4, s2_run)
TEST (c2, char, __UINT16_TYPE__, 2, c4_run)

int
main (void)
{
  c2_run ();
}
