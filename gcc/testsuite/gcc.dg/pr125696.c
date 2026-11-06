/* { dg-do compile } */
/* { dg-options "-O2 -fchecking" } */

/* Once FRE turned the partial store to vec0 into a store of an empty
   constructor, rewriting it to a BIT_INSERT_EXPR left a VIEW_CONVERT_EXPR of
   that constructor behind, which isn't valid GIMPLE.  */

typedef short int16_t; typedef int int32_t; typedef signed char int8_t;
typedef int16_t v2i16 __attribute__((vector_size(4)));
_Bool g1, g18;
void g28()
{
    v2i16 vec0;
    int32_t v4;
    int32_t v5;
    void *p7 = &v4;
    v5 = __builtin_clrsb(v4);
    g18 = 0;
lbl_br6:
    *(int32_t *)p7 = 0;
    int32_t bb9 = -v5;
    if (g18) goto lbl_b65;
    do bb9 = __builtin_clz(bb9);
    while (bb9);
    switch ((int32_t)v5)
    case 65:
    case 7033:
        goto lbl_b65;
    p7 = &vec0;
    g18 = 6;
    goto lbl_br6;
lbl_b65:
    vec0[0] = 0;
    g1 = *(int8_t *)p7;
}
