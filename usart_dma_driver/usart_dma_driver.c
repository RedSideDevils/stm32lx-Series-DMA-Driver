#include "usart_dma_driver.h"

uint8_t ch_1_tc_flag = 0;
uint8_t ch_1_hc_flag = 0;

uint8_t ch_2_tc_flag = 0;
uint8_t ch_2_hc_flag = 0;

uint8_t ch_3_tc_flag = 0;
uint8_t ch_3_hc_flag = 0;

uint8_t ch_4_tc_flag = 0;
uint8_t ch_4_hc_flag = 0;

uint8_t ch_5_tc_flag = 0;
uint8_t ch_5_hc_flag = 0;

uint8_t ch_6_tc_flag = 0;
uint8_t ch_6_hc_flag = 0;

uint8_t ch_7_tc_flag = 0;
uint8_t ch_7_hc_flag = 0;

/* Private */
uint8_t get_tc_flag(dma_handler_channel_index_t ch_idx);
void reset_tc_flag(dma_handler_channel_index_t ch_idx);

void enable_rx_dma_channel(DMA_Init_Struct_t *dma_s);
void disable_rx_dma_channel(DMA_Init_Struct_t *dma_s);

void enable_tx_dma_channel(DMA_Init_Struct_t *dma_s);
void disable_tx_dma_channel(DMA_Init_Struct_t *dma_s);

void dma_init_rx_usart(DMA_Init_Struct_t *dma_s);
void dma_deinit_rx_usart(DMA_Init_Struct_t *dma_s);

void dma_init_tx_usart(DMA_Init_Struct_t *dma_s);
void dma_deinit_tx_usart(DMA_Init_Struct_t *dma_s);
/* end */

/* Private */

uint8_t get_tc_flag(dma_handler_channel_index_t ch_idx)
{
	switch(ch_idx)
	{
	case DMA_CHANNEL_1:
		return ch_1_tc_flag;
		break;
	case DMA_CHANNEL_2:
		return ch_2_tc_flag;
		break;
	case DMA_CHANNEL_3:
		return ch_3_tc_flag;
		break;
	case DMA_CHANNEL_4:
		return ch_4_tc_flag;
		break;
	case DMA_CHANNEL_5:
		return ch_5_tc_flag;
		break;
	case DMA_CHANNEL_6:
		return ch_6_tc_flag;
		break;
	case DMA_CHANNEL_7:
		return ch_7_tc_flag;
		break;
	}
	return 0;
}


void reset_tc_flag(dma_handler_channel_index_t ch_idx)
{
	switch(ch_idx)
	{
	case DMA_CHANNEL_1:
		ch_1_tc_flag = 0;
		break;
	case DMA_CHANNEL_2:
		ch_2_tc_flag = 0;
		break;
	case DMA_CHANNEL_3:
		ch_3_tc_flag = 0;
		break;
	case DMA_CHANNEL_4:
		ch_4_tc_flag = 0;
		break;
	case DMA_CHANNEL_5:
		ch_5_tc_flag = 0;
		break;
	case DMA_CHANNEL_6:
		ch_6_tc_flag = 0;
		break;
	case DMA_CHANNEL_7:
		ch_7_tc_flag = 0;
		break;
	}

}


void enable_rx_dma_channel(DMA_Init_Struct_t *dma_s)
{
	dma_s->dma_channel_rx->CCR |= DMA_CCR_EN;
}


void disable_rx_dma_channel(DMA_Init_Struct_t *dma_s)
{
	dma_s->dma_channel_rx->CCR &= ~DMA_CCR_EN;
}


void enable_tx_dma_channel(DMA_Init_Struct_t *dma_s)
{
	dma_s->dma_channel_tx->CCR |= DMA_CCR_EN;
}


void disable_tx_dma_channel(DMA_Init_Struct_t *dma_s)
{
	dma_s->dma_channel_tx->CCR &= ~DMA_CCR_EN;
}
/* end */

/* ### USAGE OF DMA RECEIVE ###
 * call dma_init_rx_usart
 * call test_dma_receive_data
 * call dma_deinit_rx_usart
 * ############################
*/
void dma_init_rx_usart(DMA_Init_Struct_t *dma_s)
{
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Configure USART
    // Enable receiver
    dma_s->usart->CR1 |= USART_CR1_RE;

    // Configure DMA Channel
    // Enable memory increment mode
    dma_s->dma_channel_rx->CCR |= DMA_CCR_MINC;

    // Set peripheral address to USART data register
    dma_s->dma_channel_rx->CPAR = (uint32_t)&dma_s->usart->RDR;
}


