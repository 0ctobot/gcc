/* Aggregate zero propagation must not replace a volatile read of a zeroed
   object with zeros.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-forwprop1-details -fdump-tree-optimized" } */

typedef struct { unsigned long pte; } pte_t;
struct flag { unsigned long v; };

#define READ_ONCE(x) (*(const volatile __typeof__ (x) *) &(x))
#define WRITE_ONCE(x, val) \
  do { *(volatile __typeof__ (x) *) &(x) = (val); } while (0)

static struct flag done;

void
set_done (void)
{
  struct flag one = { 1 };
  WRITE_ONCE (done, one);
}

/* The flag can be set between the clear and the read.  */
int
start_and_poll_once (void)
{
  done = (struct flag){};
  __asm__ volatile ("");
  struct flag f = READ_ONCE (done);
  return f.v != 0;
}

struct tbl { long long hdr[2]; pte_t ent; long pad[5]; };
struct tbl gt;

unsigned long
read_after_clear (void)
{
  __builtin_memset (&gt, 0, sizeof (gt));
  pte_t t = READ_ONCE (gt.ent);
  return t.pte;
}

struct pair { int a, b; };
struct regs { volatile struct pair status; int ctl; };

void
read_status (struct regs *r, struct pair *out)
{
  *r = (struct regs){};
  *out = r->status;
}

/* A plain zero stored to volatile memory can still have its source
   replaced with {}; the volatile store stays.  */
void
pte_clear (pte_t *ptep)
{
  pte_t z = { 0 };
  WRITE_ONCE (*ptep, z);
}

/* A plain read after the clear is still folded.  */
unsigned long
plain_read (void)
{
  __builtin_memset (&gt, 0, sizeof (gt));
  pte_t t = gt.ent;
  return t.pte;
}

/* { dg-final { scan-tree-dump-not "Simplified\n\[^\n\]* ={v} \[^\n\]*(&done|&gt|->status)" "forwprop1" } } */
/* { dg-final { scan-tree-dump "into\n *t = \\{\\};" "forwprop1" } } */
/* { dg-final { scan-tree-dump "={v} MEM\[^\n\]*&done\\\];" "optimized" } } */
/* { dg-final { scan-tree-dump "={v} MEM\[^\n\]*&gt \\+ 16B\\\];" "optimized" } } */
/* { dg-final { scan-tree-dump "={v} r_\[0-9\]+\\(D\\)->status;" "optimized" } } */
/* { dg-final { scan-tree-dump "ptep_\[0-9\]+\\(D\\)\\\] ={v} \\{\\};" "optimized" } } */
/* { dg-final { scan-tree-dump-times "={v}" 5 "optimized" } } */
