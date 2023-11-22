/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */

#include <sbi/riscv_asm.h>
#include <sbi/riscv_encoding.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_const.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/fdt/fdt_domain.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_helper.h>
#include <sbi_utils/fdt/fdt_pmu.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/reset/fdt_reset.h>
#include <sbi_utils/sys/htif.h>
#include <sbi_utils/timer/fdt_timer.h>


int cartesi_htif_serial_init();
int cartesi_htif_system_reset_init();

static int platform_final_init(bool cold_boot)
{
	void *fdt;

	if (cold_boot)
		fdt_reset_init();

	if (!cold_boot)
		return 0;

	fdt = fdt_get_address();

	fdt_cpu_fixup(fdt);
	fdt_fixups(fdt);
	fdt_domain_fixup(fdt);

	return cartesi_htif_system_reset_init();
}

static u64 platform_tlbr_flush_limit(void)
{
	return SBI_PLATFORM_TLB_RANGE_FLUSH_LIMIT_DEFAULT;
}

static int platform_pmu_init(void)
{
	return fdt_pmu_setup(fdt_get_address());
}

const struct sbi_platform_operations platform_ops = {
	.final_init		= platform_final_init,
	.console_init		= cartesi_htif_serial_init,
	.timer_init		= fdt_timer_init,
	.timer_exit		= fdt_timer_exit,
	.ipi_init		= fdt_ipi_init,
	.ipi_exit		= fdt_ipi_exit,
	.get_tlbr_flush_limit	= platform_tlbr_flush_limit,
	.pmu_init		= platform_pmu_init,
};

const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x00),
	.name			= "cartesi",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= 1,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.heap_size		= SBI_PLATFORM_DEFAULT_HEAP_SIZE(0),
	.platform_ops_addr	= (unsigned long)&platform_ops
};
