/* { dg-do compile } */
/* { dg-options "-O2 -mgeneral-regs-only -w -Wno-psabi" } */

/* A 16-byte generic vector has an integer mode without FP and SIMD.  */

typedef unsigned T __attribute__((vector_size(16)));
typedef int S __attribute__((vector_size(16)));

unsigned g7;
T g26;
_Bool f28_c9;

void f28()
{
    T a2;
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
    a2 = a2 > g26 < (S)g26;
    v16 = a2[0];
    a2 = ~a2;
    goto lbl_entry;
}
