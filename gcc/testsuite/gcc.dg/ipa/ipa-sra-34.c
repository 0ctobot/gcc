/* { dg-do run } */
/* { dg-require-effective-target weak_undefined } */
/* { dg-options "-O2" } */
/* { dg-add-options weak_undefined } */

/* opt_cfg is never defined, so its address is null.  cfg_b reads it only
   when told the object is present, and that read must not be moved into
   sum, where it would run unconditionally.  */

struct cfg { long a, b, c; };

extern struct cfg opt_cfg __attribute__ ((weak));
struct cfg std_cfg = { 1, 2, 3 };
volatile int have_opt = 0, have_std = 1;

static __attribute__ ((noinline)) long
cfg_b (struct cfg *p, int present)
{
  return present ? p->b : 5;
}

__attribute__ ((noinline)) long
sum (void)
{
  return cfg_b (&opt_cfg, have_opt) + cfg_b (&std_cfg, have_std);
}

int
main (void)
{
  if (sum () != 7)
    __builtin_abort ();
  return 0;
}
