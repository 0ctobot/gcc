/* { dg-do run } */
/* { dg-require-effective-target int32plus } */

/* Reads of a plain bit-field in a record with a volatile member, directly or
   in a nested record, or in a record nested in one, are limited to the
   bit-field's representative.  Check that every such read gives the same
   value as the same read from a record without the volatile qualifier,
   including fields that are read in pieces near the end of the region,
   fields of a packed record and reads with a variable index.  */

#define S1(N, V) struct N { V char x; unsigned a:20; char y; }
#define S2(N, V) struct N { _Bool a:1; int c:1; V signed char e; short f; }
#define S3(N, V) struct N { unsigned a:3; unsigned b:7; V char q; \
			    __UINT64_TYPE__ big:40; }
#define S4(N, V) struct N { V char v; __UINT64_TYPE__ f:44; unsigned char t; }
#define S5(N, V) struct N { V unsigned char status; \
			    struct { unsigned char a:4; unsigned short b:6; } f; \
			    long l; }
#define S6(N, V) struct N { V unsigned int status; \
			    struct { unsigned int a:4; int b:6; }; long l; }
#define S7(N, V) struct N { V int x; unsigned a:3; int s:9; }
#define S8(N, V) struct N { V unsigned char v; unsigned a:3; unsigned b:13; }
#define S9(N, V) struct N { struct { V int st; } in; unsigned a:5, b:11; }
#define S10(N, V) struct __attribute__((packed)) N { V char v; \
  struct __attribute__((packed)) { long long a:20; long long b:30; \
				   signed c:14; } in; V char w; }

S1 (s1v, volatile); S1 (s1n, );
S2 (s2v, volatile); S2 (s2n, );
S3 (s3v, volatile); S3 (s3n, );
S4 (s4v, volatile); S4 (s4n, );
S5 (s5v, volatile); S5 (s5n, );
S6 (s6v, volatile); S6 (s6n, );
S7 (s7v, volatile); S7 (s7n, );
S8 (s8v, volatile); S8 (s8n, );
S9 (s9v, volatile); S9 (s9n, );
S10 (s10v, volatile); S10 (s10n, );

#define RD(T, F, R) \
  __attribute__((noipa)) __INT64_TYPE__ R##v (struct T##v *p) \
  { return p->F; } \
  __attribute__((noipa)) __INT64_TYPE__ R##n (struct T##n *p) \
  { return p->F; }
#define RDI(T, F, R) \
  __attribute__((noipa)) __INT64_TYPE__ R##v (struct T##v *p, long i) \
  { return p[i].F; } \
  __attribute__((noipa)) __INT64_TYPE__ R##n (struct T##n *p, long i) \
  { return p[i].F; }

RD (s1, a, r1)
RD (s2, a, r2a)
RD (s2, c, r2c)
RD (s3, a, r3a)
RD (s3, b, r3b)
RD (s3, big, r3)
RD (s4, f, r4)
RD (s5, f.a, r5a)
RD (s5, f.b, r5b)
RD (s6, a, r6a)
RD (s6, b, r6b)
RD (s7, a, r7a)
RD (s8, a, r8a)
RD (s8, b, r8b)
RD (s9, b, r9)
RD (s10, in.a, r10a)
RD (s10, in.b, r10b)
RD (s10, in.c, r10c)
RDI (s7, s, r7i)
RDI (s3, big, r3i)
RDI (s5, f.b, r5i)

#define N 4
struct s1v a1v[N]; struct s1n a1n[N];
struct s2v a2v[N]; struct s2n a2n[N];
struct s3v a3v[N]; struct s3n a3n[N];
struct s4v a4v[N]; struct s4n a4n[N];
struct s5v a5v[N]; struct s5n a5n[N];
struct s6v a6v[N]; struct s6n a6n[N];
struct s7v a7v[N]; struct s7n a7n[N];
struct s8v a8v[N]; struct s8n a8n[N];
struct s9v a9v[N]; struct s9n a9n[N];
struct s10v a10v[N]; struct s10n a10n[N];

static __UINT64_TYPE__ seed;

static void
fill (void *v, void *n, unsigned long size, int k)
{
  unsigned char *pv = v, *pn = n;
  for (unsigned long i = 0; i < size; i++)
    {
      unsigned char b;
      if (k == 0)
	b = 0;
      else if (k == 1)
	b = 0xff;
      else if (k == 2)
	b = (i & 1) ? 0x80 : 0x7f;
      else
	{
	  seed = seed * 6364136223846793005ULL + 1442695040888963407ULL;
	  b = seed >> 56;
	}
      pv[i] = b;
      pn[i] = b;
    }
}

#define CHECK(R, A, I) \
  if (R##v (&A##v[I]) != R##n (&A##n[I])) __builtin_abort ()
#define CHECKI(R, A, I) \
  if (R##v (A##v, I) != R##n (A##n, I)) __builtin_abort ()
#define FILL(A) fill ((void *) A##v, (void *) A##n, sizeof A##v, k); \
  if (sizeof A##v != sizeof A##n) __builtin_abort ()

int
main (void)
{
  for (int k = 0; k < 24; k++)
    {
      FILL (a1); FILL (a2); FILL (a3); FILL (a4); FILL (a5); FILL (a6);
      FILL (a7); FILL (a8); FILL (a9); FILL (a10);
      for (int i = 0; i < N; i++)
	{
	  CHECK (r1, a1, i);
	  CHECK (r2a, a2, i);
	  CHECK (r2c, a2, i);
	  CHECK (r3a, a3, i);
	  CHECK (r3b, a3, i);
	  CHECK (r3, a3, i);
	  CHECK (r4, a4, i);
	  CHECK (r5a, a5, i);
	  CHECK (r5b, a5, i);
	  CHECK (r6a, a6, i);
	  CHECK (r6b, a6, i);
	  CHECK (r7a, a7, i);
	  CHECK (r8a, a8, i);
	  CHECK (r8b, a8, i);
	  CHECK (r9, a9, i);
	  CHECK (r10a, a10, i);
	  CHECK (r10b, a10, i);
	  CHECK (r10c, a10, i);
	  CHECKI (r7i, a7, i);
	  CHECKI (r3i, a3, i);
	  CHECKI (r5i, a5, i);
	}
    }
  return 0;
}
