/* PR c/128184 */
/* { dg-do run { target { bitint && sync_int_long } } } */
/* { dg-options "-std=c23 -pedantic-errors -Wno-sync-nand" } */

#define NAND_FNS(T, N) \
[[gnu::noipa]] T							\
sync_fetch_nand_##N (T *p, T q)						\
{									\
  return __sync_fetch_and_nand (p, q);					\
}									\
									\
[[gnu::noipa]] T							\
sync_nand_fetch_##N (T *p, T q)						\
{									\
  return __sync_nand_and_fetch (p, q);					\
}									\
									\
NAND_ATOMIC_FNS (T, N)

#define NAND_ATOMIC_FNS(T, N) \
[[gnu::noipa]] T							\
atomic_fetch_nand_##N (T *p, T q)					\
{									\
  return __atomic_fetch_nand (p, q, __ATOMIC_RELAXED);			\
}									\
									\
[[gnu::noipa]] T							\
atomic_nand_fetch_##N (T *p, T q)					\
{									\
  return __atomic_nand_fetch (p, q, __ATOMIC_SEQ_CST);			\
}

/* Check that FN stores ~(A & B) and returns the old value when OLD_P is
   nonzero or the new value otherwise.  */
#define CHECK(T, FN, A, B, R, OLD_P) \
  do {									\
    T v = (A);								\
    T r = FN (&v, (B));							\
    if (v != (R) || r != ((OLD_P) ? (A) : (R)))				\
      __builtin_abort ();						\
  } while (0)

#define CHECK_SYNC(T, N, A, B, R) \
  do {									\
    CHECK (T, sync_fetch_nand_##N, A, B, R, 1);				\
    CHECK (T, sync_nand_fetch_##N, A, B, R, 0);				\
  } while (0)

#define CHECK_ATOMIC(T, N, A, B, R) \
  do {									\
    CHECK (T, atomic_fetch_nand_##N, A, B, R, 1);			\
    CHECK (T, atomic_nand_fetch_##N, A, B, R, 0);			\
  } while (0)

NAND_FNS (_BitInt(17), s17)
NAND_FNS (unsigned _BitInt(17), u17)
#if __BITINT_MAXWIDTH__ >= 129
NAND_ATOMIC_FNS (unsigned _BitInt(129), u129)
#endif
#if __BITINT_MAXWIDTH__ >= 255
NAND_ATOMIC_FNS (_BitInt(255), s255)
#endif

int
main ()
{
  CHECK_SYNC (_BitInt(17), s17, 0wb, 5wb, -1wb);
  CHECK_SYNC (_BitInt(17), s17, 0x5555wb, 0x3333wb, -0x1112wb);
  CHECK_ATOMIC (_BitInt(17), s17, 0wb, 5wb, -1wb);
  CHECK_ATOMIC (_BitInt(17), s17, 0x5555wb, 0x3333wb, -0x1112wb);
  CHECK_SYNC (unsigned _BitInt(17), u17, 0x1a5a5uwb, 0x1f0f0uwb, 0x5f5fuwb);
  CHECK_ATOMIC (unsigned _BitInt(17), u17, 0x1a5a5uwb, 0x1f0f0uwb,
		0x5f5fuwb);
#if __BITINT_MAXWIDTH__ >= 129
  CHECK_ATOMIC (unsigned _BitInt(129), u129, 0uwb, 5uwb,
		0x1ffffffffffffffffffffffffffffffffuwb);
  CHECK_ATOMIC (unsigned _BitInt(129), u129,
		0x1a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5uwb,
		0xf0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0uwb,
		0x15f5f5f5f5f5f5f5f5f5f5f5f5f5f5f5fuwb);
#endif
#if __BITINT_MAXWIDTH__ >= 255
  CHECK_ATOMIC (_BitInt(255), s255,
		0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdewb,
		-0xfedcba9876543210fedcba9876543210fedcba9876543210fedcba98765432wb,
		-0x10101450108a8cce10101450108a8cce10101450108a8cce10101450108a8cfwb);
#endif
}
