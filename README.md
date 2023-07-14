# STM32 DMA Driver
Made by **Trippingcarpet** 
## Installation
Driver Download
``` sh
1. git clone git@github.com:RedSideDevils/dma_driver_for_stm32.git.
2. move to directory of your stm project.
3. include "usart_dma_driver.h".
4. add dma_channel_handler(DMA, DMA_CHANNEL_IDX) functon to irqn handler.
```
## Usage Example
**main.c**
```c
#include "myMain.h"

uint8_t loc_buff[256] = {0};
uint8_t new_string_flag;

void dma_proccess_byte_callback(DMA_Init_Struct_t *dma_s,char byte)
{

}

void dma_proccess_string_callback(DMA_Init_Struct_t *dma_s, char *string)
{
	new_string_flag = 1;
	strcpy((char *)loc_buff, string);
}


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


	dma_config_recieve_mode(&dmaRecv, INTERRUPT_EVERY_BYTE_STRING);
	dma_config_delimeter(&dmaRecv, '\n');

	uint32_t d_size = 0;

	while(1)
	{
		if(new_string_flag == 1)
		{
			new_string_flag = 0;
			d_size = strlen((char *)loc_buff);
			dma_transmit_data(&dmaSend, loc_buff, d_size, 1000);
		}
		dma_receive_loop(&dmaRecv);
	}
}
```
**stm32l0xx_it.c**
```c
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 0 */
  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 0 */

  /* USER CODE BEGIN DMA1_Channel4_5_6_7_IRQn 1 */
	dma_channel_handler(DMA1, DMA_CHANNEL_4);
	dma_channel_handler(DMA1, DMA_CHANNEL_5);

  /* USER CODE END DMA1_Channel4_5_6_7_IRQn 1 */
}
```
