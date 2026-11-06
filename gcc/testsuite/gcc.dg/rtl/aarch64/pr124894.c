/* PR target/124894 */
/* { dg-do compile { target aarch64*-*-* } } */
/* { dg-require-effective-target lp64 } */
/* { dg-options "-O2 -fdump-rtl-dse1" } */

/* DSE deletes the first store to g.  The REG_EQUAL note that reads g has to
   go since it might depend on the deleted store, but the note that reads the
   stack frame can stay because the frame can't alias g.  */

int g;

int __RTL (startwith ("dse1")) foo (int a, int b)
{
  (function "foo"
    (param "a"
      (DECL_RTL (reg/v:SI <0> [ a ]))
      (DECL_RTL_INCOMING (reg:SI x0 [ a ])))
    (param "b"
      (DECL_RTL (reg/v:SI <1> [ b ]))
      (DECL_RTL_INCOMING (reg:SI x1 [ b ])))
    (insn-chain
      (block 2
	(edge-from entry (flags "FALLTHRU"))
	(cnote 3 [bb 2] NOTE_INSN_BASIC_BLOCK)
	(cinsn 4 (set (reg:SI <0>) (reg:SI x0)))
	(cinsn 5 (set (reg:SI <1>) (reg:SI x1)))
	(cinsn 6 (set (reg/f:DI <2>) (high:DI (symbol_ref:DI ("g")))))
	(cinsn 7 (set (reg/f:DI <3>) (lo_sum:DI (reg/f:DI <2>)
						(symbol_ref:DI ("g")))))
	(cinsn 8 (set (mem/c:SI (reg/f:DI <3>) [1 g+0 S4 A32]) (reg:SI <0>)))
	(cinsn 9 (set (mem/c:SI (reg/f:DI <3>) [1 g+0 S4 A32]) (reg:SI <1>)))
	(cinsn 10 (set (reg:SI <4>) (reg:SI <0>)) "pr124894.c":1
	  (expr_list:REG_EQUAL (mem/c:SI (plus:DI (reg/f:DI sfp)
						  (const_int -4))
					 [1 x+0 S4 A32])))
	(cinsn 11 (set (reg:SI <5>) (reg:SI <1>)) "pr124894.c":1
	  (expr_list:REG_EQUAL (mem/c:SI (reg/f:DI <3>) [1 g+0 S4 A32])))
	(cinsn 12 (set (reg:SI <6>) (mem/c:SI (reg/f:DI <3>) [1 g+0 S4 A32])))
	(cinsn 13 (set (reg:SI <7>) (plus:SI (reg:SI <4>) (reg:SI <5>))))
	(cinsn 14 (set (reg:SI <8>) (plus:SI (reg:SI <7>) (reg:SI <6>))))
	(cinsn 15 (set (reg/i:SI x0) (reg:SI <8>)))
	(cinsn 16 (use (reg/i:SI x0)))
	(edge-to exit (flags "FALLTHRU"))
      ) ;; block 2
    ) ;; insn-chain
    (crtl
      (return_rtx
	(reg/i:SI x0)
      ) ;; return_rtx
    ) ;; crtl
  ) ;; function
}

/* { dg-final { scan-rtl-dump "local deletions = 1" "dse1" } } */
/* { dg-final { scan-rtl-dump {REG_EQUAL \(mem/c:SI \(plus:DI \(reg/f:DI [0-9]+ sfp\)} "dse1" } } */
/* { dg-final { scan-rtl-dump-not {REG_EQUAL \(mem/c:SI \(reg/f:DI [0-9]+\) \[1 g\+0 S4 A32\]\)} "dse1" } } */
