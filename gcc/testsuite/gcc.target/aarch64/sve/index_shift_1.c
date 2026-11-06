/* A shift of 32 or more can't be an SVE index scale, even though the host's
   1 << shift could wrap around to the size of the element.  */
/* { dg-do assemble } */
/* { dg-options "-O3 -fno-strict-overflow -fno-strict-aliasing -fno-vect-cost-model -march=armv9-a+sve2 -msve-vector-bits=128" } */

#define TEST(NAME, TYPE, SHIFT)						\
  void									\
  NAME (TYPE *restrict p, long n)					\
  {									\
    for (long i = 0; i < n; i++)					\
      {									\
	p[(i << SHIFT) + 0] = i + 1;					\
	p[(i << SHIFT) + 1] = i + 2;					\
	p[(i << SHIFT) + 2] = i + 3;					\
	p[(i << SHIFT) + 3] = i + 4;					\
      }									\
  }

TEST (f8, unsigned char, 32)
TEST (f16, unsigned short, 32)
TEST (f32, unsigned int, 32)
TEST (f64, unsigned long, 32)
TEST (g8, unsigned char, 33)
TEST (g16, unsigned short, 33)
