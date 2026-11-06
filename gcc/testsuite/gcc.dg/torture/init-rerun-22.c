/* { dg-do run { target aarch64*-*-* } } */
/* { dg-require-effective-target trampolines } */

/* An asm counts as a call since we can't tell what it does, and it can call a
   nested function that reads the old value of a variable of its parent through
   the static chain while the variable's initializer runs a second time.  */

struct S { long a, b, c[6]; };

#define CLOBBERS							\
  "x0", "x1", "x2", "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11",	\
  "x12", "x13", "x14", "x15", "x16", "x17", "x18", "x30",		\
  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v16", "v17", "v18",	\
  "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28",	\
  "v29", "v30", "v31", "cc", "memory"

__attribute__ ((noipa)) long
t_asm (void)
{
  int n = 0;
  long got = 0;
  auto void rd (void);
  void (*fp) (void) = rd;
again:;
  struct S v = { .a = n + 1,
		 .b = ({ void (*f) (void) = fp;
			 __asm__ volatile ("blr %0" : "+r" (f) : : CLOBBERS);
			 got; }) };
  __attribute__ ((noinline)) void
  rd (void)
  {
    got = n ? v.a : -1;
  }
  if (n++ < 1)
    goto again;
  return v.b;
}

int
main (void)
{
  if (t_asm () != 1)
    __builtin_abort ();
  return 0;
}
