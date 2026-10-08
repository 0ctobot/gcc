struct status_block
{
  volatile unsigned int status;
};

struct device_regs
{
  struct status_block channels[2];
};

extern struct device_regs device;

unsigned int
read_status (void)
{
  return device.channels[1].status;
}
