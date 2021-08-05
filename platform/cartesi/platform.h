#ifndef _CARTESI_PLATFORM_H_
#define _CARTESI_PLATFORM_H_

#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_system.h>
#include <sbi/sbi_trap.h>
#include <sbi/sbi_console.h>

extern struct sbi_console_device cartesi_console;
extern struct sbi_system_reset_device cartesi_reset;

int platform_ext_provider(long extid, long funcid,
		const struct sbi_trap_regs *regs,
		unsigned long *out_value,
		struct sbi_trap_info *out_trap);

enum sbi_ext_cartesi_fid {
	SBI_EXT_CARTESI_YIELD = 9,
};

#endif /* _CARTESI_PLATFORM_H_ */
