/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* IPA-SRA moves the loads of a split by-value parameter into the callers, where
   they run unconditionally.  A load past the end of the parameter is only valid
   as long as it stays on the guarded path that never runs, so the parameter
   must not be split.  The constant offsets are big enough that a load which
   escapes its guard goes off the end of the stack and faults.  */

struct s3
{
  __attribute__ ((__vector_size__ (sizeof (int)))) int f0;
  unsigned int f3[19];
};

unsigned short g11, g24, g22, g12;
_Bool f5_repair25;

static void
f5 (struct s3 a1)
{
  _Bool repair21;
  if (f5_repair25)
    g24 = g12;
  if (g22 != 50295)
    repair21 = g11 != 5;
  if (repair21)
    return;
  g24 = a1.f0[62119];
}

struct s
{
  int a[20];
};

struct el
{
  int x[8];
};

struct arr
{
  struct el e[2];
};

volatile int guard;
int sink;

__attribute__ ((noipa)) void
init (void *p)
{
  *(int *) p = 1;
}

__attribute__ ((noipa)) void
use_el (struct el e)
{
  sink = e.x[0];
}

static __attribute__ ((noinline)) void
load_guarded (struct s p, int c)
{
  if (c)
    sink = p.a[1 << 26];
  sink += p.a[0];
}

static __attribute__ ((noinline)) void
arg_guarded (struct arr p, int c)
{
  if (c)
    use_el (p.e[1 << 23]);
  sink += p.e[0].x[0];
}

int
main (void)
{
  struct s t;
  struct arr u;

  f5 ((struct s3) { 0 });

  init (&t);
  load_guarded (t, guard);

  init (&u);
  arg_guarded (u, guard);

  return 0;
}
