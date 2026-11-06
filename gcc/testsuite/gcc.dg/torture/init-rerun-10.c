/* { dg-do run } */

/* When a jump back runs an initializer again, the old value of a variable with
   its address taken can be read through a pointer until the new value has been
   evaluated.  A call has no side effects to show for that read if it's pure or
   a builtin like strlen, and it reads the same memory as an open-coded read.  */

struct S { char s[16]; long pad[6]; long x; };

struct S *gp;

__attribute__ ((noipa, pure)) long
first (void)
{
  return gp ? gp->s[0] : -1;
}

__attribute__ ((noipa)) long
open_coded (void)
{
  int n = 0;
again:;
  struct S v = { .x = gp ? gp->s[0] : -1 };
  v.s[0] = 7;
  gp = &v;
  if (n++ < 1)
    goto again;
  gp = 0;
  return v.x;
}

__attribute__ ((noipa)) long
pure_call (void)
{
  int n = 0;
again:;
  struct S v = { .x = first () };
  v.s[0] = 7;
  gp = &v;
  if (n++ < 1)
    goto again;
  gp = 0;
  return v.x;
}

__attribute__ ((noipa)) long
strlen_ptr (void)
{
  char *p = 0;
  int n = 0;
again:;
  struct S v = { .x = p ? (long) __builtin_strlen (p) : -1 };
  __builtin_strcpy (v.s, "abcde");
  p = v.s;
  if (n++ < 1)
    goto again;
  return v.x;
}

__attribute__ ((noipa)) long
strlen_member (void)
{
  struct S *q = 0;
  int n = 0;
again:;
  struct S v = { .x = q ? (long) __builtin_strlen (q->s) : -1 };
  __builtin_strcpy (v.s, "abcde");
  q = &v;
  if (n++ < 1)
    goto again;
  return v.x;
}

__attribute__ ((noipa)) long
memcmp_ptr (void)
{
  char *p = 0;
  int n = 0;
again:;
  struct S v = { .x = p ? __builtin_memcmp (p, "abc", 3) : -1 };
  __builtin_strcpy (v.s, "abcde");
  p = v.s;
  if (n++ < 1)
    goto again;
  return v.x;
}

int
main (void)
{
  if (open_coded () != 7)
    __builtin_abort ();
  if (pure_call () != 7)
    __builtin_abort ();
  if (strlen_ptr () != 5)
    __builtin_abort ();
  if (strlen_member () != 5)
    __builtin_abort ();
  if (memcmp_ptr () != 0)
    __builtin_abort ();
  return 0;
}
