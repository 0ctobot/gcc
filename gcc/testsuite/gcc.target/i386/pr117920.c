/* PR target/117920 */
/* { dg-do compile { target { ! ia32 } } } */
/* { dg-options "-O2 -fharden-conditional-branches -mstackrealign --param=max-grow-copy-bb-insns=32" } */

float _mesa_snorm_to_half___trans_tmp_3;
long _mesa_snorm_to_half___trans_tmp_2;
__attribute__((__vector_size__(2 * sizeof(long)))) long _mesa_snorm_to_half_out;
int util_get_cpu_caps();
void _mesa_float_to_half_slow(float);
void _mesa_snorm_to_half() {
  if (_mesa_snorm_to_half___trans_tmp_2)
    _mesa_snorm_to_half___trans_tmp_3 = 1.0f;
  if (util_get_cpu_caps())
    __asm("" : "=v"(_mesa_snorm_to_half_out));
  _mesa_float_to_half_slow(0);
}

/* The prologue realigning the stack mustn't be duplicated.  */
/* { dg-final { scan-assembler-times "and\[lq\]\[ \t\]+\\\$-\[0-9\]+, %\[er\]sp" 1 } } */
