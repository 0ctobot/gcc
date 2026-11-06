/* Convert between a mask and ordinary vector lanes without assuming that each
   mask element takes as much space as a data element.  */

static M __attribute__ ((noipa)) __GIMPLE (ssa, startwith ("veclower2"))
to_mask (V v, V zero)
{
  M m;

  __BB(2):
  m_3 = v_1(D) != zero_2(D);
  return m_3;
}

static V __attribute__ ((noipa)) __GIMPLE (ssa, startwith ("veclower2"))
from_mask (M m, V ones, V zero)
{
  V v;

  __BB(2):
  v_4 = m_1(D) ? ones_2(D) : zero_3(D);
  return v_4;
}
