/**************************************************************************
 * Copyright (c) 2002-2009, Tobias Ringström <tobias@ringis.se>
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
#include <stddef.h>

void __reset_handler(void) __attribute__((naked));

void __trap(void);
void __hwinit(void);

int main(int argc, char *argv[]);

extern unsigned long __stack_top;
extern unsigned long __data_start;
extern unsigned long __data_end;
extern unsigned long __data_load;
extern unsigned long __bss_start;
extern unsigned long __bss_end;

const void *vectors[71] __attribute__((section(".romvectors"))) = {
    &__stack_top,
    __reset_handler,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
    __trap,
};

void
__trap(void)
{
    for (;;)
        ;
}

void
__reset_handler(void)
{
    unsigned long *dst;
    const unsigned long *src;

    __hwinit();

    for (dst = &__data_start, src = &__data_load; dst != &__data_end; ++dst)
        *dst = *src++;

    for (dst = &__bss_start; dst != &__bss_end; ++dst)
        *dst = 0;

    main(0, NULL);

    for (;;)
        ;
}
