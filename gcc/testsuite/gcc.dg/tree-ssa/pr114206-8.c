/* PR tree-optimization/114206 */
/* A leaf builtin reads through the pointers it's handed, as far as the length
   it's handed says, which a sibling call would have it do after the frame is
   released.  A pointer or a length depending on a result that may carry the
   address of a local therefore keeps the call from being a tail call, as does
   any operand once a branch on the result can lead to the call.  Operands
   that don't depend on the result otherwise leave it be.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-tree-loop-distribute-patterns -fdump-tree-tailc-details" } */

typedef __SIZE_TYPE__ size_t;
typedef __UINTPTR_TYPE__ uintptr_t;

extern long dst;
extern int seen;

__attribute__ ((noinline, noclone)) int
same (int *a, uintptr_t c)
{
  return (uintptr_t) a == c;
}

__attribute__ ((noinline, noclone)) void *
find (int *a, uintptr_t c)
{
  return (uintptr_t) a == c ? (void *) c : (void *) 0;
}

/* The search stops at the address of x[128].  */
void *
copy_found (uintptr_t b, size_t n)
{
  int x[256];
  x[128] = 1;
  for (uintptr_t c = b; c < b + 4096; c += 4)
    if (same (&x[128], c))
      return __builtin_memcpy (&dst, (void *) c, n);
  return 0;
}

void *
copy_returned (uintptr_t b, size_t n)
{
  int x[256];
  x[128] = 1;
  for (uintptr_t c = b; c < b + 4096; c += 4)
    {
      void *r = find (&x[128], c);
      if (r)
	return __builtin_memcpy (&dst, r, n);
    }
  return 0;
}

/* The pointer is a parameter, but the length reaches up to x[128].  */
void *
copy_upto (uintptr_t b)
{
  int x[256];
  x[128] = 1;
  for (uintptr_t c = b; c < b + 4096; c += 4)
    if (same (&x[128], c))
      return __builtin_memcpy (&dst, (void *) b, c - b + 4);
  return 0;
}

/* Filling x[] converts its address to an integer before the search.  */
void *
copy_filled (uintptr_t b)
{
  int x[256];
  for (int i = 0; i < 256; i++)
    x[i] = i;
  for (uintptr_t c = b; c < b + 4096; c += 4)
    if (same (&x[128], c))
      return __builtin_memcpy (&dst, (void *) b, c - b + 4);
  return 0;
}

size_t
length_found (uintptr_t b)
{
  int x[256];
  x[128] = 0x00414141;
  for (uintptr_t c = b; c < b + 4096; c += 4)
    if (same (&x[128], c))
      return __builtin_strlen ((char *) c);
  return 0;
}

/* Reaching the call tells which way the branch went, so SRC may be the
   address of x[128] that the helper confirmed.  */
void *
copy_guess (void *src, size_t n)
{
  int x[256];
  x[128] = 1;
  if (same (&x[128], (uintptr_t) src))
    return 0;
  return __builtin_memcpy (&dst, src, n);
}

/* Nothing handed to the builtin depends on the result, and nothing branches
   on it.  */
void *
copy_unrelated (void *src, size_t n)
{
  int x[256];
  x[128] = 1;
  seen = same (&x[128], (uintptr_t) src);
  return __builtin_memcpy (&dst, src, n);
}

/* { dg-final { scan-tree-dump-times "Found tail call \[^\n\]*memcpy" 1 "tailc" } } */
/* { dg-final { scan-tree-dump-not "Found tail call \[^\n\]*strlen" "tailc" } } */
