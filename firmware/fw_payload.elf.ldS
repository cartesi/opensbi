/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (c) 2019 Western Digital Corporation or its affiliates.
 *
 * Authors:
 *   Anup Patel <anup.patel@wdc.com>
 */

OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
	//ram    (rwx) : ORIGIN = 0x80000000, LENGTH = 64M
	opensbi(rwx) : ORIGIN = 0x80000000, LENGTH =  2M
	payload(rwx) : ORIGIN = 0x80200000, LENGTH = 62M /* must match FW_PAYLOAD_OFFSET */
	htif   (rwx) : ORIGIN = 0x40008000, LENGTH =  1k
};

SECTIONS
{
	#include "fw_base.ldS"
	.payload : {
		PROVIDE(_payload_start = .);
		*(.payload)
		. = ALIGN(8);
		PROVIDE(_payload_end = .);
	} > payload

	PROVIDE(_fw_reloc_end = .);

	.htif (NOLOAD): {
		*(.htif)
	} > htif
}
