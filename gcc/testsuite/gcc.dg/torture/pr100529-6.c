/* PR middle-end/100529 */
/* { dg-do run } */
/* { dg-skip-if "the stack must grow down" { hppa*-*-* } } */
/* { dg-additional-options "-fno-builtin-memcpy -Wno-vla" } */

/* An __auto_type variable can take the type declared by its own initializer.
   The initializer then runs in a statement of its own, ahead of the allocation
   of the variable, so the temporary holding its value must stay allocated until
   it has been copied into the variable.  The copy of a variable-sized object is
   a call to memcpy, which checks that its source hasn't been left below the
   stack pointer, where a callee or an interrupt would overwrite it.  */

typedef __SIZE_TYPE__ size_t;

static volatile int armed, ncopies;

__attribute__ ((noipa)) void *
memcpy (void *d, const void *s, size_t n)
{
  volatile char *vd = d;
  const volatile char *vs = s;

  if (armed)
    {
      char *frame = __builtin_frame_address (0);
      ncopies++;
      if ((char *) s < frame)
	__builtin_abort ();
      if ((char *) s < (char *) d + n && (char *) d < (char *) s + n)
	__builtin_abort ();
    }
  for (size_t i = 0; i < n; i++)
    vd[i] = vs[i];
  return d;
}

__attribute__ ((noipa)) int
get_size (void)
{
  return 4096;
}

__attribute__ ((noipa)) void
use (void *p)
{
  __asm__ ("" : : "r" (p) : "memory");
}

static void
fill (char *p, int n)
{
  for (int i = 0; i < n; i++)
    p[i] = i % 101;
}

static void
check (const char *p, int n)
{
  for (int i = 0; i < n; i++)
    if (p[i] != i % 101)
      __builtin_abort ();
}

/* The type is declared inside of the statement expression.  */

__attribute__ ((noipa)) void
own_type (void)
{
  __auto_type x = ({ int m = get_size (); struct { char a[m]; } t; fill (t.a, m); t; });
  use (&x);
  check (x.a, get_size ());
  if (sizeof (x) != 4096)
    __builtin_abort ();
}

/* The type is declared outside of it.  */

__attribute__ ((noipa)) void
outer_type (void)
{
  int m = get_size ();
  struct V { char a[m]; };
  __auto_type x = ({ struct V t; fill (t.a, m); t; });
  use (&x);
  check (x.a, m);
}

/* The space must still come back at the end of the block.  */

__attribute__ ((noipa)) void
in_loop (void)
{
  char *first = 0;
  for (int i = 0; i < 64; i++)
    {
      __auto_type x = ({ int m = get_size (); struct { char a[m]; } t; fill (t.a, m); t; });
      use (&x);
      check (x.a, get_size ());
      if (!first)
	first = x.a;
      else if (x.a != first)
	__builtin_abort ();
    }
}

int
main (void)
{
  armed = 1;
  own_type ();
  outer_type ();
  in_loop ();
  armed = 0;
  return 0;
}
