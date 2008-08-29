#ifndef INCLUDE__vic_h__200808291459
#define INCLUDE__vic_h__200808291459

void vic_set_isr(int irq, int priority, void (*isr)(void));

#endif
