/* { dg-lto-do run } */
/* { dg-require-effective-target tls_runtime } */
/* { dg-skip-if "uses aarch64 system registers" { ! aarch64*-*-* } } */
/* { dg-lto-options { { -O2 -flto } { -O3 -flto } } } */

/* main computes the addresses of v and w from tpidr_el0, while the second
   file is compiled with -mtp=tpidrro_el0 and computes them from that
   register instead.  IPA-CP must not specialize the functions in the second
   file for an address that main passes, directly or within a structure,
   since a specialized copy would recompute that address from tpidrro_el0.
   ro_step passes the address of w from within the second file, so
   specializing step for that call is still fine.  */

__thread int v[4], w[4];

struct ref { int *p; };
struct two { int *p; int k; };

extern int *step (int *, int);
extern int *step_ref (const struct ref *, int);
extern int *step_two (struct two);
extern int *ro_step (int);
extern int *ro_ref (int);

int
main (void)
{
  struct ref r = { v };
  struct two t1 = { v, 1 }, t2 = { v, 2 };

  if (step (w, 1) != &w[1] || step (w, 2) != &w[2])
    __builtin_abort ();
  if (step_ref (&r, 1) != &v[1] || step_ref (&r, 2) != &v[2])
    __builtin_abort ();
  if (step_two (t1) != &v[1] || step_two (t2) != &v[2])
    __builtin_abort ();
  if (ro_step (1) != ro_ref (1) || ro_step (3) != ro_ref (3))
    __builtin_abort ();
  return 0;
}
