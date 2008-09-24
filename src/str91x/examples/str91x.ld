/**************************************************************************
 * Copyright (c) 2002-2008, Tobias Ringström <tobias@ringis.se>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **************************************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(gyros__reset_handler)

MEMORY 
{
	rom (rx)	: ORIGIN = 0x00000000, LENGTH = 512K
	ram (rw)	: ORIGIN = 0x04000000, LENGTH = 96K
}

SECTIONS
{
	PROVIDE(__hwinit_stack_top = 0x04000000 + 32*1024);
	PROVIDE(__stack_top = 0x04000000 + 96*1024);

	.text.vectors :
	{
		*(.text.vectors);
		. = ALIGN(4);
	} >rom

	.text :
	{
		PROVIDE(__text_start = .);
		*(.text);
		*(.rodata*);
		*(.glue_7t)
		*(.glue_7)
		. = ALIGN(32);
	} >rom
	PROVIDE(__text_size = SIZEOF(.text));

	.text.ram :
	{
		PROVIDE(__text_ram_start = .);
		*(.text.ram);
		. = ALIGN(32);
	} >ram AT>rom
	PROVIDE(__text_ram_size = SIZEOF(.text.ram));
	PROVIDE(__text_ram_load = LOADADDR(.text.ram));

	.data :
	{
		PROVIDE(__data_start = .);
		*(.data);
		*(.sdata);
		. = ALIGN(32);
	} >ram AT>rom
	PROVIDE(__data_size = SIZEOF(.data));
	PROVIDE(__data_load = LOADADDR(.data));

	.bss :
	{
		PROVIDE(__bss_start = .);
		*(.sbss);
		*(.scommon);
		*(.bss);
		*(COMMON);
		. = ALIGN(4);
	} >ram
	PROVIDE(__bss_size = SIZEOF(.bss));
}
