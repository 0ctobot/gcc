/* { dg-do compile } */
/* { dg-additional-options "-w" } */

#include <stdint.h>
#define BITCAST(DST_TYPE, SRC_TYPE, VALUE)                                     \
    (union {                                                                   \
        SRC_TYPE src;                                                          \
        DST_TYPE dst;                                                         \
    }){ VALUE }                                                                \
        .dst
typedef int8_t v2i8 __attribute__((vector_size(2)));
typedef uint8_t v2u8 __attribute__((vector_size(2)));
_Bool g3;
__attribute__((cold)) void f12()
{
    v2u8 bc6;
    _Bool c10, c11;
    uint8_t v17;
    goto lbl_br25;
lbl_sw10:
    bc6 = (v2u8)((v2i8)bc6 * bc6);
    if (c11) __builtin_unreachable();
    bc6 = BITCAST(v2u8, uint16_t, ~BITCAST(uint16_t, v2u8, bc6));
    bc6 = ~bc6;
    g3 = v17;
lbl_br25:
    v17 = bc6[c10];
    if (c11) __builtin_memset(0, 0, 3);
    bc6 = bc6 > (v2u8){};
    goto lbl_sw10;
}

