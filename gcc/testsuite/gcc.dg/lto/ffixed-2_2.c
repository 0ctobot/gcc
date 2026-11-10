/* { dg-options "-fcall-saved-x15" } */

__attribute__ ((noipa)) long
saved (long a)
{
  return a + 1;
}
