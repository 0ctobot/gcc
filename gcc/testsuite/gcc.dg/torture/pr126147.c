/* { dg-do run } */
/* { dg-additional-options "-fipa-pta" } */

/* Store merging turns the bit-field stores in the callees below into a
   read-modify-write of the containing bytes, which happens after IPA-PTA has
   run.  The late modref summary of the callee then says the call kills those
   bytes while the IPA-PTA use set still says the call doesn't read them, which
   used to let DSE in the caller delete or trim stores to the bytes' other
   bits.  */

struct bits
{
  unsigned char f1 : 1;
  unsigned char f2 : 1;
  unsigned char f3 : 1;
  unsigned char other;
};

/* These are byte-sized and byte-aligned, so they have DECL_BIT_FIELD clear.  */
struct bytes
{
  unsigned long long a : 8, b : 8, c : 8, d : 8, e : 8, f : 8, g : 8, h : 8;
};

struct bits gbits;

__attribute__ ((noinline)) static void
set_bits (struct bits *p)
{
  p->f1 = 1;
  p->f2 = 1;
}

/* The kill and the missing use both have to propagate through this call.  */
__attribute__ ((noinline)) static void
set_bits_wrap (struct bits *p)
{
  set_bits (p);
}

__attribute__ ((noinline)) static void
set_bytes (struct bytes *p)
{
  p->a = 1;
  p->c = 3;
  p->e = 5;
  p->g = 7;
}

__attribute__ ((noinline)) static struct bits *
heap_store (void)
{
  struct bits *p = __builtin_malloc (sizeof (*p));
  p->f3 = 1;
  set_bits (p);
  return p;
}

/* DSE trims this memset through a different path than the one deleting whole
   stores, by clearing the killed bytes from its live byte mask.  */
__attribute__ ((noinline)) static struct bits *
heap_memset (void)
{
  struct bits *p = __builtin_malloc (sizeof (*p));
  __builtin_memset (p, 0xff, sizeof (*p));
  set_bits (p);
  return p;
}

__attribute__ ((noinline)) static struct bits *
heap_wrap (void)
{
  struct bits *p = __builtin_malloc (sizeof (*p));
  p->f3 = 1;
  set_bits_wrap (p);
  return p;
}

__attribute__ ((noinline)) static struct bytes *
heap_bytes (void)
{
  struct bytes *p = __builtin_malloc (sizeof (*p));
  p->b = 2;
  p->d = 4;
  p->f = 6;
  p->h = 8;
  set_bytes (p);
  return p;
}

__attribute__ ((noinline)) static int
local_store (void)
{
  struct bits s;
  s.f3 = 1;
  set_bits (&s);
  __asm__ volatile ("" : : "r" (&s) : "memory");
  return s.f3;
}

__attribute__ ((noinline)) static void
parm_store (struct bits *p)
{
  p->f3 = 1;
  set_bits (p);
}

int
main (void)
{
  struct bits *a = heap_store ();
  struct bits *b = heap_memset ();
  struct bits *c = heap_wrap ();
  struct bytes *d = heap_bytes ();

  if (a->f3 != 1)
    __builtin_abort ();
  if (b->f3 != 1 || b->other != 0xff)
    __builtin_abort ();
  if (c->f3 != 1)
    __builtin_abort ();
  if (d->b != 2 || d->d != 4 || d->f != 6 || d->h != 8)
    __builtin_abort ();
  if (local_store () != 1)
    __builtin_abort ();
  parm_store (&gbits);
  if (gbits.f3 != 1)
    __builtin_abort ();
  return 0;
}
