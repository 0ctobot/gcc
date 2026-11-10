/* { dg-options "-fcall-saved-x9" } */

__attribute__ ((noipa)) long
saved (long a)
{
  return a + 1;
}
