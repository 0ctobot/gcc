/* Identical noipa functions must not have their callers merged because the
   bodies reached through their aliases compare equal.  */
/* { dg-do compile } */
/* { dg-require-alias "" } */
/* { dg-options "-O2 -fipa-icf -fdump-ipa-icf-details" } */

__attribute__ ((noipa)) int
a1 (int v)
{
  return v + 1;
}

__attribute__ ((noipa)) int
a2 (int v)
{
  return v + 1;
}

extern int b1 (int) __attribute__ ((alias ("a1")));
extern int b2 (int) __attribute__ ((alias ("a2")));

int
c1 (int v)
{
  return b1 (v);
}

int
c2 (int v)
{
  return b2 (v);
}

/* { dg-final { scan-ipa-dump-not "Unified" "icf" } } */
