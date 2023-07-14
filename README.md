# STM32 DMA Driver
Made by **Trippingcarpet** 

[![Build Status](https://app.travis-ci.com/microsoft/DynamicProto-JS.svg?branch=master)](https://github.com/RedSideDevils/dma_driver_for_stm32/releases/tag/V1.0.0)

## Features
- Compatible with all **STM32F7** series.
- Fast solution for your STM projects
- Easy integrate

## Installation
``` sh
1. git clone git@github.com:RedSideDevils/dma_driver_for_stm32.git.
2. move to directory of your STM project.
3. include "usart_dma_driver.h".
4. add dma_channel_handler(DMA, DMA_CHANNEL_IDX) functon to irqn handler.
```

## IOC file config
```md
1. Turn on USART. NOT TURN ON USART INTERRUPT!
2. Turn off "DMA on RX Error" and "Overrun".
3. Go to "DMA Settings" -> Select RX and TX pins -> Both pins Priority set medium -> Select channels.
4. Set TX pin mode to normal and RX pin mode to Circular.
5. Go to "NVIC Settings".
6. Turn on DMA channels interrupts.
```
## Usage Example
Using in ```INTERRUPT_EVERY_BYTE_STRING``` mode.
***main.c***
```c
#include "usart_dma_driver.h"

uint8_t loc_buff[256] = {0};
uint8_t new_string_flag;


void dma_proccess_string_callback(DMA_Init_Struct_t *dma_s, char *string)
{
	new_string_flag = 1;
	strcpy((char *)loc_buff, string);
}


void main(void)
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


	dma_config_recieve_mode(&dmaRecv, NORMAL_MODE);
    dma_config_delimeter(dmaRecv, '\n');
    
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
Using in ```NORMAL``` mode.
**main.c**
```c
#include "usart_dma_driver.h"

uint8_t loc_buff[256] = {0};

void main(void)
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
			
    dma_config_recieve_mode(&dmaRecv, NORMAL_MODE);
    
    dma_transmit_data(&dmaSend, (uint8_t *)Hello world, 11, 1000);
   
    // The incomming 2 bytes will store in loc_buff, if not data it will return with timeout
    dma_receive_data(&dmaRecv, loc_buff, 2, 1000);

    while(1);
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
