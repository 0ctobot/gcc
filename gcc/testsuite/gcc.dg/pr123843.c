/* { dg-do compile } */
/* { dg-options "-O -ftrapv -finstrument-functions -fnon-call-exceptions" } */

int CRYPTO_DOWN_REF_ret;
int *asn1_do_lock_val;

void
asn1_do_lock ()
{
  CRYPTO_DOWN_REF_ret = __atomic_fetch_sub (asn1_do_lock_val, 1, 0) - 1;
}
