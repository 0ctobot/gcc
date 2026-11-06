/* { dg-do run } */

/* A narrow use of a pseudo that has a memory equivalence turns into a narrow
   load from the equivalent address.  The stack arguments below give us such an
   equivalence, and its index is extended and scaled for the wider access.  That
   scale isn't valid for the narrow load on aarch64, so LRA reloads the whole
   address into a register.  It used to emit the address's MULT as a move
   without rewriting it as a shift, which isn't a recognizable insn and caused
   an ICE at -O1.  */

#define TEST(NAME, IDX, ELT, NARROW)					\
  IDX h_##NAME;								\
  int m_##NAME = 1;							\
  NARROW j_##NAME;							\
									\
  static inline void							\
  k_##NAME (int p, ELT *q)						\
  {									\
    int *n = __builtin_malloc (sizeof (int));				\
    n[0] = 0;								\
    for (; 0 <= p; m_##NAME++, p--)					\
      {									\
	if (m_##NAME)							\
	  j_##NAME = q[h_##NAME++];					\
	if (j_##NAME)							\
	  m_##NAME = n[i];						\
      }									\
  }									\
									\
  __attribute__ ((noipa)) void						\
  o_##NAME (int a, int b, int c, int d, int e, int f, int g, int l, ELT r) \
  {									\
    k_##NAME (a, &r);							\
  }									\
									\
  static void								\
  check_##NAME (void)							\
  {									\
    o_##NAME (0, 0, 0, 0, 0, 0, 0, 0, (ELT) 0x1234567a);		\
    if (j_##NAME != (NARROW) 0x1234567a					\
	|| h_##NAME != 1 || m_##NAME != 1)				\
      __builtin_abort ();						\
  }

int i;

TEST (a, int, int, char)
TEST (b, unsigned int, int, char)
TEST (c, int, int, short)
TEST (d, unsigned int, int, short)
TEST (e, int, long long, int)
TEST (f, unsigned int, long long, char)

/* Two narrow uses of the same equivalence, each of which needs the address
   reloaded on its own.  */

int h_s, m_s = 1;
char j_s;
short t_s;

static inline void
k_s (int p, int *q)
{
  int *n = __builtin_malloc (sizeof (int));
  n[0] = 0;
  for (; 0 <= p; m_s++, p--)
    {
      if (m_s)
	{
	  int v = q[h_s++];
	  j_s = v;
	  t_s = v;
	}
      if (j_s + t_s)
	m_s = n[i];
    }
}

__attribute__ ((noipa)) void
o_s (int a, int b, int c, int d, int e, int f, int g, int l, int r)
{
  k_s (a, &r);
}

int
main (void)
{
  check_a ();
  check_b ();
  check_c ();
  check_d ();
  check_e ();
  check_f ();

  o_s (0, 0, 0, 0, 0, 0, 0, 0, 0x1234567a);
  if (j_s != (char) 0x1234567a || t_s != (short) 0x1234567a
      || h_s != 1 || m_s != 1)
    __builtin_abort ();

  return 0;
}
