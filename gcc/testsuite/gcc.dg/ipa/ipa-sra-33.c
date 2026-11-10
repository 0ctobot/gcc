/* { dg-do compile } */
/* { dg-require-weak "" } */
/* { dg-options "-O2 -fdump-ipa-sra-details" } */

/* The address of a weak variable may be null, so it doesn't prove that the
   callee's guarded load can be moved into the caller.  */

struct cfg { long a, b, c; };

extern struct cfg weak_cfg __attribute__ ((weak));
static struct cfg ref_cfg __attribute__ ((weakref ("ref_cfg_real")));
#pragma weak pragma_cfg
extern struct cfg pragma_cfg;
extern struct cfg std_cfg, alt_cfg;

static __attribute__ ((noinline)) long
weak_b (struct cfg *p, int present)
{
  return present ? p->b : 5;
}

static __attribute__ ((noinline)) long
ref_a (struct cfg *p, int present)
{
  return present ? p->a : 6;
}

static __attribute__ ((noinline)) long
pragma_c (struct cfg *p, int present)
{
  return present ? p->c : 7;
}

/* Only ordinary variables are passed here, so this parameter is still
   split.  */
static __attribute__ ((noinline)) long
std_b (struct cfg *p, int present)
{
  return present ? p->b : 8;
}

long
sum (int have_opt, int have_std)
{
  return (weak_b (&weak_cfg, have_opt) + weak_b (&std_cfg, have_std)
	  + ref_a (&ref_cfg, have_opt) + ref_a (&std_cfg, have_std)
	  + pragma_c (&pragma_cfg, have_opt) + pragma_c (&std_cfg, have_std)
	  + std_b (&alt_cfg, have_opt) + std_b (&std_cfg, have_std));
}

/* { dg-final { scan-ipa-dump-times "Will split parameter" 1 "sra" } } */
/* { dg-final { scan-ipa-dump "Evaluating analysis results for std_b" "sra" } } */
