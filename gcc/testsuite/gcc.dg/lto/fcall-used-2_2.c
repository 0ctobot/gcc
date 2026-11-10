/* { dg-options "-fno-lto" } */

__asm__ (".text\n"
	 ".p2align 2\n"
	 ".globl call_check\n"
	 ".type call_check, %function\n"
	 "call_check:\n"
	 "\tstp x29, x30, [sp, -32]!\n"
	 "\tstr x19, [sp, 16]\n"
	 "\tmov x19, #0x123\n"
	 "\tbl clobber\n"
	 "\tcmp x19, #0x123\n"
	 "\tcset w0, eq\n"
	 "\tldr x19, [sp, 16]\n"
	 "\tldp x29, x30, [sp], 32\n"
	 "\tret\n"
	 ".size call_check, .-call_check\n");
