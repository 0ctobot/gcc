/* DSE tracks the bytes of a zero initialization that are still live on
   each path after the walk over its uses forks.  Each function here reads
   a member of new that's only zeroed by the initialization, on one path
   or after the paths join, so the initialization has to stay.  dirty()
   leaves a nonzero pattern on the stack to catch it if it doesn't.  */
/* { dg-do run } */
/* { dg-options "-ftrivial-auto-var-init=zero" } */

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

__attribute__((noipa)) void
lock_it (void *p)
{
  ((struct slabx *) p)->units++;
}

__attribute__((noipa)) void
unlock_it (void *p)
{
  ((struct slabx *) p)->refcount++;
}

__attribute__((noipa)) unsigned long
t_fork_neg (struct slabx *slab, int c)
{
  struct slabx new;
  new.counters = slab->counters;
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

__attribute__((noipa)) unsigned long
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

__attribute__((noipa)) unsigned long
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

__attribute__((noipa)) void
dirty (void)
{
  volatile unsigned char buf[1024];
  for (int i = 0; i < 1024; i++)
    buf[i] = 0xa5;
}

static struct slabx s;

static void
reset (void)
{
  __builtin_memset (&s, 0, sizeof s);
  s.counters = 0x30004;
  s.pad = 0x76;
}

int
main (void)
{
  reset (); dirty ();
  if (t_fork_neg (&s, 1) != 0 || s.counters != 0x30004 || s.pad != 0x76)
    __builtin_abort ();
  reset (); dirty ();
  if (t_fork_neg (&s, 0) != 0 || s.pad != 0x30004)
    __builtin_abort ();
  reset (); dirty ();
  if (t_join_a (&s, 1) != 0x76)
    __builtin_abort ();
  reset (); dirty ();
  if (t_join_a (&s, 0) != 0)
    __builtin_abort ();
  reset (); dirty ();
  if (t_join_b (&s, 1) != 0)
    __builtin_abort ();
  reset (); dirty ();
  if (t_join_b (&s, 0) != 0x76)
    __builtin_abort ();
  return 0;
}
