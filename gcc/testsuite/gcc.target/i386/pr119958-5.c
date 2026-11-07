/* PR target/119958 */
/* { dg-do compile { target { *-*-linux* && lp64 } } } */
/* { dg-require-effective-target tls } */
/* { dg-options "-O2 -fno-pic" } */

/* The assembler accepts @gottpoff only on a full-width mov or add that writes a
   general register, so no other insn can be allowed to read the GOT slot.  */

extern __thread char tb[64] __attribute__ ((tls_model ("initial-exec")));
extern void w (int, int, int, int, int, int, int, unsigned long);

#define O ((unsigned long) &tb[0] - (unsigned long) __builtin_thread_pointer ())

int cmp1 (long x) { return (long) O > x; }
int cmp2 (long x) { return x == (long) O; }
int cmp3 (int *p) { return *p == (int) O; }
int test1 (long x) { return (x & (long) O) != 0; }
long sub1 (long x) { return x - (long) O; }
short narrow1 (short x) { return x + (short) O; }
long narrow2 (long x) { return (long) (signed char) O + x; }
long narrow3 (int x) { return (long) (int) O + x; }
void push1 (void) { w (1, 2, 3, 4, 5, 6, 7, O); }
double conv1 (void) { return (double) (long) O; }

/* The fold into an add has to keep working.  */
int add1 (long x, long *p) { *p = x + (long) O; return *p == 0; }

/* { dg-final { scan-assembler-not {\m(?!addq|movq)[a-z0-9]+[ \t]+[^\n]*@gottpoff} } } */
/* { dg-final { scan-assembler {\maddq[ \t]+tb@gottpoff\(%rip\), %r} } } */
