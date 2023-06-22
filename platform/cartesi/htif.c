/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2010-2020, The Regents of the University of California
 * (Regents).  All Rights Reserved.
 */

#include <sbi/riscv_locks.h>
#include <sbi/sbi_console.h>
#include <sbi/sbi_error.h>
#include <sbi/sbi_init.h>
#include <sbi/sbi_platform.h>
#include <sbi/sbi_system.h>
#include <sbi_utils/sys/htif.h>

#define HTIF_DEV_SYSTEM		0
#define HTIF_DEV_CONSOLE	1

#define HTIF_CONSOLE_CMD_GETC	0
#define HTIF_CONSOLE_CMD_PUTC	1

#define TOHOST_DEV(dev) ((uint64_t)(dev) << 56)
#define TOHOST_CMD(cmd) ((uint64_t)(cmd) << 56 >> 8)
#define TOHOST_DATA(cmd) ((uint64_t)(cmd) << 16 >> 16)
#define TOHOST_CMD_DATA(cmd_data) (cmd_data << 8 >> 8)
#define TOHOST_DEV_CMD_DATA(dev, cmd, data) \
  (TOHOST_DEV(dev) | TOHOST_CMD(cmd) | TOHOST_DATA(data))

#define FROMHOST_DEV(fromhost_value) ((uint64_t)(fromhost_value) >> 56)
#define FROMHOST_CMD(fromhost_value) ((uint64_t)(fromhost_value) << 8 >> 56)
#define FROMHOST_DATA(fromhost_value) ((uint64_t)(fromhost_value) << 16 >> 16)

typedef struct {
	uint64_t tohost;
	uint64_t fromhost;
	uint64_t iconsole;
	uint64_t iyield;
} htif_t;
volatile htif_t *htif = (void *)(0x40008000);

static void cartesi_htif_putc(char ch)
{
	htif->fromhost = 0;
	htif->tohost = TOHOST_DEV_CMD_DATA(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_PUTC, ch);
	(void)htif->fromhost;
}

static int cartesi_htif_getc(void)
{
	htif->fromhost = 0;
	htif->tohost = TOHOST_DEV_CMD_DATA(HTIF_DEV_CONSOLE, HTIF_CONSOLE_CMD_GETC, 0);
	return (int)(FROMHOST_DATA(htif->fromhost))-1;
}

static struct sbi_console_device cartesi_console = {
	.name = "htif",
	.console_putc = cartesi_htif_putc,
	.console_getc = cartesi_htif_getc
};

int cartesi_htif_serial_init()
{
	sbi_console_set_device(&cartesi_console);
	return 0;
}

static int cartesi_htif_system_reset_check(u32 type, u32 reason)
{
	return 1;
}

static void cartesi_htif_system_reset(u32 type, u32 reason)
{
	while (1) {
		htif->fromhost = 0;
		htif->tohost = 1;
	}
}

struct sbi_system_reset_device cartesi_reset = {
	.name = "htif",
	.system_reset_check = cartesi_htif_system_reset_check,
	.system_reset = cartesi_htif_system_reset
};

int cartesi_htif_system_reset_init()
{
	sbi_system_reset_add_device(&cartesi_reset);
	return 0;
}

uint64_t sbi_system_yield(uint64_t cmd_data)
{
	htif->fromhost = 0;
	htif->tohost = TOHOST_DEV(2) | TOHOST_CMD_DATA(cmd_data);
	return htif->fromhost;
}
