/*_____ I N C L U D E S ____________________________________________________*/

#include "project_config.h"
#include "SPI_Flash.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/
/*_____ D E F I N I T I O N S ______________________________________________*/

volatile uint32_t PDMA_FLASH_TX_INT_Flag = 0;
volatile uint32_t PDMA_FLASH_RX_INT_Flag = 0;

/*_____ M A C R O S ________________________________________________________*/
/*_____ F U N C T I O N S __________________________________________________*/

void SpiFlash_Set_CS_High(void)
{
	
	#if defined (ENABLE_MANUAL_SS)	
	SPI_FLASH_CS_HIGH;
	#elif defined (ENABLE_AUTO_SS)
    SPI_SET_SS_HIGH(SPI_FLASH_PORT);
	#endif
				
}

void SpiFlash_Set_CS_Low(void)
{	
	#if defined (ENABLE_MANUAL_SS)	
	SPI_FLASH_CS_LOW;
	#elif defined (ENABLE_AUTO_SS)
    SPI_SET_SS_LOW(SPI_FLASH_PORT);
	#endif
				
}


void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();

	// printf("status = 0x%8X\r\n",status);

    if(status & 0x1) {  /* CH0 */
        if(PDMA_GET_CH_INT_STS(0) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(0, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x2) {  /* CH1 */
        if(PDMA_GET_CH_INT_STS(1) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(1, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x4) {  /* CH2 */
        if(PDMA_GET_CH_INT_STS(2) & 0x2)
        {
			PDMA_FLASH_TX_INT_Flag = 1; 			
		}
        PDMA_CLR_CH_INT_FLAG(2, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x8) {  /* CH3 */
        if(PDMA_GET_CH_INT_STS(3) & 0x2)
        {
			PDMA_FLASH_RX_INT_Flag = 1; 					
		}
        PDMA_CLR_CH_INT_FLAG(3, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x10) {  /* CH4 */
        if(PDMA_GET_CH_INT_STS(4) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(4, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x20) {  /* CH5 */
        if(PDMA_GET_CH_INT_STS(5) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(5, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x40) {  /* CH6 */
        if(PDMA_GET_CH_INT_STS(6) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(6, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x80) {  /* CH7 */
        if(PDMA_GET_CH_INT_STS(7) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(7, PDMA_ISR_BLKD_IF_Msk);
    } else if(status & 0x100) {  /* CH8 */
        if(PDMA_GET_CH_INT_STS(8) & 0x2)
        {
			
		}
        PDMA_CLR_CH_INT_FLAG(8, PDMA_ISR_BLKD_IF_Msk);
    } else
        printf("unknown interrupt !!\n");

	
    /* Get PDMA Block transfer down interrupt status */
    // if(PDMA_GET_CH_INT_STS(SPI_FLASH_TX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)
    // {
    //     /* Clear PDMA Block transfer down interrupt flag */   
    //     PDMA_CLR_CH_INT_FLAG(SPI_FLASH_TX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);   
        
    //     /* Handle PDMA block transfer done interrupt event */
    //     //insert process
              
	// 	PDMA_FLASH_TX_INT_Flag = 1;     
    // }

    // if(PDMA_GET_CH_INT_STS(SPI_FLASH_RX_DMA_CH) & PDMA_ISR_BLKD_IF_Msk)
    // {
    //     /* Clear PDMA Block transfer down interrupt flag */   
    //     PDMA_CLR_CH_INT_FLAG(SPI_FLASH_RX_DMA_CH, PDMA_ISR_BLKD_IF_Msk);   
        
    //     /* Handle PDMA block transfer done interrupt event */
    //     //insert process
			
	// 	PDMA_FLASH_RX_INT_Flag = 1;         
    // }    

}

void SpiFlash_RX_PDMA(uint8_t* Rx , uint16_t len)
{	
	PDMA_FLASH_RX_INT_Flag = 0;

	// SpiFlash_Set_CS_Low();

	//RX	
    PDMA_SetTransferCnt(SPI_FLASH_RX_DMA_CH, PDMA_WIDTH_8, len);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_FLASH_RX_DMA_CH, (uint32_t)&SPI_FLASH_PORT->RX, PDMA_SAR_FIX, (uint32_t)Rx, PDMA_DAR_INC);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(SPI_FLASH_RX_DMA_CH, PDMA_SPI1_RX, FALSE, 0);

	PDMA_Trigger(SPI_FLASH_RX_DMA_CH);
	SPI_TRIGGER_RX_PDMA(SPI_FLASH_PORT);
	SPI_TRIGGER(SPI_FLASH_PORT);

	// PDMA_EnableInt(SPI_FLASH_RX_DMA_CH, PDMA_IER_BLKD_IE_Msk);

    // wait PDMA done
	#if 1
    while(!PDMA_FLASH_RX_INT_Flag);	
	#else	
    while(1)
    {
        if(PDMA_FLASH_RX_INT_Flag)
        {
            PDMA_FLASH_RX_INT_Flag = 0;
            break;
        }
    }
	#endif

	while (SPI_IS_BUSY(SPI_FLASH_PORT));			
	// SpiFlash_Set_CS_High();

	// PDMA_DisableInt(SPI_FLASH_RX_DMA_CH, PDMA_IER_BLKD_IE_Msk);	
}

void SpiFlash_TX_PDMA(uint8_t* Tx , uint16_t len)
{
	PDMA_FLASH_TX_INT_Flag = 0;

	// SpiFlash_Set_CS_Low();

	//TX
    PDMA_SetTransferCnt(SPI_FLASH_TX_DMA_CH, PDMA_WIDTH_8, len);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_FLASH_TX_DMA_CH, (uint32_t)Tx, PDMA_SAR_INC, (uint32_t)&SPI_FLASH_PORT->TX, PDMA_DAR_FIX);
    /* Set request source; set basic mode. */	
    PDMA_SetTransferMode(SPI_FLASH_TX_DMA_CH, PDMA_SPI1_TX, FALSE, 0);
 
    PDMA_Trigger(SPI_FLASH_TX_DMA_CH);
    SPI_TRIGGER_TX_PDMA(SPI_FLASH_PORT);
	SPI_TRIGGER(SPI_FLASH_PORT);

    // PDMA_EnableInt(SPI_FLASH_TX_DMA_CH, PDMA_IER_BLKD_IE_Msk);

	#if 1 //debug
	printf("TX start\r\n");	
	#endif

    // wait PDMA done
	#if 1
    while(!PDMA_FLASH_TX_INT_Flag);	
	#else
    while(1)
    {
        if(PDMA_FLASH_TX_INT_Flag)
        {
            PDMA_FLASH_TX_INT_Flag = 0;
            break;
        }
    }
	#endif

	while (SPI_IS_BUSY(SPI_FLASH_PORT));			
	// SpiFlash_Set_CS_High();

	// PDMA_DisableInt(SPI_FLASH_TX_DMA_CH, PDMA_IER_BLKD_IE_Msk);
}

void SpiFlash_TX_Write(uint8_t width , uint32_t TxData)
{
    SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, width);

    SPI_WRITE_TX(SPI_FLASH_PORT, TxData);
	SPI_TRIGGER(SPI_FLASH_PORT);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

}

void SpiFlash_WriteEnable(void)
{
    // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0x06, Write enable
	SpiFlash_TX_Write(8,0x06);

    // /CS: de-active
    SpiFlash_Set_CS_High();
}

uint8_t SpiFlash_SendByte(uint8_t byte)
{

	/*!< Loop while DR register in not emplty */
	// while (SPI_GetStatus(SPI_FLASH_PORT, SPI_TX_EMPTY_MASK) == FALSE);
	/*!< Send byte through the SPI1 peripheral */
	SPI_WRITE_TX(SPI_FLASH_PORT, byte);SPI_TRIGGER(SPI_FLASH_PORT);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

	/*!< Wait to receive a byte */
	while (SPI_GetStatus(SPI_FLASH_PORT, SPI_RX_EMPTY_MASK) != FALSE);
	/*!< Return the byte read from the SPI bus */
	return SPI_READ_RX(SPI_FLASH_PORT);
}

uint16_t SpiFlash_ReadMidDid(void)
{
    unsigned int au32DestinationData;

    // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0x90, Read Manufacturer/Device ID
	SpiFlash_TX_Write(8,0x90);

    // send 24-bit '0', dummy
	SpiFlash_TX_Write(24,0x00);

    // receive 16-bit
	SpiFlash_TX_Write(16,0x00);

    // /CS: de-active
    SpiFlash_Set_CS_High();


	au32DestinationData = SPI_READ_RX(SPI_FLASH_PORT);
    return (au32DestinationData & 0xFFFF);

}

void SpiFlash_ChipErase(void)
{
	SpiFlash_WriteEnable();

    // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0xC7, Chip Erase
	SpiFlash_TX_Write(8,0xC7);	

    // /CS: de-active
    SpiFlash_Set_CS_High();

}

void SpiFlash_SectorErase(uint32_t SectorAddr)
{

	SpiFlash_WriteEnable();

    SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 8);

    // /CS: active
    SpiFlash_Set_CS_Low();

    SpiFlash_SendByte(0x20);

    SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 24);
    // send 24-bit start address
    SpiFlash_SendByte(SectorAddr);

    // /CS: de-active
    SpiFlash_Set_CS_High();

	SpiFlash_WaitReady();
}

uint8_t SpiFlash_ReadStatusReg(void)
{
    unsigned int au32DestinationData;

    // /CS: active
    SpiFlash_Set_CS_Low();

	SpiFlash_TX_Write(16,0x0500);	

    // /CS: de-active
    SpiFlash_Set_CS_High();

    // skip first rx data
    au32DestinationData = SPI_READ_RX(SPI_FLASH_PORT);

    return ( au32DestinationData & 0xFF);
	
}

void SpiFlash_WriteStatusReg(uint8_t u8Value)
{
	SpiFlash_WriteEnable();

    // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0x01, Write status register
	SpiFlash_TX_Write(8,0x01);	

    // write status
	SpiFlash_TX_Write(8,u8Value);

    // /CS: de-active
    SpiFlash_Set_CS_High();

}

void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue = 0;
//    uint32_t cnt = 0;
	
    do
    {
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;

		#if 0	//debug purpose
		printf("BUSY counter : %4d\r\n" , cnt++);
//		printf(".");
		
		#endif

    }
    while(ReturnValue!=0);   // check the BUSY bit

//	#if (_debug_log_UART_ == 1)	//debug
//	printf("\r\n");
//	#endif
}

void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer, uint16_t NumByteToWrite , uint8_t EnablePDMA)
{
    uint32_t i = 0;
	
	SpiFlash_WriteEnable();

    // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0x02, Page program
	SpiFlash_TX_Write(8,0x02);	

    // send 24-bit start address
	SpiFlash_TX_Write(24,StartAddress);	

    // write data
	if (EnablePDMA)
	{
		SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 8);
		SpiFlash_TX_PDMA(u8DataBuffer , NumByteToWrite);
//		TIMER_Delay(TIMER0,500);		
	}
	else
	{

	    // SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 8);
	    for(i = 0 ; i < NumByteToWrite ; i++)
		{
			// SpiFlash_SendByte(u8DataBuffer[i]);

			/*!< Send byte through the SPI1 peripheral */
			// SPI_WRITE_TX(SPI_FLASH_PORT, u8DataBuffer[i]);SPI_TRIGGER(SPI_FLASH_PORT);

			// wait tx finish
			// while(SPI_IS_BUSY(SPI_FLASH_PORT));
			SpiFlash_TX_Write(8,u8DataBuffer[i]);	
		}

	    // wait tx finish
	    // while(SPI_IS_BUSY(SPI_FLASH_PORT));
		TIMER_Delay(TIMER0,500);
						
	}

	// /CS: de-active
	SpiFlash_Set_CS_High();

    // SPI_ClearRxFIFO(SPI_FLASH_PORT);

}

void SpiFlash_PageWrite(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToWrite , uint8_t EnablePDMA)
{
	SpiFlash_NormalPageProgram(page_no*SPI_FLASH_PAGE_BYTE , u8DataBuffer ,NumByteToWrite, EnablePDMA);
	SpiFlash_WaitReady();
}

// void SpiFlash_SectorWrite(uint32_t sector_no, uint8_t *u8DataBuffer, uint16_t NumByteToWrite , uint8_t EnablePDMA)
// {
// 	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

// 	uint32_t WriteAddr = sector_no*SPI_FLASH_SECTOR_SIZE ;

// 	Addr = WriteAddr % SPI_FLASH_PAGE_BYTE;
// 	count = SPI_FLASH_PAGE_BYTE - Addr;
// 	NumOfPage =  NumByteToWrite / SPI_FLASH_PAGE_BYTE;
// 	NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_BYTE;

// 	// printf("%s : Addr:0x%3X,count:0x%2X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" ,__FUNCTION__ , Addr,count,NumOfPage,NumOfSingle);

// 	SpiFlash_SectorErase(sector_no);

// 	if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
// 	{
// 		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
// 		{
// 			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumByteToWrite, EnablePDMA);
// 		}
// 		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
// 		{
// 			while (NumOfPage--)
// 			{
// 				// printf("WriteAddr:0x%3X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" , WriteAddr,NumOfPage,NumOfSingle);
// 				TIMER_Delay(TIMER0,500);

// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
// 				WriteAddr +=  SPI_FLASH_PAGE_BYTE;
// 				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
// 			}

// 			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
// 		}
// 	}
// 	else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
// 	{
// 		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
// 		{
// 			if (NumOfSingle > count) /*!< (SPI_FLASH_SECTOR_SIZE + WriteAddr) > sFLASH_PAGESIZE */
// 			{
// 				temp = NumOfSingle - count;

// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  count, EnablePDMA);
// 				WriteAddr +=  count;
// 				u8DataBuffer += count;

// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  temp, EnablePDMA);
// 			}
// 			else
// 			{
// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumByteToWrite, EnablePDMA);
// 			}
// 		}
// 		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
// 		{
// 			NumByteToWrite -= count;
// 			NumOfPage =  NumByteToWrite / SPI_FLASH_PAGE_BYTE;
// 			NumOfSingle = NumByteToWrite % SPI_FLASH_PAGE_BYTE;

// 			SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  count, EnablePDMA);
// 			WriteAddr +=  count;
// 			u8DataBuffer += count;

// 			while (NumOfPage--)
// 			{
// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
// 				WriteAddr +=  SPI_FLASH_PAGE_BYTE;
// 				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
// 			}

// 			if (NumOfSingle != 0)
// 			{
// 				SpiFlash_NormalPageProgram(WriteAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
// 			}
// 		}
// 	}

// 	SpiFlash_WaitReady();
	
// }

void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
{
    uint32_t i = 0;

   // /CS: active
    SpiFlash_Set_CS_Low();

    // send Command: 0x03, Read data
	SpiFlash_TX_Write(8,0x03);

    // send 24-bit start address
	SpiFlash_TX_Write(24,StartAddress);	

    // read data
    if (EnablePDMA)
    {
		SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 8);
		SpiFlash_RX_PDMA(u8DataBuffer , NumByteToRead);
		TIMER_Delay(TIMER0,500);
    }
	else
	{
		// SPI_SET_DATA_WIDTH(SPI_FLASH_PORT, 8);
	    for(i = 0 ; i < NumByteToRead ; i++)
	    {
	        // SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);SPI_TRIGGER(SPI_FLASH_PORT);
	        // while(SPI_IS_BUSY(SPI_FLASH_PORT));
			SpiFlash_TX_Write(8,0x00);

	        u8DataBuffer[i] = (unsigned char) SPI_READ_RX(SPI_FLASH_PORT);
	    }

	    // wait tx finish
	    // while(SPI_IS_BUSY(SPI_FLASH_PORT));
		TIMER_Delay(TIMER0,500);
	}	

	// /CS: de-active
	SpiFlash_Set_CS_High();			
}

void SpiFlash_PageRead(uint32_t page_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
{
	SpiFlash_NormalRead(page_no*SPI_FLASH_PAGE_BYTE , u8DataBuffer ,NumByteToRead, EnablePDMA);
}

// void SpiFlash_SectorRead(uint32_t sector_no, uint8_t *u8DataBuffer , uint16_t NumByteToRead, uint8_t EnablePDMA)
// {

// 	uint32_t ReadAddr = sector_no*SPI_FLASH_SECTOR_SIZE;
// 	uint16_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

// 	Addr = ReadAddr % SPI_FLASH_PAGE_BYTE;
// 	count = SPI_FLASH_PAGE_BYTE - Addr;
// 	NumOfPage =  NumByteToRead / SPI_FLASH_PAGE_BYTE;
// 	NumOfSingle = NumByteToRead % SPI_FLASH_PAGE_BYTE;

// 	if (Addr == 0) /*!< WriteAddr is sFLASH_PAGESIZE aligned  */
// 	{
// 		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
// 		{
// 			SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumByteToRead, EnablePDMA);
// 		}
// 		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
// 		{
// 			while (NumOfPage--)
// 			{
// 				// printf("ReadAddr:0x%3X,NumOfPage:0x%2X,NumOfSingle:0x%2X,\r\n" , ReadAddr,NumOfPage,NumOfSingle);
// 				TIMER_Delay(TIMER0,500);
				
// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
// 				ReadAddr +=  SPI_FLASH_PAGE_BYTE;
// 				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
// 			}
			
// 			// TIMER_Delay(TIMER0,500);
// 			// SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
// 		}
// 	}
// 	else /*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
// 	{
// 		if (NumOfPage == 0) /*!< SPI_FLASH_SECTOR_SIZE < sFLASH_PAGESIZE */
// 		{
// 			if (NumOfSingle > count) /*!< (SPI_FLASH_SECTOR_SIZE + WriteAddr) > sFLASH_PAGESIZE */
// 			{
// 				temp = NumOfSingle - count;

// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  count, EnablePDMA);
// 				ReadAddr +=  count;
// 				u8DataBuffer += count;

// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  temp, EnablePDMA);
// 			}
// 			else
// 			{
// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumByteToRead, EnablePDMA);
// 			}
// 		}
// 		else /*!< SPI_FLASH_SECTOR_SIZE > sFLASH_PAGESIZE */
// 		{
// 			NumByteToRead -= count;
// 			NumOfPage =  NumByteToRead / SPI_FLASH_PAGE_BYTE;
// 			NumOfSingle = NumByteToRead % SPI_FLASH_PAGE_BYTE;

// 			SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  count, EnablePDMA);
// 			ReadAddr +=  count;
// 			u8DataBuffer += count;

// 			while (NumOfPage--)
// 			{
// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  SPI_FLASH_PAGE_BYTE, EnablePDMA);
// 				ReadAddr +=  SPI_FLASH_PAGE_BYTE;
// 				u8DataBuffer += SPI_FLASH_PAGE_BYTE;
// 			}

// 			if (NumOfSingle != 0)
// 			{
// 				SpiFlash_NormalRead(ReadAddr,u8DataBuffer,  NumOfSingle, EnablePDMA);
// 			}
// 		}
// 	}
 
// }

void SpiFlash_PDMA_Init(void)
{
    PDMA_T *PDMA_TX;	
    PDMA_T *PDMA_RX;

    PDMA_Open( (1 << SPI_FLASH_TX_DMA_CH));

	//TX
    PDMA_SetTransferCnt(SPI_FLASH_TX_DMA_CH, PDMA_WIDTH_8, SPI_FLASH_PAGE_BYTE);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_FLASH_TX_DMA_CH, (uint32_t)TxBuffer, PDMA_SAR_INC, (uint32_t)&SPI_FLASH_PORT->TX, PDMA_DAR_FIX);
    /* Set request source; set basic mode. */	
    PDMA_SetTransferMode(SPI_FLASH_TX_DMA_CH, PDMA_SPI1_TX, FALSE, 0);
	
	/* Set Memory-to-Peripheral mode */
	PDMA_TX = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_FLASH_TX_DMA_CH));
	PDMA_TX->CSR = (PDMA_TX->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x2<<PDMA_CSR_MODE_SEL_Pos);

    PDMA_Open( (1 << SPI_FLASH_RX_DMA_CH));
	//RX	
    PDMA_SetTransferCnt(SPI_FLASH_RX_DMA_CH, PDMA_WIDTH_8, SPI_FLASH_PAGE_BYTE);
    /* Set source/destination address and attributes */
    PDMA_SetTransferAddr(SPI_FLASH_RX_DMA_CH, (uint32_t)&SPI_FLASH_PORT->RX, PDMA_SAR_FIX, (uint32_t)RxBuffer, PDMA_DAR_INC);
    /* Set request source; set basic mode. */
    PDMA_SetTransferMode(SPI_FLASH_RX_DMA_CH, PDMA_SPI1_RX, FALSE, 0);
	
	PDMA_RX = (PDMA_T *)((uint32_t) PDMA0_BASE + (0x100 * SPI_FLASH_RX_DMA_CH));
	PDMA_RX->CSR = (PDMA_RX->CSR & (~PDMA_CSR_MODE_SEL_Msk)) | (0x2<<PDMA_CSR_MODE_SEL_Pos);

    PDMA_EnableInt(SPI_FLASH_TX_DMA_CH, PDMA_IER_BLKD_IE_Msk);	
    PDMA_EnableInt(SPI_FLASH_RX_DMA_CH, PDMA_IER_BLKD_IE_Msk);

	NVIC_EnableIRQ(PDMA_IRQn);
}

void SpiFlash_Init(void)
{
    uint16_t u16ID = 0;
    uint16_t i = 0;

    /* Set PA.15 and PA.14 as Output mode */
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA15_Msk | SYS_GPA_MFP_PA14_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA15_GPIO | SYS_GPA_MFP_PA14_GPIO;
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PA15_Msk | SYS_ALT_MFP_PA14_Msk);
    SYS->ALT_MFP |= SYS_ALT_MFP_PA15_GPIO | SYS_ALT_MFP_PA14_GPIO;
 
    GPIO_SetMode(PA, (BIT14 | BIT15), GPIO_PMD_OUTPUT); 
	
    /* Set WP : PA14 and HOLD : PA15 of SPI Flash to high */ 
    PA14 = 1;
    PA15 = 1;

	//initial TX , RX data
    for (i=0; i < SPI_FLASH_PAGE_BYTE; i++)
    {
        TxBuffer[i] = 0xFF;
        RxBuffer[i] = 0xFF;
    }

	SYS_ResetModule(SPI1_RST);
    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI Mode-0 timing, clock is 20MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, SPI_FLASH_CLK_FREQ);

   	SYS_UnlockReg();	

	#if defined (ENABLE_MANUAL_SS)	
    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC8_Msk);
    SYS->GPC_MFP |= SYS_GPC_MFP_PC8_GPIO;
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC8_Msk);
    SYS->ALT_MFP |= SYS_ALT_MFP_PC8_GPIO;

    // USE SS : PC8 as manual control , 
    GPIO_SetMode(PC, BIT8, GPIO_PMD_OUTPUT);
	#endif
	
    SYS_LockReg();	

	#if defined (ENABLE_MANUAL_SS)	
    /* Disable auto SS function, control SS signal manually. */
    SPI_DisableAutoSS(SPI_FLASH_PORT);

    // /CS: de-active
    SpiFlash_Set_CS_High();

	#elif defined (ENABLE_AUTO_SS)
    SPI_EnableAutoSS(SPI_FLASH_PORT, SPI_SS, SPI_SS_ACTIVE_LOW);
	#endif

	SYS_ResetModule(PDMA_RST);
	SpiFlash_PDMA_Init();

	u16ID = SpiFlash_ReadMidDid();
	#if (_debug_log_UART_ == 1)	//debug
	printf("ID : 0x%2X\r\n" , u16ID);		// EVM_SPI_FALSH_ID
	#endif
	

}



