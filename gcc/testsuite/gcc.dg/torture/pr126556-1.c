/* PR rtl-optimization/126556 */
/* { dg-do compile } */
/* { dg-additional-options "-w" } */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef u8 v4u8 __attribute__((vector_size (4)));
typedef u8 fn_t (_Bool, u16, u32, void *);

void *sink;

_Bool
f (void)
{
  u8 v6;
  u32 init;
  v4u8 b;
  fn_t fn;

  b = (union { u32 u; v4u8 v; }) { init }.v;
  v6 = b[3];
  b[(signed char) v6 > 0] = v6;
  sink = &b;
  fn (f, 1098, 8038, sink);
}
