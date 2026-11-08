// { dg-options "-O2 -g -fno-schedule-insns -fno-schedule-insns2 -fdump-rtl-early_ra" }

#include <arm_sme.h>

#pragma GCC target "+sme2"

// Each tuple is loaded into a strided group, so the vectors of one tuple
// are four registers apart.  A debug bind that extracts a single vector
// from a tuple must name the register that holds that vector rather than
// the register at the same offset from the first register of the group.

void test1(int32_t *dest, int32_t *src) __arm_streaming
{
  svcount_t pg = svptrue_c32();
  svint32x4_t l0 = svld1_vnum_x4(pg, src, 0);
  svint32x4_t l1 = svld1_vnum_x4(pg, src, 4);
  svint32x4_t l2 = svld1_vnum_x4(pg, src, 8);
  svint32x4_t l3 = svld1_vnum_x4(pg, src, 12);
  svint32_t dbg1 = svget4(l1, 1);
  svint32_t dbg2 = svget4(l2, 2);
  svint32_t dbg3 = svget4(l3, 3);
  svst1_vnum(pg, dest, 0,
	     svcreate4(svget4(l0, 0), svget4(l1, 0),
		       svget4(l2, 0), svget4(l3, 0)));
  svst1_vnum(pg, dest, 4,
	     svcreate4(svget4(l0, 1), svget4(l1, 1),
		       svget4(l2, 1), svget4(l3, 1)));
  svst1_vnum(pg, dest, 8,
	     svcreate4(svget4(l0, 2), svget4(l1, 2),
		       svget4(l2, 2), svget4(l3, 2)));
  svst1_vnum(pg, dest, 12,
	     svcreate4(svget4(l0, 3), svget4(l1, 3),
		       svget4(l2, 3), svget4(l3, 3)));
}

// { dg-final { scan-assembler {\tld1w\t\{z17\.s, z21\.s, z25\.s, z29\.s\}} } }
// { dg-final { scan-assembler {\tld1w\t\{z18\.s, z22\.s, z26\.s, z30\.s\}} } }
// { dg-final { scan-assembler {\tld1w\t\{z19\.s, z23\.s, z27\.s, z31\.s\}} } }
// { dg-final { scan-rtl-dump {var_location:VNx4SI dbg1 \(reg:VNx4SI [0-9]+ v21\)\) } "early_ra" } }
// { dg-final { scan-rtl-dump {var_location:VNx4SI dbg2 \(reg:VNx4SI [0-9]+ v26\)\) } "early_ra" } }
// { dg-final { scan-rtl-dump {var_location:VNx4SI dbg3 \(reg:VNx4SI [0-9]+ v31\)\) } "early_ra" } }
