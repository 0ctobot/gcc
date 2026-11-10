/* The strlen pass exempted the strings updated for the store of a call's
   aggregate return value from the invalidation check for the call itself,
   although the callee can write those strings before its value is stored.
   Every function below must see the nul that next_pair () writes.  */
/* { dg-do run } */
/* { dg-options "-O2" } */

typedef unsigned char u8;
struct pair { u8 a, b; };

static u8 *shared;

__attribute__((noipa)) struct pair
next_pair (void)
{
  shared[0] = 0;
  return (struct pair) { 'y', 'z' };
}

/* A store into a string from memcpy, then strlen.  */

__attribute__((noipa)) int
short_string (u8 *buf)
{
  shared = buf;
  __builtin_memcpy (buf, "abcdef", 7);
  *(struct pair *) (buf + 2) = next_pair ();
  return __builtin_strlen ((char *) buf) < 2;
}

/* A store into a string from strcpy, then a byte test.  */

__attribute__((noipa)) int
first_is_nul (u8 *buf)
{
  shared = buf;
  __builtin_strcpy ((char *) buf, "abcdef");
  *(struct pair *) (buf + 2) = next_pair ();
  return buf[0] == 0;
}

/* A store through a pointer into the middle of the string, which adjusts
   the length of the whole string as a related string.  */

__attribute__((noipa)) int
related (u8 *buf)
{
  shared = buf;
  __builtin_strcpy ((char *) buf, "abcdef");
  u8 *p = buf + 2;
  if (__builtin_strlen ((char *) p) != 4)
    __builtin_abort ();
  *(struct pair *) (p + 1) = next_pair ();
  return __builtin_strlen ((char *) buf) < 3;
}

/* A store into a local array whose address escapes.  */

__attribute__((noipa)) int
local_array (void)
{
  char s[16];
  shared = (u8 *) s;
  __builtin_strcpy (s, "abcdef");
  *(struct pair *) (s + 2) = next_pair ();
  return __builtin_strlen (s) < 2;
}

int
main (void)
{
  u8 b1[16], b2[16], b3[16];
  if (short_string (b1) != 1)
    __builtin_abort ();
  if (first_is_nul (b2) != 1)
    __builtin_abort ();
  if (related (b3) != 1)
    __builtin_abort ();
  if (local_array () != 1)
    __builtin_abort ();
  return 0;
}
