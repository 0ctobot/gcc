/* PR c/125973 */
/* { dg-do run } */
/* { dg-options "-std=gnu23 -O2" } */

/* A compound literal with static storage duration is one object that lives
   for the whole program, so a read must see the last value stored to it and
   not its initializer.  The object must also stay writable when a read of it
   can't be replaced by the initializer.  */

extern void abort (void);

typedef enum : bool { EF, ET } ebool;
typedef int v4si __attribute__ ((vector_size (16)));

int x;
char buf[8];

#define T(name, type, expr)					\
  __attribute__ ((noinline)) static type			\
  name (void)							\
  {								\
    return expr;						\
  }

T (bool_predec, int, --(static bool){})
T (bool_postdec, int, (static bool){}--)
T (bool_postinc, int, (static bool){}++)
T (bool_xor, int, (static bool){} ^= 1)
T (ebool_predec, int, --(static ebool){})
T (int_add, int, (static int){} += 1)
T (int_preinc, int, ++(static int){})
T (int_postinc, int, (static int){}++)
T (int_shift, int, (static int){1} <<= 1)
T (schar_sub, int, (static signed char){} -= 1)
T (long_add, long, (static long){} += 1)
T (float_add, float, (static float){} += 1)
T (double_mul, double, (static double){1} *= 2)
T (ptr_add, char *, (static char *){buf} += 1)
T (cplx_add, int, __real__ ((static _Complex int){} += 1))
T (addr_add, __INTPTR_TYPE__,
   ((static __INTPTR_TYPE__){(__INTPTR_TYPE__) &x} += 1) - (__INTPTR_TYPE__) &x)
T (vol_bool_predec, int, --(static volatile bool){})
T (vol_int_add, int, (static volatile int){} += 1)
T (member_add, int, (static struct { int a; }){}.a += 1)
T (static_read, int, (static int){5})
T (const_read, int, (static const int){5})
T (constexpr_read, int, (constexpr int){5})
T (constexpr_bool_read, int, (constexpr bool){1})
T (auto_int_add, int, (int){5} += 1)
T (auto_bool_predec, int, --(bool){1})
T (register_int_add, int, (register int){5} += 1)
T (register_bool_predec, int, --(register bool){1})

__attribute__ ((noinline)) static int
vec_add (void)
{
  v4si v = { 1, 1, 1, 1 };
  return ((static v4si){ 1, 2, 3, 4 } += v)[0];
}

int
main (void)
{
  for (int i = 0; i < 4; i++)
    {
      int odd = i & 1;

      if (bool_predec () != !odd
	  || bool_postdec () != odd
	  || bool_postinc () != (i != 0)
	  || bool_xor () != !odd
	  || ebool_predec () != !odd
	  || int_add () != i + 1
	  || int_preinc () != i + 1
	  || int_postinc () != i
	  || int_shift () != 2 << i
	  || schar_sub () != -i - 1
	  || long_add () != i + 1
	  || float_add () != i + 1
	  || double_mul () != 2 << i
	  || ptr_add () != buf + i + 1
	  || cplx_add () != i + 1
	  || addr_add () != i + 1
	  || vol_bool_predec () != !odd
	  || vol_int_add () != i + 1
	  || member_add () != i + 1
	  || vec_add () != i + 2)
	abort ();

      /* These can't change, or start over at each evaluation.  */
      if (static_read () != 5
	  || const_read () != 5
	  || constexpr_read () != 5
	  || constexpr_bool_read () != 1
	  || auto_int_add () != 6
	  || auto_bool_predec () != 0
	  || register_int_add () != 6
	  || register_bool_predec () != 0)
	abort ();
    }
  return 0;
}
