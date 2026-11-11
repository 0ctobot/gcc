/* { dg-lto-do run } */
/* { dg-require-effective-target tls_runtime } */
/* { dg-skip-if "uses aarch64 system registers" { ! aarch64*-*-* } } */
/* { dg-lto-options { { -O2 -flto -fdump-ipa-inline } { -O1 -flto -fdump-ipa-inline } } } */

/* The second file is compiled with -mtp=tpidrro_el0, so its functions must
   keep reading the thread pointer from tpidrro_el0 when called from this
   file, which uses the default tpidr_el0.  Inlining them here would make
   them read tpidr_el0 instead, which means that only scale may be
   inlined.  */

extern void *ro_tp (void);
extern int *pick (int);
extern int *pick_ref (int);
extern int scale (int);

int
main (int argc, char **argv)
{
  void *tp;

  __asm__ volatile ("mrs %0, tpidrro_el0" : "=r" (tp));
  if (ro_tp () != tp)
    __builtin_abort ();
  if (pick (argc) != pick_ref (argc) || pick (!argc) != pick_ref (!argc))
    __builtin_abort ();
  if (scale (argc) != argc * 3 + 1)
    __builtin_abort ();
  return 0;
}

/* { dg-final { scan-wpa-ipa-dump "scale/\[0-9\]+ inlined" "inline" } } */
/* { dg-final { scan-wpa-ipa-dump-not "(?:pick|ro_tp)/\[0-9\]+ inlined" "inline" } } */
