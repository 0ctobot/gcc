/* Define NAME to read a union member of type TO from a constant stored through
   a member of type FROM, by value and by reference, and abort unless the result
   holds the bytes of that constant.  Each use gets its own readers, since
   conflicting values at one offset would stop IPA-CP from propagating them.  */

#define TEST(NAME, FROM, TO, ...)					\
  union NAME								\
  {									\
    FROM from;								\
    TO to;								\
    unsigned char b[sizeof (TO)];					\
  };									\
									\
  static volatile union NAME NAME##_expected = { .from = __VA_ARGS__ };	\
									\
  static TO __attribute__ ((noinline))					\
  NAME##_val (union NAME u)						\
  {									\
    return u.to;							\
  }									\
									\
  static TO __attribute__ ((noinline))					\
  NAME##_ref (const union NAME *u)					\
  {									\
    return u->to;							\
  }									\
									\
  static void								\
  NAME##_check (TO x)							\
  {									\
    unsigned char *p = (unsigned char *) &x;				\
    for (unsigned int i = 0; i < sizeof (TO); i++)			\
      if (p[i] != NAME##_expected.b[i])					\
	__builtin_abort ();						\
  }									\
									\
  static void								\
  NAME (void)								\
  {									\
    union NAME u1 = { .from = __VA_ARGS__ };				\
    union NAME u2 = { .from = __VA_ARGS__ };				\
    NAME##_check (NAME##_val (u1));					\
    NAME##_check (NAME##_ref (&u2));					\
  }
