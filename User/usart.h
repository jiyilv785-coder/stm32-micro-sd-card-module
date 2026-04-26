#ifndef __USART_H
#define __USART_H
#include <stdio.h>
#include "stdint.h"

void USART1_Init(uint32_t baud);
int fputc(int ch, FILE* f);  // 用于printf重定向

#endif
