/* { dg-lto-do run } */
/* { dg-lto-options { { -O2 -flto } } } */

/* Two TUs declare z noipa and a third one defines it without the attribute,
   so both noipa declarations must keep the body hidden from their callers.  */

__attribute__ ((noipa)) int z (int);
extern int other (void);

int
main (void)
{
  return z (5) != 6 || other () != 6;
}
