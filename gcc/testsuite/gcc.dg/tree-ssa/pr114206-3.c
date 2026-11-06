/* PR tree-optimization/114206 */
/* A live local must not block a tail call whose arguments can't hold its
   address, including values loaded from memory and returned by other calls.
   Once a branch on a result that may reveal that address can lead to the
   call, any argument that isn't constant can be a guess the branch confirmed,
   which keeps tail recursion on such values from turning into a loop.  */
/* { dg-do compile } */
/* { dg-options "-O2 -foptimize-sibling-calls -fdump-tree-tailc-details -fdump-tree-tailr1-details" } */

typedef __SIZE_TYPE__ size_t;

struct lock { int x; };
struct dev { struct lock l; int v; int a[8]; };
struct node { struct node *next; };

extern unsigned long _raw_spin_lock_irqsave (struct lock *);
extern void _raw_spin_unlock_irqrestore (struct lock *, unsigned long);
extern void fill (char *, size_t);
extern int _printk (const char *, ...);
extern int errno_of (int);
extern int read_it (void);
extern size_t strlen (const char *);
__attribute__ ((pure)) extern int use (int *);
extern struct node *head;

static __attribute__ ((noinline)) void
get_vals (struct dev *d, int *t)
{
  for (int i = 0; i < 4; i++)
    t[i] = d->a[i] * d->v;
}

static __attribute__ ((noinline)) int
get_val (struct dev *d, int *out)
{
  *out = d->v;
  return d->a[0];
}

/* A wide integer argument returned by another call.  */
void
k_unlock (struct dev *d)
{
  int tmp[4];
  unsigned long flags = _raw_spin_lock_irqsave (&d->l);
  get_vals (d, tmp);
  d->v = tmp[0] + tmp[1];
  _raw_spin_unlock_irqrestore (&d->l, flags);
}

/* The local's location decides a branch, which strlen can't make use of.  */
size_t
k_strlen (const char *p, int *q)
{
  char buf[32];
  if ((unsigned long) buf & 64)
    *q = 1;
  return strlen (p);
}

/* A narrow value loaded from memory in a variadic call.  */
int
k_printk (struct dev *d)
{
  int tmp[4];
  get_vals (d, tmp);
  d->v = tmp[2];
  return _printk ("%d\n", d->a[3]);
}

/* A narrow value returned by another call.  */
int
k_errno (struct dev *d)
{
  int tmp[4];
  get_vals (d, tmp);
  d->v = tmp[1];
  int status = read_it ();
  return errno_of (status);
}

/* A value the callee wrote to the local, whose address the callee only
   received to do that.  */
int
k_status (struct dev *d)
{
  int val;
  get_val (d, &val);
  return errno_of (val);
}

/* Tail recursion on a pointer loaded from memory can't become a loop, since
   getting past the early return on the result can confirm that the pointer
   was a guess of the local's address.  */
long
walk (struct node *p)
{
  int l = 1;
  if (use (&l))
    return 0;
  if (!p)
    return 0;
  return walk (p->next);
}

/* The same for an integer argument.  */
long
count_down (long n)
{
  int l = n;
  if (use (&l))
    return 0;
  if (n <= 0)
    return 0;
  return count_down (n - 1);
}

/* Tail recursion that hands over nothing, which the early return on the
   result can't pick.  */
long
drain (void)
{
  int l = 1;
  if (use (&l))
    return 0;
  if (!head)
    return 0;
  head = head->next;
  return drain ();
}

/* { dg-final { scan-tree-dump-times {Found tail call [^\n]*_raw_spin_unlock_irqrestore} 1 "tailc" } } */
/* { dg-final { scan-tree-dump-times {Found tail call [^\n]*strlen} 1 "tailc" } } */
/* { dg-final { scan-tree-dump-times {Found tail call [^\n]*_printk} 1 "tailc" } } */
/* { dg-final { scan-tree-dump-times {Found tail call [^\n]*errno_of} 2 "tailc" } } */
/* { dg-final { scan-tree-dump-times "Eliminated tail recursion" 1 "tailr1" } } */
/* { dg-final { scan-tree-dump {Eliminated tail recursion [^\n]*drain} "tailr1" } } */
/* { dg-final { scan-assembler {jmp\s+_raw_spin_unlock_irqrestore} { target { { i?86-*-* x86_64-*-* } && lp64 } } } } */
/* { dg-final { scan-assembler {jmp\s+strlen} { target { { i?86-*-* x86_64-*-* } && lp64 } } } } */
/* { dg-final { scan-assembler {\mb\s+_raw_spin_unlock_irqrestore} { target aarch64*-*-* } } } */
/* { dg-final { scan-assembler {\mb\s+strlen} { target aarch64*-*-* } } } */
