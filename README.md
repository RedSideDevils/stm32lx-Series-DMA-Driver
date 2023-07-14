# STM32 DMA Driver
Made by Trippingcarpet 
## Installation
Driver Download
``` sh
1. git clone git@github.com:RedSideDevils/dma_driver_for_stm32.git.
2. move to directory of your stm project.
3. include "usart_dma_driver.h".
4. add dma_channel_handler(DMA, DMA_CHANNEL_IDX) functon to irqn handler.
```
## Usage Example
```c
#include "usart_dma_driver.h"

uint8_t loc_buff[256] = {0};

void myMain(void)
{
	DMA_Init_Struct_t dmaSend = {
			.dma = DMA1,
			.dma_channel_tx = DMA1_Channel4,
			.usart = USART2,
			.ch_idx = DMA_CHANNEL_4,
			.dma_irqn_handler = DMA1_Channel4_5_6_7_IRQn};


	DMA_Init_Struct_t dmaRecv = {
			.dma = DMA1,
			.dma_channel_rx = DMA1_Channel5,
			.usart = USART2,
			.ch_idx = DMA_CHANNEL_5,
			.dma_irqn_handler = DMA1_Channel4_5_6_7_IRQn};


	dma_receive_data(&dmaRecv, loc_buff, 5, 20000);
	dma_transmit_data(&dmaSend, loc_buff, 5, 1000);

	dma_receive_data(&dmaRecv, loc_buff, 5, 20000);
	dma_transmit_data(&dmaSend, loc_buff, 5, 1000);

	while(1);
}
