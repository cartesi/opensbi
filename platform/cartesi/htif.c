#include <platform.h>
#include <sbi/sbi_ecall_interface.h>

#define TOHOST_DEV(dev) ((uint64_t)(dev) << 56)
#define TOHOST_CMD(cmd) ((uint64_t)(cmd) << 56 >> 8)
#define TOHOST_DATA(cmd) ((uint64_t)(cmd) << 16 >> 16)
#define TOHOST_CMD_DATA(cmd_data) (cmd_data << 8 >> 8)
#define TOHOST_DEV_CMD_DATA(dev, cmd, data) \
  (TOHOST_DEV(dev) | TOHOST_CMD(cmd) | TOHOST_DATA(data))

#define FROMHOST_DEV(fromhost_value) ((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

struct htif {
	uint64_t tohost;
	uint64_t fromhost;
	uint64_t iconsole;
	uint64_t iyield;
};
volatile struct htif htif __attribute__((section (".htif")));

static void cartesi_htif_putc(char x)
{
	htif.fromhost = 0;
	htif.tohost = TOHOST_DEV_CMD_DATA(1,1,x);
	(void)htif.fromhost;
}

static int cartesi_htif_getc(void)
{
	htif.fromhost = 0;
	htif.tohost = TOHOST_DEV_CMD_DATA(1,0,0);
	return (int)(FROMHOST_DATA(htif.fromhost))-1;
}

struct sbi_console_device cartesi_console = {
	.name = "htif",
	.console_putc = cartesi_htif_putc,
	.console_getc = cartesi_htif_getc
};
/* -------------------------------------------------------------------------- */
static int cartesi_htif_system_reset_check(u32 type, u32 reason)
{
	return 1;
}

static void cartesi_htif_system_reset(u32 type, u32 reason)
{
	while (1) {
		htif.fromhost = 0;
		htif.tohost = 1;
	}
}

struct sbi_system_reset_device cartesi_reset = {
	.name = "htif",
	.system_reset_check = cartesi_htif_system_reset_check,
	.system_reset = cartesi_htif_system_reset
};
/* -------------------------------------------------------------------------- */
uint64_t cartesi_htif_yield(uint64_t cmd_data)
{
  htif.fromhost = 0;
  htif.tohost = TOHOST_DEV(2) | TOHOST_CMD_DATA(cmd_data);
  return htif.fromhost;
}

int platform_ext_provider(long extid, long funcid,
		const struct sbi_trap_regs *regs,
		unsigned long *out_value,
		struct sbi_trap_info *out_trap)
{
	int ret = 0;

	switch (funcid) {
	case SBI_EXT_CARTESI_YIELD:
		*out_value = cartesi_htif_yield((u64)regs->a0);
		break;
	default:
		sbi_printf("Unsupported vendor sbi call : %ld\n", funcid);
		break;
	}

	return ret;
}
