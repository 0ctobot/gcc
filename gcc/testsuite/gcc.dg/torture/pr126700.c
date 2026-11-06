/* { dg-do compile } */
/* { dg-additional-options "-w -Wno-psabi" } */

typedef unsigned v16u32 __attribute__((vector_size(64)));
typedef int v16s32 __attribute__((vector_size(64)));

unsigned g7;
v16u32 g26;
_Bool f28_c9;

void f28()
{
    v16u32 a2;
    unsigned v16;
lbl_entry:
    if (g7)
    {
        g7 = v16;
        f28_c9 = 402098599006253881;
    }
    switch (0)
    lbl_sw31:
        if (f28_c9) goto lbl_br72;
    g26 = 0 >= g26;
    goto lbl_sw31;
lbl_br72:
    a2 = a2 > g26 < (v16s32)g26;
    v16 = a2[3];
    a2 = ~a2;
    goto lbl_entry;
}
