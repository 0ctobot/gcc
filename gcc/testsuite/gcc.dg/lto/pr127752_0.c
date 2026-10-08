/* PR middle-end/127752 */
/* { dg-lto-do link } */
/* { dg-lto-options { { -O2 -flto -flto-partition=one -fdump-rtl-expand } } } */

struct status_block
{
  volatile unsigned int status;
};

struct device_regs
{
  struct status_block channels[2];
};

struct device_regs device = { { { 7 }, { 9 } } };

unsigned int read_status (void);

int
main (void)
{
  return read_status () != 9;
}

/* { dg-final { scan-ltrans-rtl-dump "mem/v" "expand" } } */
