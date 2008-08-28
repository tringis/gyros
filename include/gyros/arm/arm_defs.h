#ifndef INCLUDED__armdefs_h__200203211323
#define INCLUDED__armdefs_h__200203211323

/*--------------------------*/
/* ARM Mode and Status Bits */
/*--------------------------*/

#define ARM_MODE_USER       0x10
#define ARM_MODE_FIQ        0x11
#define ARM_MODE_IRQ        0x12
#define ARM_MODE_SVC        0x13
#define ARM_MODE_ABORT      0x17
#define ARM_MODE_UNDEF      0x1B
#define ARM_MODE_SYS        0x1F

#define ARM_IRQ_BIT         0x80
#define ARM_FIQ_BIT         0x40
#define ARM_THUMB_BIT       0x20

#endif
