/* PR target/69182 */
/* { dg-do compile { target ia32 } } */
/* { dg-options "-O2 -ffixed-ebp -fno-pic" } */

extern void g (char *, int);

int
main (int argc, char **argv)
{
  char *p = __builtin_alloca (argc * 16);
  g (p, argc);
  g (argv[0], argc);
  return 0;
}

/* main realigns the stack through the DRAP register and alloca needs the
   frame pointer, so the fixed %ebp must be saved and then used in that role.  */
/* { dg-final { scan-assembler "pushl\[ \t\]+%ebp" } } */
/* { dg-final { scan-assembler "movl\[ \t\]+%esp, %ebp" } } */
