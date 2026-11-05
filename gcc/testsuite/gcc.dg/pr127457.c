/* PR middle-end/127457 */
/* { dg-do run } */
/* { dg-options "-Os" } */

/* A wide string must not end up sharing the storage of a narrow string that
   has the same bytes, since the narrow string only has an alignment of 1.  The
   narrow strings have embedded nuls so they aren't mergeable and get placed
   next to each other in .rodata.  They have sizes that, along with the odd
   sized padding between them, cover every possible misalignment of a 32-bit
   wide string regardless of where .rodata starts.  */

typedef __UINTPTR_TYPE__ uintptr;

#define ALIGN16 _Alignof (__CHAR16_TYPE__)
#define ALIGN32 _Alignof (__CHAR32_TYPE__)

volatile int zero;

__attribute__((noipa)) static void
check (const void *p, unsigned long align)
{
  if ((uintptr) p % align)
    __builtin_abort ();
}

/* The addresses are used separately, so only the table of emitted constants
   can make them the same.  */
__attribute__((noipa)) static void
separate (void)
{
  check ("a\0", 1);
  check ("\0\0\0", 1);
  check (U"", ALIGN32);
  check ("b\0", 1);
  check ("\1\0\0\0\0\0\0", 1);
  check (U"\1", ALIGN32);
  check ("c\0", 1);
  check ("\2\0\0\0\0\0\0", 1);
  check (U"\2", ALIGN32);
  check ("d\0", 1);
  check ("\3\0\0\0\0\0\0", 1);
  check (U"\3", ALIGN32);
  check ("e", 1);
  check ("\4", 1);
  check (u"\4", ALIGN16);
  check ("f\0", 1);
  check ("\5\0\0", 1);
  check (u"\5", ALIGN16);
#if __SIZEOF_WCHAR_T__ == 4
  check ("g\0", 1);
  check ("\0\0\0", 1);
  check (L"", _Alignof (__WCHAR_TYPE__));
#endif
}

/* The two arms of the conditional would be folded into the narrow string if
   their addresses compared equal.  */
__attribute__((noipa)) static void
merged (int c)
{
  check ("h\0", 1);
  check (c ? (const void *) "\6\0\0\0\0\0\0" : (const void *) U"\6", ALIGN32);
  check ("i\0", 1);
  check (c ? (const void *) "\7\0\0\0\0\0\0" : (const void *) U"\7", ALIGN32);
  check ("j\0", 1);
  check (c ? (const void *) "\10\0\0\0\0\0\0" : (const void *) U"\10", ALIGN32);
  check ("k\0", 1);
  check (c ? (const void *) "\11\0\0\0\0\0\0" : (const void *) U"\11", ALIGN32);
}

int
main (void)
{
  separate ();
  merged (zero);
  return 0;
}
