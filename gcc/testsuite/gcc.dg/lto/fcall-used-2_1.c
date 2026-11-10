/* { dg-options "-fcall-used-x19" } */

__attribute__ ((noipa)) long
used (long a)
{
  return a + 1;
}
