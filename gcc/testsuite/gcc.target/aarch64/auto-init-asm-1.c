/* The Linux kernel's atomics and per-cpu offset reads are asm statements
   with a "Q" memory operand.  Such an asm between a zero initialization of
   a local whose address is never taken and the stores that overwrite it
   doesn't keep the initialization alive, except in k_q, where the asm
   reads part of the local.  A hard register variable gets no automatic
   initialization, but in k_hreg, the store to it that the asm reads
   through an input operand has to stay.  */
/* { dg-do compile } */
/* { dg-options "-O2 -ftrivial-auto-var-init=zero -fdump-tree-optimized" } */

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

struct big { long a[8]; };

static inline __attribute__((always_inline)) void
atomic_inc (int *v)
{
  __asm__ __volatile__ ("" : "+Q" (*v) : "r" (1));
}

static inline __attribute__((always_inline)) unsigned long
read_off (const unsigned long *p)
{
  unsigned long r;
  __asm__ ("" : "=r" (r) : "Q" (*p));
  return r;
}

int
f_union (struct slabx *s, int *cnt, int n)
{
  struct slabx new;
  atomic_inc (cnt);
  new.counters = s->counters;
  new.inuse -= n;
  return new.frozen + (int) new.counters;
}

long
f_arr (const struct big *src, int *cnt, int i)
{
  struct big b;
  atomic_inc (cnt);
  b = *src;
  return b.a[i & 7];
}

long
f_off (const struct big *src, const unsigned long *off, int i)
{
  struct big b;
  unsigned long o = read_off (off);
  b = *src;
  return b.a[(i + o) & 7];
}

long
k_q (const struct big *src, int i)
{
  struct big b;
  __asm__ __volatile__ ("" : : "Q" (b.a[1]));
  b = *src;
  return b.a[i & 7];
}

long
k_hreg (long v)
{
  register long hr asm ("x9") = v;
  long o;
  __asm__ __volatile__ ("add %0, %1, 1" : "=r" (o) : "r" (hr) : "memory");
  hr = 3;
  __asm__ __volatile__ ("" : : "r" (hr));
  return o;
}

/* { dg-final { scan-tree-dump-times "\\.DEFERRED_INIT" 1 "optimized" } } */
/* { dg-final { scan-tree-dump-not "new = \\.DEFERRED_INIT" "optimized" } } */
/* { dg-final { scan-tree-dump "hr = v_\[0-9\]+\\(D\\);" "optimized" } } */
