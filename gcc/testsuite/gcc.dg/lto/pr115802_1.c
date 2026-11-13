static int gate;

__attribute__ ((noinline)) void
barrier (void)
{
  __asm__ __volatile__ ("" : : : "memory");
}

__attribute__ ((noinline)) void
lock (void)
{
  int expected;

  do
    expected = 0;
  while (!__atomic_compare_exchange_n (&gate, &expected, 1, 0,
				       __ATOMIC_ACQUIRE, __ATOMIC_RELAXED));
}

__attribute__ ((noinline)) void
unlock (void)
{
  int expected = 1;

  __atomic_compare_exchange_n (&gate, &expected, 0, 0, __ATOMIC_RELEASE,
			       __ATOMIC_RELAXED);
}
