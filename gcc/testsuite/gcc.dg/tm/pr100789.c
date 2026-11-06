/* PR100789 */
/* { dg-do compile } */
/* { dg-options "-fgnu-tm -w" } */

int
bar (void)
{
  return __transaction_relaxed (0x1234567876543210LL << 32);
}
