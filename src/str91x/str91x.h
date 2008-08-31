#ifndef INCLUDED__gyros_str91x_h__200808312206
#define INCLUDED__gyros_str91x_h__200808312206

#include <stdint.h>

typedef volatile uint16_t reg16;
typedef volatile uint32_t reg32;

/* AHB peripherals */
#define SCU_P0_FMI          0x1
#define SCU_P0_FPQBC        0x2
#define SCU_P0_SRAM         0x8
#define SCU_P0_SRAM_ARBITER 0x10
#define SCU_P0_VIC          0x20
#define SCU_P0_EMI          0x40
#define SCU_P0_EMI_MEM_CLK  0x80
#define SCU_P0_DMA          0x100
#define SCU_P0_USB          0x200
#define SCU_P0_USB48M       0x400
#define SCU_P0_ENET         0x800
#define SCU_P0_PFQBC_AHB    0x1000

/* APB peripherals */
#define SCU_P1_TIM01 0x1
#define SCU_P1_TIM23 0x2
#define SCU_P1_MC    0x4
#define SCU_P1_UART0 0x8
#define SCU_P1_UART1 0x10
#define SCU_P1_UART2 0x20
#define SCU_P1_I2C0  0x40
#define SCU_P1_I2C1  0x80
#define SCU_P1_SSP0  0x100
#define SCU_P1_SSP1  0x200
#define SCU_P1_CAN   0x400
#define SCU_P1_ADC   0x800
#define SCU_P1_WDG   0x1000
#define SCU_P1_WIU   0x2000
#define SCU_P1_GPIO0 0x4000
#define SCU_P1_GPIO1 0x8000
#define SCU_P1_GPIO2 0x10000
#define SCU_P1_GPIO3 0x20000
#define SCU_P1_GPIO4 0x40000
#define SCU_P1_GPIO5 0x80000
#define SCU_P1_GPIO6 0x100000
#define SCU_P1_GPIO7 0x200000
#define SCU_P1_GPIO8 0x400000
#define SCU_P1_GPIO9 0x800000
#define SCU_P1_RTC   0x1000000

struct SCU_regs
{
    reg32 CLKCNTR;
    reg32 PLLCONF;
    reg32 SYSSTATUS;
    reg32 PWRMNG;
    reg32 ITCMSK;
    reg32 PCGR0;
    reg32 PCGR1;
    reg32 PRR0;
    reg32 PRR1;
    reg32 MGR0;
    reg32 MGR1;
    reg32 PECGR0;
    reg32 PECGR1;
    reg32 SCR0;
    reg32 SCR1;
    reg32 SCR2;
    uint32_t reserved1;
    reg32 GPIOOUT[8];
    reg32 GPIOIN[8];
    reg32 GPIOTYPE[10];
    reg32 GPIOEMI;
    reg32 WKUPSEL;
    uint32_t reserved2[2];
    reg32 GPIOANA;
};

struct TIM_regs
{
    reg16 IC1R;
    reg16 EMPTY1;
    reg16 IC2R;
    reg16 EMPTY2;
    reg16 OC1R;
    reg16 EMPTY3;
    reg16 OC2R;
    reg16 EMPTY4;
    reg16 CNTR;
    reg16 EMPTY5;
    reg16 CR1;
    reg16 EMPTY6;
    reg16 CR2;
    reg16 EMPTY7;
    reg16 SR;
    reg16 EMPTY8;
};

#define TIM_SRC_WDG        0
#define TIM_SRC_SW         1
#define TIM_SRC_ARMRX      2
#define TIM_SRC_ARMTX      3
#define TIM_SRC_TIM0       4
#define TIM_SRC_TIM1       5
#define TIM_SRC_TIM2       6
#define TIM_SRC_TIM3       7
#define TIM_SRC_USBHP      8
#define TIM_SRC_USBLP      9
#define TIM_SRC_SCU        10
#define TIM_SRC_ENET       11
#define TIM_SRC_DMA        12
#define TIM_SRC_CAN        13
#define TIM_SRC_MC         14
#define TIM_SRC_ADC        15
#define TIM_SRC_UART0      16
#define TIM_SRC_UART1      17
#define TIM_SRC_UART2      18
#define TIM_SRC_I2C0       19
#define TIM_SRC_I2C1       20
#define TIM_SRC_SSP0       21
#define TIM_SRC_SSP1       22
#define TIM_SRC_LVD        23
#define TIM_SRC_RTC        24
#define TIM_SRC_WIU        25
#define TIM_SRC_EXTIT0     26
#define TIM_SRC_EXTIT1     27
#define TIM_SRC_EXTIT2     28
#define TIM_SRC_EXTIT3     29
#define TIM_SRC_USBWU      30
#define TIM_SRC_PFQBC      31

struct VIC_regs
{
    reg32 ISR;
    reg32 FSR;
    reg32 RINTSR;
    reg32 INTSR;
    reg32 INTER;
    reg32 INTECR;
    reg32 SWINTR;
    reg32 SWINTCR;
    reg32 PER;
    reg32 EMPTY1[3];
    reg32 VAR;
    reg32 DVAR;
    reg32 EMPTY2[50];
    reg32 VAiR[16];
    reg32 EMPTY3[48];
    reg32 VCiR[16];
};

/*----------------------------------------------------------------------------*/

#define SCU          ((struct SCU_regs*)0x5c002000)
#define TIM(n)       ((struct TIM_regs*)(0x58002000 + 0x1000 * (n)))
#define VIC0         ((struct VIC_regs*)0xfffff000)
#define VIC1         ((struct VIC_regs*)0xfc000000)

#endif
