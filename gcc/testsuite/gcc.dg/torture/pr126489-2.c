/* { dg-do compile } */
/* { dg-additional-options "-Wno-psabi" } */

/* A shift count of all ones must not wrap the lane width to zero.  */

typedef unsigned long long v2du __attribute__ ((vector_size (16)));

v2du
f (v2du x)
{
  v2du s = { -1ULL, -1ULL };
  v2du m = { 0x11, 0x11 };
  v2du n = { 15, 15 };

  return ((x >> s) & m) * n;
}
