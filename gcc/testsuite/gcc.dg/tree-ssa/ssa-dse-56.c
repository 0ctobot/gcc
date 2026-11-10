/* A zero initialization of new is dead in each function below except the
   t_join ones, but the walk over its uses forks into more than one path.
   DSE has to track the bytes that are still live on each path to see
   that.  The loops are reduced from the Linux kernel's __slab_free.  */
/* { dg-do compile } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-dse1-details" } */

struct slabx {
  unsigned long flags;
  void *cache;
  void *list[2];
  void *freelist;
  union {
    unsigned long counters;
    struct { unsigned inuse:16; unsigned objects:15; unsigned frozen:1; };
  };
  unsigned int units;
  int refcount;
  unsigned long pad;
};

extern void lock_it (void *);
extern void unlock_it (void *);

static inline __attribute__((always_inline)) int
cas2 (unsigned long *ptr, unsigned long o0, unsigned long o1,
      unsigned long n0, unsigned long n1)
{
  if (ptr[0] != o0 || ptr[1] != o1)
    return 0;
  ptr[0] = n0;
  ptr[1] = n1;
  return 1;
}

#define SLAB_LOOP_BODY							\
  do {									\
    prior = slab->freelist;						\
    counters = slab->counters;						\
    new.counters = counters;						\
    was_frozen = new.frozen;						\
    new.inuse -= cnt;							\
    if ((!new.inuse || !prior) && !was_frozen)				\
      {									\
	lock_it (slab);							\
	locked = 1;							\
      }									\
  } while (!cas2 ((unsigned long *) &slab->freelist,			\
		  (unsigned long) prior, counters,			\
		  (unsigned long) head, new.counters));			\
  return locked + was_frozen + new.inuse;

int
t_loop_di (struct slabx *slab, void *head, int cnt)
{
  struct slabx new;
  void *prior;
  unsigned long counters;
  int was_frozen, locked = 0;
  SLAB_LOOP_BODY
}

int
t_loop_brace (struct slabx *slab, void *head, int cnt)
{
  struct slabx new = {};
  void *prior;
  unsigned long counters;
  int was_frozen, locked = 0;
  SLAB_LOOP_BODY
}

int
t_loop_memset (struct slabx *slab, void *head, int cnt)
{
  struct slabx new;
  void *prior;
  unsigned long counters;
  int was_frozen, locked = 0;
  __builtin_memset (&new, 0, sizeof new);
  SLAB_LOOP_BODY
}

int
t_fork (struct slabx *slab, int c)
{
  struct slabx new;
  new.counters = slab->counters;
  new.inuse -= 1;
  if (c)
    {
      lock_it (slab);
      slab->counters = new.counters;
    }
  else
    {
      unlock_it (slab);
      slab->pad = new.counters;
    }
  return new.frozen;
}

/* new.pad is read on one path, but SRA gives it a scalar of its own, so
   the initialization of the whole of new is still dead.  */

int
t_fork_neg (struct slabx *slab, int c)
{
  struct slabx new;
  new.counters = slab->counters;
  new.inuse -= 1;
  if (c)
    {
      lock_it (slab);
      slab->counters = new.counters;
    }
  else
    {
      unlock_it (slab);
      slab->pad = new.counters + new.pad;
    }
  return new.frozen;
}

/* new.counters is stored on one arm only and read after the join, so the
   initialization has to stay.  */

unsigned long
t_join_a (struct slabx *slab, int c)
{
  struct slabx new;
  if (c)
    {
      lock_it (slab);
      new.counters = slab->pad;
    }
  else
    unlock_it (slab);
  lock_it (slab);
  return new.counters + new.frozen;
}

unsigned long
t_join_b (struct slabx *slab, int c)
{
  struct slabx new;
  if (c)
    unlock_it (slab);
  else
    {
      lock_it (slab);
      new.counters = slab->pad;
    }
  lock_it (slab);
  return new.counters + new.frozen;
}

unsigned long
t_union (struct slabx *slab, int c)
{
  struct slabx new = {};
  new.counters = slab->counters;
  if (c)
    {
      lock_it (slab);
      new.units = 1;
      new.refcount = 2;
      new.pad = 3;
      return new.frozen + new.flags + new.units + new.pad;
    }
  unlock_it (slab);
  new.units = 4;
  new.refcount = 5;
  new.pad = 6;
  return new.frozen + (unsigned long) new.cache + new.refcount;
}

/* { dg-final { scan-tree-dump-times "Deleted dead store: new = \\.DEFERRED_INIT" 4 "dse1" } } */
/* { dg-final { scan-tree-dump-times "Deleted dead store: new = {}" 2 "dse1" } } */
/* { dg-final { scan-tree-dump-times "Deleted dead call: __builtin_memset" 1 "dse1" } } */
/* { dg-final { scan-tree-dump-times "\n  new = \\.DEFERRED_INIT" 2 "dse1" } } */
