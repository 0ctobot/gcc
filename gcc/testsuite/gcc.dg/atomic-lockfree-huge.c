/* { dg-do compile { target { lp64 || llp64 } } } */
/* { dg-options "" } */

/* No object this large can be lock-free.  The bit counts of these sizes
   overflow 64 bits and used to wrap around to the size of a lock-free
   integer mode.  */

#define BIG(n) __atomic_always_lock_free ((__SIZE_TYPE__) (n), 0)

_Static_assert (!BIG ((1ULL << 60) + 1), "");
_Static_assert (!BIG ((1ULL << 61) + 1), "");
_Static_assert (!BIG ((1ULL << 61) + 2), "");
_Static_assert (!BIG ((1ULL << 61) + 4), "");
_Static_assert (!BIG ((1ULL << 61) + 8), "");
_Static_assert (!BIG ((1ULL << 62) + 1), "");
_Static_assert (!BIG ((3ULL << 61) + 2), "");
_Static_assert (!BIG ((3ULL << 61) + 8), "");
