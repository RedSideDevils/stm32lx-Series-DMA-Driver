/*
 * usart_dma_driver.h
 *
 *  Created on: Jul 12, 2023
 *      Author: armen
 */

#ifndef MYMAIN_LIB_USART_DMA_DRIVER_USART_DMA_DRIVER_H_
#define MYMAIN_LIB_USART_DMA_DRIVER_USART_DMA_DRIVER_H_

#include "main.h"
#include "string.h"


typedef enum
{
	DMA_CHANNEL_1,
	DMA_CHANNEL_2,
	DMA_CHANNEL_3,
	DMA_CHANNEL_4,
	DMA_CHANNEL_5,
	DMA_CHANNEL_6,
	DMA_CHANNEL_7
} dma_handler_channel_index_t;


typedef enum
{
	NORMAL_MODE,
	INTERRUPT_EVERY_BYTE_STRING,
} dma_working_modes_t;


typedef struct
{
	DMA_TypeDef *dma;
	USART_TypeDef *usart;
	DMA_Channel_TypeDef *dma_channel_tx;
	DMA_Channel_TypeDef *dma_channel_rx;
	int dma_irqn_handler;
	dma_handler_channel_index_t ch_idx;
	dma_working_modes_t w_mode;

	/* DO NOT CHANGE! */

	uint8_t dma_local_buffer[256];
	uint8_t dma_local_buffer_ptr;
	char delimeter;

	/* END */
} DMA_Init_Struct_t;


typedef enum
{
	DMA_STATUS_OK,
	DMA_STATUS_ERROR,
	DMA_STATUS_TIMEOUT
} dma_status_t;


/* RECEIVE */
dma_status_t dma_receive_data(DMA_Init_Struct_t *dma_s, uint8_t *buff, uint32_t size, uint32_t timeout);
void dma_config_recieve_mode(DMA_Init_Struct_t *dma_s, dma_working_modes_t w_mode);
void dma_config_delimeter(DMA_Init_Struct_t *dma_s, char delimeter);
void dma_receive_loop(DMA_Init_Struct_t *dma_s);

/* TRANSMIT */
dma_status_t dma_transmit_data(DMA_Init_Struct_t *dma_s, const uint8_t *data, uint32_t data_size, uint32_t timeout);

// ==== CALLBACKS ====
void dma_channel_handler(DMA_TypeDef *dma, dma_handler_channel_index_t ch_idx);
void dma_half_transmit_callback(dma_handler_channel_index_t ch_idx);
void dma_complete_transmit_callback(dma_handler_channel_index_t ch_idx);

void dma_proccess_byte_callback(DMA_Init_Struct_t *dma_s,char byte);
void dma_proccess_string_callback(DMA_Init_Struct_t *dma_s, char *string);

#endif /* MYMAIN_LIB_USART_DMA_DRIVER_USART_DMA_DRIVER_H_ */
