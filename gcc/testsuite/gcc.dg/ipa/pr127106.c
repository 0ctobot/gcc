/* { dg-do compile } */
/* { dg-options "-O2 -fchecking" } */

/* IPA-SRA replaced a vector or complex component of a by-value parameter with
   a new register parameter even though the function stores to just a part of
   that component, which a register cannot represent.  */

typedef unsigned short v4hi __attribute__ ((vector_size (8)));

struct s0
{
  v4hi f0;
  void *f1;
};

struct s1
{
  _Complex int c;
  void *p;
};

unsigned short g2;
v4hi g4;
_Complex int g5;

static void
f4 (struct s0 a1, unsigned int a2)
{
  unsigned int v11;
  a1.f0[1] = 6;
  switch (v11)
    case 60428:
    case 2039730824:
    case 10418:
      g4 = a1.f0;
  a1.f0[a2] = g2;
}

void
f3 (void)
{
  f4 ((struct s0) { }, 9);
}

static __attribute__ ((noinline)) void
f5 (struct s1 a)
{
  __real__ a.c = g2;
  g5 = a.c;
}

void
f6 (void)
{
  f5 ((struct s1) { });
}
