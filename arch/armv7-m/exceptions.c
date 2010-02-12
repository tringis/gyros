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
void
gyros__arch_pendsv_handler(void) __attribute__((__naked__));

void
gyros__arch_pendsv_handler(void)
{
    __asm__ __volatile__(
        "mrs     r3, psp\n"
        "stmdb   r3, {r4-r11}\n"         /* Write registers to task stack. */
        "ldr     r0, =gyros\n"           /* r0 = &gyros.current */
        "ldr     r1, [r0]\n"             /* r1 = gyros.current */
        "str     r3, [r1]\n"             /* gyros.current->context.sp = PSP */

        "ldr     r2, [r0, #4]\n"         /* r2 = gyros.running.next */
        "sub     r2, r2, #4\n"           /* r2 = TASK(gyros.running.next) */
        "str     r2, [r0]\n"             /* gyros.current = r2 */

        "ldr     r3, [r2]\n"             /* r3 = SP of new task */
        "ldmdb   r3, {r4-r11}\n"         /* Load regs for new task */
        "msr     psp, r3\n"              /* Set PSP to SP of new task */
        "bx      lr\n"
        ::: "memory");
}
