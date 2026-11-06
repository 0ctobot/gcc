/* PR target/124266 */
/* A store pair that pair fusion has to insert as a new insn, because neither
   original store can be moved to where the pair goes, must carry the CFI note
   of the frame-related store.  Without it, dwarf2cfi falls back to the
   UNSPEC_STP pattern of the pair and ICEs.  */
/* { dg-do compile } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fasynchronous-unwind-tables -fno-schedule-fusion -fno-schedule-insns2" } */

void __RTL (startwith ("ldp_fusion2")) foo (void)
{
  (function "foo"
    (insn-chain
      (block 2
	(edge-from entry (flags "FALLTHRU"))
	(cnote 3 [bb 2] NOTE_INSN_BASIC_BLOCK)
	(cinsn/f 4 (set (mem:DI (plus:DI (reg/f:DI sp)
					 (const_int 8)) [0  S8 A64])
			(reg:DI x19)))
	(cinsn 5 (set (reg:DI x0)
		      (mem:DI (plus:DI (reg/f:DI sp)
				       (const_int 32)) [0  S8 A64])))
	(cinsn 6 (set (reg:DI x1) (const_int 5)))
	(cinsn 7 (set (reg:DI x19) (const_int 7)))
	(cinsn 8 (set (reg:DI x2)
		      (mem:DI (plus:DI (reg/f:DI sp)
				       (const_int 64)) [0  S8 A64])))
	(cinsn 9 (set (mem:DI (plus:DI (reg/f:DI sp)
				       (const_int 16)) [0  S8 A64])
		      (reg:DI x1)))
	(cinsn 10 (use (reg:DI x0)))
	(cinsn 11 (use (reg:DI x2)))
	(cinsn 12 (use (reg:DI x19)))
	(edge-to exit (flags "FALLTHRU"))
      ) ;; block 2
    ) ;; insn-chain
  ) ;; function
}

/* { dg-final { scan-assembler {\tstp\tx19, x1, \[sp, 8\]\n\t\.cfi_offset 19, 8\n} } } */
