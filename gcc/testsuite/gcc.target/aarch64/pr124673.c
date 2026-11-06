/* { dg-do compile } */
/* { dg-options "-O2 -mbig-endian" } */

/* The half of the constant that each word of a TImode comparison holds depends
   on endianness, and each word has to fit the CMP or CCMP that it ends up in.
   These used to ICE with an unrecognizable insn.  */

__int128 x;

#define TEST(NAME, VAL) \
  int NAME (void) { return __sync_bool_compare_and_swap (&x, VAL, 0); }

TEST (f1, 92)
TEST (f2, -92)
TEST (f3, 0x1000)
TEST (f4, 0x123456789)
TEST (f5, (__int128) 92 << 64)
TEST (f6, (__int128) 0x123456789 << 64)
TEST (f7, ((__int128) 0x123456789 << 64) | 0x987654321)
