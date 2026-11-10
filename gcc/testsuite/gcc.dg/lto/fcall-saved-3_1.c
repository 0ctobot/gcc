/* { dg-options "-fcall-used-x9 -fcall-saved-9" } */

__attribute__ ((noipa)) long
saved (long a)
{
  return a + 1;
}