void dma_deinit_rx_usart(DMA_Init_Struct_t *dma_s)
{
    // Disable DMA Channel
	disable_rx_dma_channel(dma_s);

	// Disable DMA Channel  transfer complete interrupt
	dma_s->dma_channel_rx->CCR &= ~DMA_CCR_TCIE;

	// Reset local DMA transfer complete flag
	reset_tc_flag(dma_s->ch_idx);
}


dma_status_t dma_receive_data(DMA_Init_Struct_t *dma_s, uint8_t *buff, uint32_t size, uint32_t timeout)
{
	dma_init_rx_usart(dma_s);

	dma_status_t status = DMA_STATUS_OK;

	disable_rx_dma_channel(dma_s);

	reset_tc_flag(dma_s->ch_idx);

	// Configure Buffers
	dma_s->dma_channel_rx->CNDTR = size;
	dma_s->dma_channel_rx->CMAR = (uint32_t)buff;

	enable_rx_dma_channel(dma_s);

	uint32_t start_time = HAL_GetTick() + timeout;

	// Enable transmit complete interrupt
	dma_s->dma_channel_rx->CCR |= DMA_CCR_TCIE;

	// Enable USART receive mode
	dma_s->usart->CR3 |= USART_CR3_DMAR;

	while(get_tc_flag(dma_s->ch_idx) != 1)
	{
		if(HAL_GetTick() > start_time)
		{
			status = DMA_STATUS_ERROR;
			break;
		}
	}

	// Reset local DMA interrupt flag
	reset_tc_flag(dma_s->ch_idx);

	// Disable transmit complete interrupt
	dma_s->dma_channel_rx->CCR &= ~DMA_CCR_TCIE;

	disable_rx_dma_channel(dma_s);

	// Disable uart receive mode
	dma_s->usart->CR3 &= ~USART_CR3_DMAR;
	dma_deinit_rx_usart(dma_s);
	return status;
}

/* ### USAGE OF DMA TRANSMIT ###
 * call dma_init_tx_usart
 * call dma_transmit_data
 * call test_dma_deinit_tx_usart
 * #############################
*/
void dma_init_tx_usart(DMA_Init_Struct_t *dma_s)
{
    // Enable USART and DMA1 clocks
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    // Configure USART
    // Enable transmitter
    dma_s->usart->CR1 = USART_CR1_TE;

    // Configure DMA Channel (USART)
    dma_s->dma_channel_tx->CCR = DMA_CCR_MINC | DMA_CCR_DIR; // Enable memory increment mode and set direction to memory-to-peripheral
    dma_s->dma_channel_tx->CPAR = (uint32_t)&dma_s->usart->TDR; // Set peripheral
    // Turn on USART
    dma_s->usart->CR1 |= USART_CR1_UE;
}


void dma_deinit_tx_usart(DMA_Init_Struct_t *dma_s)
{
    // Disable DMA Channel
	disable_tx_dma_channel(dma_s);

    // Disable DMA Channel  transfer complete interrupt
    dma_s->dma_channel_tx->CCR &= ~DMA_CCR_TCIE;

    // Clear transfer complete flag
    reset_tc_flag(dma_s->ch_idx);
}


dma_status_t dma_transmit_data(DMA_Init_Struct_t *dma_s, const uint8_t *data, uint32_t data_size, uint32_t timeout)
{
	dma_init_tx_usart(dma_s);

	dma_status_t status = DMA_STATUS_OK;

    // Disable DMA Channel
	disable_tx_dma_channel(dma_s);

	// Clear transfer complete flag
	reset_tc_flag(dma_s->ch_idx);

	// Configure DMA Channel
	dma_s->dma_channel_tx->CNDTR = data_size; // Set number of data to be transferred
	dma_s->dma_channel_tx->CMAR = (uint32_t)data; // Set memory address

    // Enable DMA Channel
	enable_tx_dma_channel(dma_s);

	uint32_t start_time = HAL_GetTick() + timeout;

	dma_s->dma_channel_tx->CCR |= DMA_CCR_TCIE;
	// Enable USART DMA request
    dma_s->usart->CR3 |= USART_CR3_DMAT;

    // Wait for DMA transfer complete
    while (get_tc_flag(dma_s->ch_idx) != 1)
    {
    	if(HAL_GetTick() > start_time)
    	{
    		status = DMA_STATUS_ERROR;
    		break;
    	}
    }

	dma_s->dma_channel_tx->CCR &= ~DMA_CCR_TCIE;

    // Clear transfer complete flag
    reset_tc_flag(dma_s->ch_idx);

    // Disable DMA Channel
    disable_tx_dma_channel(dma_s);

    // Disable USART DMA request
    dma_s->usart->CR3 &= ~USART_CR3_DMAT;

	dma_deinit_tx_usart(dma_s);
    return status;
}


