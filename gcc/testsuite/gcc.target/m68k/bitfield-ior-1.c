/* { dg-do compile } */
/* { dg-options "-O2" } */

/* The IOR of single bit fields from the same word is 0 or 1, so it
   mustn't be simplified to an NE of the word ANDed with the fields' mask.
   NE is STORE_FLAG_VALUE when true, which is -1 on m68k, and the sne
   emitted for it made these functions return 255 instead of 1.  */

typedef struct
{
  _Bool a : 1;
  _Bool b : 1;
  _Bool c : 1;
  _Bool d : 1;
  unsigned int e : 4;
} S;

int
test_00 (S *s)
{
  return s->b | s->c;
}

int
test_01 (S *s)
{
  return s->b | s->c | s->d;
}

/* { dg-final { scan-assembler-not {\tsne\M} } } */
