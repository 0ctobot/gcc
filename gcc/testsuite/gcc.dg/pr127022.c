/* PR rtl-optimization/127022 */
/* The clobber of the uninitialized b that -fno-dce keeps must not be scheduled
   after the noreturn call to c.  */
/* { dg-do compile } */
/* { dg-options "-O2 -fno-dce -fsched2-use-superblocks -w" } */

extern void c (char *, char *, int, char *) __attribute__ ((__noreturn__));
extern void d ();
int a;

int
f ()
{
  int b;
  if (a)
    {
      d ();
      if (__builtin_expect (a, 0))
	{
	  if (__builtin_expect (b, 0))
	    while (1)
	      ;
	  c ("", "foo", 1, "bar");
	}
    }
  return 0;
}
