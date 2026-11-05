/* The front end reuses one compound literal for both the read and the write
   when it lowers a compound assignment or a bool increment or decrement, so
   the gimplifier sees it twice.  The second time must not put the
   -ftrivial-auto-var-init initialization over the value from the first.  */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O0 -ftrivial-auto-var-init=zero" } */

extern void abort (void);

typedef int v4si __attribute__ ((vector_size (16)));

struct S
{
  int a;
  int b;
};

struct B
{
  int x : 8;
  int y : 8;
};

struct P
{
  int k;
  int *buf __attribute__ ((counted_by (k)));
};

static int calls;
static int arr[4];
static _Complex int carr[2];
static int glob = 5;
static volatile int vglob = 10;
static int mat[2][2];

__attribute__ ((noipa)) static int
val (int x)
{
  calls++;
  return x;
}

__attribute__ ((noipa)) static _Complex int
cval (int re, int im)
{
  calls++;
  return re + im * 1i;
}

/* A loop or a jump back must run the initialization on each evaluation.  */

__attribute__ ((noipa)) static int
loop_add (int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += (int) { val (i) } += 10;
  return sum;
}

__attribute__ ((noipa)) static int
loop_shift (int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += (int) { i } <<= 1;
  return sum;
}

__attribute__ ((noipa)) static int
loop_bool (int n)
{
  int sum = 0;
  for (int i = 0; i < n; i++)
    sum += --(bool) { val (i & 1) };
  return sum;
}

__attribute__ ((noipa)) static int
jump_back (int n)
{
  int sum = 0, i = 0;
again:
  sum += (int) { val (i) } += 1;
  if (++i < n)
    goto again;
  return sum;
}

__attribute__ ((noipa)) static int
var_init (int n)
{
  return (int) { n * 2 } += 1;
}

__attribute__ ((noipa)) static int
load_init (int *p)
{
  return (int) { *p } += 1;
}

__attribute__ ((noipa)) static long
vla_ptr (int n, int (*q)[n])
{
  return ((int (*)[n]) { q } += 1) - q;
}

/* The front end also reuses the literal to find the count of a counted_by
   member.  The size is unknown without optimization.  */

__attribute__ ((noipa)) static long
counted_size (void)
{
  int *p = (struct P) { 3, arr }.buf;
  return __builtin_dynamic_object_size (p, 1);
}

__attribute__ ((noipa)) static long
counted_size_call (void)
{
  int *p = (struct P) { val (3), arr }.buf;
  return __builtin_dynamic_object_size (p, 1);
}

#define CHECK(expr, want, wcalls)		\
  do						\
    {						\
      calls = 0;				\
      if ((expr) != (want) || calls != (wcalls))	\
	abort ();				\
    }						\
  while (0)

