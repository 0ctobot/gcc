/* PR middle-end/127457 */
/* { dg-do run } */
/* { dg-options "-O2" } */

/* A target can raise the alignment of strings except when optimizing for size,
   and optimizations can assume the raised alignment.  The string in an -O2
   function must not reuse a copy of the string emitted for an -Os function.  */

typedef __UINTPTR_TYPE__ uintptr;

#define OS __attribute__((noipa, optimize ("Os")))
#define O2 __attribute__((noipa))

/* These only make the strings that follow them land at various offsets.  */
OS const char *os_pad1 (void) { return "x"; }
OS const char *os_pad2 (void) { return "xxx"; }

OS const char *os_str1 (void) { return "first string"; }
OS const char *os_str2 (void) { return "2nd string"; }
OS const char *os_str3 (void) { return "string number three"; }

O2 const char *o2_str1 (void) { return "first string"; }
O2 const char *o2_str2 (void) { return "2nd string"; }
O2 const char *o2_str3 (void) { return "string number three"; }

O2 uintptr o2_low1 (void) { return (uintptr) "first string" & 7; }
O2 uintptr o2_low2 (void) { return (uintptr) "2nd string" & 7; }
O2 uintptr o2_low3 (void) { return (uintptr) "string number three" & 7; }

__attribute__((noipa, optimize ("O0"))) static void
check (uintptr folded, const char *p)
{
  if (folded != ((uintptr) p & 7))
    __builtin_abort ();
}

int
main (void)
{
  os_pad1 ();
  os_pad2 ();
  os_str1 ();
  os_str2 ();
  os_str3 ();
  check (o2_low1 (), o2_str1 ());
  check (o2_low2 (), o2_str2 ());
  check (o2_low3 (), o2_str3 ());
  return 0;
}
