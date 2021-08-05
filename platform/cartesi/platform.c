/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 */
#include <platform.h>
#include <sbi/sbi_platform.h>

#include <sbi_utils/fdt/fdt_domain.h>
#include <sbi_utils/fdt/fdt_fixup.h>
#include <sbi_utils/fdt/fdt_pmu.h>
#include <sbi_utils/irqchip/fdt_irqchip.h>
#include <sbi_utils/timer/fdt_timer.h>
#include <sbi_utils/ipi/fdt_ipi.h>
#include <sbi_utils/reset/fdt_reset.h>

static int platform_early_init(bool cold_boot)
{
	if (cold_boot) {
		sbi_system_reset_set_device(&cartesi_reset);
		return fdt_reset_init();
	}
	return 0;
}

static int platform_final_init(bool cold_boot)
{
	void *fdt;

	if (!cold_boot)
		return 0;

	fdt = sbi_scratch_thishart_arg1_ptr();
	fdt_cpu_fixup(fdt);
	fdt_fixups(fdt);
	fdt_domain_fixup(fdt);

	return 0;
}

static int platform_domains_init(void)
{
	return fdt_domains_populate(sbi_scratch_thishart_arg1_ptr());
}

static int platform_pmu_init(void)
{
	return fdt_pmu_setup(sbi_scratch_thishart_arg1_ptr());
}

static int cartesi_console_init(void) {
	sbi_console_set_device(&cartesi_console);
	return 0;
}

const struct sbi_platform_operations platform_ops = {
	.early_init		= platform_early_init,
	.final_init		= platform_final_init,
	.early_exit		= 0,
	.final_exit		= 0,
	.domains_init		= platform_domains_init,
	.console_init		= cartesi_console_init,
	.irqchip_init		= fdt_irqchip_init,
	.irqchip_exit		= fdt_irqchip_exit,
	.ipi_init		= fdt_ipi_init,
	.ipi_exit		= fdt_ipi_exit,
	.pmu_init		= platform_pmu_init,
	.timer_init		= fdt_timer_init,
	.timer_exit		= fdt_timer_exit,
	.vendor_ext_provider	= platform_ext_provider,
};
const struct sbi_platform platform = {
	.opensbi_version	= OPENSBI_VERSION,
	.platform_version	= SBI_PLATFORM_VERSION(0x0, 0x00),
	.name			= "Cartesi",
	.features		= SBI_PLATFORM_DEFAULT_FEATURES,
	.hart_count		= 1,
	.hart_stack_size	= SBI_PLATFORM_DEFAULT_HART_STACK_SIZE,
	.platform_ops_addr	= (unsigned long)&platform_ops
};