int
main (void)
{
  int x;

  CHECK ((int) { val (1) } += 100, 101, 1);
  CHECK ((int) { val (1) } -= 100, -99, 1);
  CHECK ((int) { val (6) } *= 3, 18, 1);
  CHECK ((int) { val (6) } /= 3, 2, 1);
  CHECK ((int) { val (7) } %= 3, 1, 1);
  CHECK ((int) { val (1) } <<= 1, 2, 1);
  CHECK ((int) { val (6) } >>= 1, 3, 1);
  CHECK ((int) { val (7) } &= 3, 3, 1);
  CHECK ((int) { val (5) } |= 2, 7, 1);
  CHECK ((int) { val (5) } ^= 1, 4, 1);

  CHECK ((char) { val (1) } += 1, 2, 1);
  CHECK ((short) { val (2) } <<= 1, 4, 1);
  CHECK ((long) { val (1) } += 1, 2, 1);
  CHECK ((long long) { val (1) } += 1, 2, 1);
  CHECK (((long) { (long) &x } += 1) - (long) &x, 1, 0);
  CHECK (((int *) { &arr[1] } += 1) - arr, 2, 0);

  CHECK ((register int) { val (1) } += 1, 2, 1);
  CHECK ((register int) { val (1) } <<= 1, 2, 1);
  CHECK ((volatile int) { val (1) } += 1, 2, 1);
  CHECK ((volatile int) { 5 } += 1, 6, 0);

  CHECK (--(bool) { val (1) }, 0, 1);
  CHECK (++(bool) { val (0) }, 1, 1);
  CHECK ((bool) { val (1) }--, 1, 1);
  CHECK ((bool) { val (0) }--, 0, 1);
  CHECK ((bool) { val (0) }++, 0, 1);

  CHECK (__real__ ((_Complex int) { cval (1, 5) } += 1), 2, 1);
  CHECK (__imag__ ((_Complex int) { cval (1, 5) } += 1), 5, 1);
  CHECK (__real__ ++(_Complex int) { cval (1, 5) }, 2, 1);
  CHECK (__imag__ ++(_Complex int) { cval (1, 5) }, 5, 1);
  CHECK (__real__ (_Complex int) { cval (1, 5) } += 1, 2, 1);
  CHECK (__imag__ (_Complex int) { cval (1, 5) } += 1, 6, 1);

  CHECK (((v4si) { val (1), 2, 3, 4 } += (v4si) { 1, 1, 1, 1 })[0], 2, 1);
  CHECK (((v4si) { val (1), 2, 3, 4 } += (v4si) { 1, 1, 1, 1 })[1], 3, 1);

  CHECK (((struct S) { val (1), val (2) }.a += 1), 2, 2);
  CHECK (((struct S) { val (1), val (2) }.b += 1), 3, 2);
  CHECK (((int [2]) { val (1), val (2) }[1] += 1), 3, 2);

  /* An identity operation on a part of the literal assigns that part to
     itself, which reads it after the second gimplification of the literal.  */
  CHECK (((struct S) { val (1), 2 }.a += 0), 1, 1);
  CHECK (((struct S) { val (1), val (2) }.b *= 1), 2, 2);
  CHECK (((int [1]) { val (1) })[0] += 0, 1, 1);
  CHECK ((struct B) { val (1) }.x += 0, 1, 1);
  CHECK (__real__ (_Complex int) { cval (1, 5) } += 0, 1, 1);
  CHECK (__imag__ (_Complex int) { cval (1, 5) } *= 1, 5, 1);
  CHECK (((v4si) { val (1), 2, 3, 4 } += (v4si) { })[0], 1, 1);
  CHECK (((v4si) { val (1), 2, 3, 4 } += (v4si) { })[1], 2, 1);
  CHECK (((v4si) { val (1), 2, 3, 4 } &= (v4si) { -1, -1, -1, -1 })[0], 1, 1);
  CHECK ((int) { val (1) } += (int) { 0 }, 1, 1);

  /* A literal in the size of a variably modified type.  */
  CHECK (({ int v[(int) { val (2) } <<= 1]; (long) sizeof (v); }),
	 4 * sizeof (int), 1);

  /* Other evaluations next to the literal.  */
  CHECK ((int) { val (1) } += val (2), 3, 2);
  CHECK ((int) { val (1) } += (int) { val (2) }, 3, 2);
  CHECK ((int) { (int) { val (1) } += 1 } += 1, 3, 1);
  CHECK ((int) { ({ int t = val (1); t; }) } += 2, 3, 1);
  CHECK (({ (int) { val (1) } += 1; }), 2, 1);
  CHECK ((val (1), (int) { val (1) } += 1), 2, 2);
  CHECK (val (1) ? (int) { val (1) } += 1 : 0, 2, 2);

  /* Literals used as an array index in a compound assignment.  */
  arr[1] = 0;
  CHECK (arr[(int) { val (1) }] += 1, 1, 1);
  CHECK (arr[1], 1, 0);
  carr[0] = carr[1] = 0;
  CHECK (__real__ carr[(int) { val (1) }] += 1, 1, 1);
  CHECK (__real__ carr[1], 1, 0);
  CHECK (__real__ carr[0], 0, 0);

  /* Initializers that aren't a call.  */
  CHECK (var_init (3), 7, 0);
  CHECK (load_init (&glob), 6, 0);
  CHECK ((int) { glob } += 1, 6, 0);
  CHECK ((int) { vglob } += 1, 11, 0);
  CHECK (vla_ptr (2, mat), 1, 0);

  if (counted_size () != 3 * (long) sizeof (int) && counted_size () != -1)
    abort ();
  if (counted_size_call () != 3 * (long) sizeof (int)
      && counted_size_call () != -1)
    abort ();

  CHECK (loop_add (5), 60, 5);
  CHECK (loop_shift (5), 20, 0);
  CHECK (loop_bool (4), 2, 4);
  CHECK (jump_back (4), 10, 4);

  return 0;
}