/* call in st32l0xx_it.c DMA channel interrupt handler */
void dma_channel_handler(DMA_TypeDef *dma, dma_handler_channel_index_t ch_idx)
{
	switch(ch_idx)
	{
	case DMA_CHANNEL_1:
		if ((dma->ISR & DMA_ISR_HTIF1) == DMA_ISR_HTIF1)
		{
			dma->IFCR |= DMA_IFCR_CGIF1;
			ch_1_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_1);
		}

		if ((dma->ISR & DMA_ISR_TCIF1) == DMA_ISR_TCIF1)
		{
			dma->IFCR |= DMA_IFCR_CTCIF1;
			ch_1_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_1);
		}
		break;

	case DMA_CHANNEL_2:
		if ((dma->ISR & DMA_ISR_HTIF2) == DMA_ISR_HTIF2)
		{
			dma->IFCR |= DMA_IFCR_CHTIF2;
			ch_2_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_2);
		}

		if ((dma->ISR & DMA_ISR_TCIF2) == DMA_ISR_TCIF2)
		{
			dma->IFCR |= DMA_IFCR_CTCIF2;
			ch_2_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_2);
		}
		break;


	case DMA_CHANNEL_3:
		if ((dma->ISR & DMA_ISR_HTIF3) == DMA_ISR_HTIF3)
		{
			dma->IFCR |= DMA_IFCR_CHTIF3;
			ch_3_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_3);
		}

		if ((dma->ISR & DMA_ISR_TCIF3) == DMA_ISR_TCIF3)
		{
			dma->IFCR |= DMA_IFCR_CTCIF3;
			ch_3_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_3);
		}
		break;


	case DMA_CHANNEL_4:
		if ((dma->ISR & DMA_ISR_HTIF4) == DMA_ISR_HTIF4)
		{
			dma->IFCR |= DMA_IFCR_CHTIF4;
			ch_4_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_4);
		}

		if ((dma->ISR & DMA_ISR_TCIF4) == DMA_ISR_TCIF4)
		{
			dma->IFCR |= DMA_IFCR_CTCIF4;
			ch_4_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_4);
		}
		break;


	case DMA_CHANNEL_5:
		if ((dma->ISR & DMA_ISR_HTIF5) == DMA_ISR_HTIF5)
		{
			dma->IFCR |= DMA_IFCR_CHTIF5;
			ch_5_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_5);
		}

		if ((dma->ISR & DMA_ISR_TCIF5) == DMA_ISR_TCIF5)
		{
			dma->IFCR |= DMA_IFCR_CTCIF5;
			ch_5_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_5);
		}
		break;


	case DMA_CHANNEL_6:
		if ((dma->ISR & DMA_ISR_HTIF6) == DMA_ISR_HTIF6)
		{
			dma->IFCR |= DMA_IFCR_CHTIF6;
			ch_6_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_6);
		}

		if ((dma->ISR & DMA_ISR_TCIF6) == DMA_ISR_TCIF6)
		{
			dma->IFCR |= DMA_IFCR_CTCIF6;
			ch_6_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_6);
		}
		break;


	case DMA_CHANNEL_7:
		if ((dma->ISR & DMA_ISR_HTIF7) == DMA_ISR_HTIF7)
		{
			dma->IFCR |= DMA_IFCR_CHTIF7;
			ch_7_hc_flag = 1;
			dma_half_transmit_callback(DMA_CHANNEL_7);
		}

		if ((dma->ISR & DMA_ISR_TCIF7) == DMA_ISR_TCIF7)
		{
			dma->IFCR |= DMA_IFCR_CTCIF7;
			ch_7_tc_flag = 1;
			dma_complete_transmit_callback(DMA_CHANNEL_7);
		}
		break;
	default:
		break;
	}
}

/* TEST */

/* END */

__weak void dma_half_transmit_callback(dma_handler_channel_index_t ch_idx);
__weak void dma_complete_transmit_callback(dma_handler_channel_index_t ch_idx);

