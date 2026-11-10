/* A volatile read of a zeroed object has to load memory, which another
   agent may have written since the object was zeroed.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

typedef struct { unsigned long pte; } pte_t;
struct flag { unsigned long v; };

#define READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))

/* Write *ADDR the way a device or another CPU would, without telling the
   compiler.  The "m" input only keeps earlier stores to *ADDR ahead of the
   asm.  */
#define AGENT_STORE64(addr, val) \
  __asm__ volatile ("str %1, [%0]" : : "r" (addr), "r" (val), "m" (*(addr)))
#define AGENT_STORE32(addr, val) \
  __asm__ volatile ("str %w1, [%0]" : : "r" (addr), "r" (val), "m" (*(addr)))

static struct flag done;

__attribute__ ((noipa)) int
start_and_poll_once (void)
{
  done = (struct flag){};
  AGENT_STORE64 (&done.v, 1UL);
  struct flag f = READ_ONCE (done);
  return f.v != 0;
}

struct tbl { long hdr[2]; pte_t ent; long pad[5]; };
struct tbl gt;

__attribute__ ((noipa)) unsigned long
read_after_clear (void)
{
  __builtin_memset (&gt, 0, sizeof (gt));
  AGENT_STORE64 (&gt.ent.pte, 5UL);
  pte_t t = READ_ONCE (gt.ent);
  return t.pte;
}

struct pair { int a, b; };
struct regs { volatile struct pair status; int ctl; };

__attribute__ ((noipa)) void
read_status (struct regs *r, struct pair *out)
{
  *r = (struct regs){};
  AGENT_STORE32 ((int *) &r->status.b, 7);
  *out = r->status;
}

int
main (void)
{
  struct regs r;
  struct pair p;

  if (start_and_poll_once () != 1)
    __builtin_abort ();
  if (read_after_clear () != 5)
    __builtin_abort ();
  read_status (&r, &p);
  if (p.a != 0 || p.b != 7)
    __builtin_abort ();
  return 0;
}
