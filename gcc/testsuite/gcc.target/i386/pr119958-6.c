/* PR target/119958 */
/* { dg-do compile { target { *-*-linux* && ia32 } } } */
/* { dg-require-effective-target tls } */
/* { dg-options "-O2 -fno-pic" } */

/* The assembler accepts @indntpoff only on a full-width mov or add that writes a
   general register, so no other insn can be allowed to read the GOT slot.  */

extern __thread char tb[64] __attribute__ ((tls_model ("initial-exec")));
extern void w (unsigned long);

#define O ((unsigned long) &tb[0] - (unsigned long) __builtin_thread_pointer ())

int cmp1 (long x) { return (long) O > x; }
int cmp2 (long x) { return x == (long) O; }
long sub1 (long x) { return x - (long) O; }
long narrow1 (long x) { return (long) (signed char) O + x; }
void push1 (void) { w (O); }
double conv1 (void) { return (double) (long) O; }

/* { dg-final { scan-assembler-not {\m(?!addl|movl)[a-z0-9]+[ \t]+[^\n]*@indntpoff} } } */
