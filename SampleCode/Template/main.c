/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#include "project_config.h"

#include "SPI_Flash.h"
/*_____ D E C L A R A T I O N S ____________________________________________*/
#define PLL_CLOCK   							72000000

/*_____ D E F I N I T I O N S ______________________________________________*/
volatile uint32_t BitFlag = 0;
volatile uint32_t counter_tick = 0;
volatile uint32_t counter_systick = 0;

uint8_t uart_rxData = 0;

uint8_t TxBuffer[SPI_FLASH_PAGE_BYTE] = {0};
uint8_t RxBuffer[SPI_FLASH_PAGE_BYTE] = {0};

uint8_t SPI_FLASH_page_counter = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

void systick_counter(void)
{
	counter_systick++;
}

uint32_t get_systick(void)
{
	return (counter_systick);
}

void set_systick(uint32_t t)
{
	counter_systick = t;
}

void tick_counter(void)
{
	counter_tick++;
}

uint32_t get_tick(void)
{
	return (counter_tick);
}

void set_tick(uint32_t t)
{
	counter_tick = t;
}

void compare_buffer(uint8_t *src, uint8_t *des, int nBytes)
{
    uint16_t i = 0;	
	
    #if 1
    for (i = 0; i < nBytes; i++)
    {
        if (src[i] != des[i])
        {
            printf("error idx : %4d : 0x%2X , 0x%2X\r\n", i , src[i],des[i]);
			set_flag(flag_error , ENABLE);
        }
    }

	if (!is_flag_set(flag_error))
	{
    	printf("%s finish \r\n" , __FUNCTION__);	
		set_flag(flag_error , DISABLE);
	}
    #else
    if (memcmp(src, des, nBytes))
    {
        printf("\nMismatch!! - %d\n", nBytes);
        for (i = 0; i < nBytes; i++)
            printf("0x%02x    0x%02x\n", src[i], des[i]);
        return -1;
    }
    #endif

}

void reset_buffer(void *dest, unsigned int val, unsigned int size)
{
    uint8_t *pu8Dest;
//    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;

	#if 1
	while (size-- > 0)
		*pu8Dest++ = val;
	#else
	memset(pu8Dest, val, size * (sizeof(pu8Dest[0]) ));
	#endif
	
}

void copy_buffer(void *dest, void *src, unsigned int size)
{
    uint8_t *pu8Src, *pu8Dest;
    unsigned int i;
    
    pu8Dest = (uint8_t *)dest;
    pu8Src  = (uint8_t *)src;


	#if 0
	  while (size--)
	    *pu8Dest++ = *pu8Src++;
	#else
    for (i = 0; i < size; i++)
        pu8Dest[i] = pu8Src[i];
	#endif
}

void dump_buffer(uint8_t *pucBuff, int nBytes)
{
    uint16_t i = 0;
    
    printf("dump_buffer : %2d\r\n" , nBytes);    
    for (i = 0 ; i < nBytes ; i++)
    {
        printf("0x%2X," , pucBuff[i]);
        if ((i+1)%8 ==0)
        {
            printf("\r\n");
        }            
    }
    printf("\r\n\r\n");
}

void dump_buffer_hex(uint8_t *pucBuff, int nBytes)
{
    int     nIdx, i;

    nIdx = 0;
    while (nBytes > 0)
    {
        printf("0x%04X  ", nIdx);
        for (i = 0; i < 16; i++)
            printf("%02X ", pucBuff[nIdx + i]);
        printf("  ");
        for (i = 0; i < 16; i++)
        {
            if ((pucBuff[nIdx + i] >= 0x20) && (pucBuff[nIdx + i] < 127))
                printf("%c", pucBuff[nIdx + i]);
            else
                printf(".");
            nBytes--;
        }
        nIdx += 16;
        printf("\n");
    }
    printf("\n");
}

void delay(uint16_t dly)
{
/*
	delay(100) : 14.84 us
	delay(200) : 29.37 us
	delay(300) : 43.97 us
	delay(400) : 58.5 us	
	delay(500) : 73.13 us	
	
	delay(1500) : 0.218 ms (218 us)
	delay(2000) : 0.291 ms (291 us)	
*/

	while( dly--);
}

void delay_ms(uint16_t ms)
{
	TIMER_Delay(TIMER0, 1000*ms);
}

void GPIO_Init (void)
{
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB4_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB4_GPIO);
	
    GPIO_SetMode(PB, BIT4, GPIO_PMD_OUTPUT);

}

void SysTick_Handler(void)
{

    systick_counter();

    if (get_systick() >= 0xFFFFFFFF)
    {
        set_systick(0);      
    }

    // if ((get_systick() % 1000) == 0)
    // {
       
    // }

    #if defined (ENABLE_TICK_EVENT)
    TickCheckTickEvent();
    #endif    
}

void SysTick_delay(unsigned long delay)
{  
    
    uint32_t tickstart = get_systick(); 
    uint32_t wait = delay; 

    while((get_systick() - tickstart) < wait) 
    { 
    } 

}

void SysTick_enable(int ticks_per_second)
{
    set_systick(0);
    if (SysTick_Config(SystemCoreClock / ticks_per_second))
    {
        /* Setup SysTick Timer for 1 second interrupts  */
        printf("Set system tick error!!\n");
        while (1);
    }

    #if defined (ENABLE_TICK_EVENT)
    TickInitTickEvent();
    #endif
}

void TMR1_IRQHandler(void)
{
	
    if(TIMER_GetIntFlag(TIMER1) == 1)
    {
        TIMER_ClearIntFlag(TIMER1);
		tick_counter();

		if ((get_tick() % 1000) == 0)
		{
            set_flag(flag_timer_period_1000ms ,ENABLE);
		}

		if ((get_tick() % 50) == 0)
		{

		}	
    }
}

void TIMER1_Init(void)
{
    TIMER_Open(TIMER1, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER1);
    NVIC_EnableIRQ(TMR1_IRQn);	
    TIMER_Start(TIMER1);
}


void UARTx_Process(void)
{
	uint8_t res = 0;
    uint16_t i = 0;
	static uint8_t cnt = 0;
    uint16_t page_cnt = 0;
		
	res = uart_rxData;// res = UART_READ(UART0);
    printf("digit : %c\r\n" ,res);

	if (res > 0x7F)
	{
		printf("invalid command\r\n");
	}
	else
	{
		switch(res)
		{
			case '?':
				#if (_debug_log_UART_ == 1)	//debug
				printf("\r\n==========================\r\n");
				
				printf("1: chip erase\r\n");
				printf("2: fill in TX data\r\n");
				printf("3: page counter\r\n");
				printf("4: Page write\r\n");
				printf("5: Page read\r\n");		
				printf("6: self test , write , read , compare\r\n");	
				printf("7: self test , write , read , compare  , with PDMA\r\n");
				
				printf("8: read ID (0x90)\r\n");	
				
				printf("==========================\r\n\r\n");
				#endif
				break;	

		
			case '1':
				#if (_debug_log_UART_ == 1)	//debug
				printf("perform SpiFlash_ChipErase\r\n");
				#endif
				
			    /* Erase SPI flash */
			    SpiFlash_ChipErase();

			    /* Wait ready */
			    SpiFlash_WaitReady();

				#if (_debug_log_UART_ == 1)	//debug
				printf("erase finish\r\n\r\n");
				#endif
				break;	

			case '2':
				#if (_debug_log_UART_ == 1)	//debug				
				printf("increase test data start from 0x%2X\r\n" , cnt);
				#endif

				//reset TxBuffer
				reset_buffer(TxBuffer,0x00,SPI_FLASH_PAGE_BYTE);

				//fill in data
			    for ( i = 0; i < SPI_FLASH_PAGE_BYTE; i++)
			    {
			        TxBuffer[i] = 0x00 + i + cnt;
			    }

				#if (_debug_log_UART_ == 1)	//debug
				printf("TxBuffer : \r\n");	
				dump_buffer_hex(TxBuffer,SPI_FLASH_PAGE_BYTE);				
				#endif	
			
				cnt++; 
			
				break;

			case '3':
				#if (_debug_log_UART_ == 1)	//debug
				printf("SPI_FLASH_page_counter current : 0x%2X\r\n\r\n" ,SPI_FLASH_page_counter++);	
				#endif
			
				break;

			case '4':
				#if (_debug_log_UART_ == 1)	//debug
				printf("programming...(%d)\r\n",SPI_FLASH_page_counter);
				#endif
		
//				SpiFlash_PageWrite(SPI_FLASH_page_counter,TxBuffer,SPI_FLASH_PAGE_BYTE,DISABLE);
				SpiFlash_PageWrite(SPI_FLASH_page_counter,TxBuffer,SPI_FLASH_PAGE_BYTE,ENABLE);
				
				#if (_debug_log_UART_ == 1)	//debug
				printf("programming finish\r\n\r\n");
				#endif
				
				break;

			case '5':
				#if (_debug_log_UART_ == 1)	//debug
				printf("read page ...(%2d)\r\n",SPI_FLASH_page_counter);
				#endif

				//reset RxBuffer
			    reset_buffer(RxBuffer,0x00,SPI_FLASH_PAGE_BYTE);
				
//				SpiFlash_PageRead(SPI_FLASH_page_counter,RxBuffer,SPI_FLASH_PAGE_BYTE,DISABLE);
				SpiFlash_PageRead(SPI_FLASH_page_counter,RxBuffer,SPI_FLASH_PAGE_BYTE,ENABLE);
				
				#if (_debug_log_UART_ == 1)	//debug				
				dump_buffer_hex(RxBuffer,SPI_FLASH_PAGE_BYTE);
				printf("read page finish\r\n\r\n");	
				#endif
				
				break;				

			case '6':
				#if (_debug_log_UART_ == 1)	//debug
				printf("perform SpiFlash_ChipErase\r\n");
				#endif
				
			    /* Erase SPI flash */
			    SpiFlash_ChipErase();

			    /* Wait ready */
			    SpiFlash_WaitReady();

				#if (_debug_log_UART_ == 1)	//debug
				printf("erase finish\r\n\r\n");
				#endif

				for ( page_cnt = 0 ; page_cnt < TEST_NUMBER ; page_cnt++)
				{
					#if (_debug_log_UART_ == 1)	//debug
					printf("\r\nSELF TEST ... (idx : %2d)\r\n" , page_cnt);
					#endif

					//reset RxBuffer
				    reset_buffer(RxBuffer,0x00,SPI_FLASH_PAGE_BYTE);

					//reset TxBuffer
					reset_buffer(TxBuffer,0x00,SPI_FLASH_PAGE_BYTE);

					//fill in data
				    for ( i = 0; i < SPI_FLASH_PAGE_BYTE; i++)
				    {
				        TxBuffer[i] = 0x00 + i + cnt;
				    }

//					printf("\r\nTxBuffer : \r\n");
//					dump_buffer_hex(TxBuffer,SPI_FLASH_PAGE_BYTE);				
					cnt++;
					
					SpiFlash_PageWrite(page_cnt,TxBuffer,SPI_FLASH_PAGE_BYTE,DISABLE);
					SpiFlash_PageRead(page_cnt,RxBuffer,SPI_FLASH_PAGE_BYTE,DISABLE);

//					printf("\r\nRxBuffer\r\n");
//					dump_buffer_hex(RxBuffer,SPI_FLASH_PAGE_BYTE);		

					compare_buffer(TxBuffer,RxBuffer,SPI_FLASH_PAGE_BYTE);
				}

				#if (_debug_log_UART_ == 1)	//debug
				printf("SELF TEST finish\r\n\r\n");	
				#endif
			
				break;	


			case '7':

				#if (_debug_log_UART_ == 1)	//debug
				printf("perform SpiFlash_ChipErase\r\n");
				#endif
				
			    /* Erase SPI flash */
			    SpiFlash_ChipErase();

			    /* Wait ready */
			    SpiFlash_WaitReady();

				#if (_debug_log_UART_ == 1)	//debug
				printf("erase finish\r\n\r\n");
				#endif

				for ( page_cnt = 0 ; page_cnt < TEST_NUMBER ; page_cnt++)
				{
					#if (_debug_log_UART_ == 1)	//debug
					printf("\r\nPDMA SELF TEST ... (idx : %2d)\r\n" , page_cnt);
					#endif

					//reset RxBuffer
				    reset_buffer(RxBuffer,0x00,SPI_FLASH_PAGE_BYTE);

					//reset TxBuffer
					reset_buffer(TxBuffer,0x00,SPI_FLASH_PAGE_BYTE);

					//fill in data
				    for ( i = 0; i < SPI_FLASH_PAGE_BYTE; i++)
				    {
				        TxBuffer[i] = 0x00 + i + cnt;
				    }

//					printf("\r\nTxBuffer : \r\n");
//					dump_buffer_hex(TxBuffer,SPI_FLASH_PAGE_BYTE);				
					cnt++;
					
					SpiFlash_PageWrite(page_cnt,TxBuffer,SPI_FLASH_PAGE_BYTE,ENABLE);
					SpiFlash_PageRead(page_cnt,RxBuffer,SPI_FLASH_PAGE_BYTE,ENABLE);

//					printf("\r\nRxBuffer\r\n");
//					dump_buffer_hex(RxBuffer,SPI_FLASH_PAGE_BYTE);		

					compare_buffer(TxBuffer,RxBuffer,SPI_FLASH_PAGE_BYTE);
				}

				#if (_debug_log_UART_ == 1)	//debug
				printf("PDMA SELF TEST finish\r\n\r\n");	
				#endif

				break;

			case '8':			
				i = SpiFlash_ReadMidDid();
				#if (_debug_log_UART_ == 1)	//debug
				printf("SpiFlash_ReadMidDid : 0x%2X\r\n\n" , i);
				#endif
				
				break;	

			case 'X':
			case 'x':
			case 'Z':
			case 'z':
				NVIC_SystemReset();		
				break;
		}
	}
}

void UART02_IRQHandler(void)
{

    if(UART_GET_INT_FLAG(UART0, UART_ISR_RDA_INT_Msk | UART_ISR_TOUT_IF_Msk))     /* UART receive data available flag */
    {
        while(UART_GET_RX_EMPTY(UART0) == 0)
        {
			set_flag(flag_uart_rcv ,ENABLE);
            uart_rxData = UART_READ(UART0);// UARTx_Process();
        }
    }

}

void UART0_Init(void)
{
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
    UART_EnableInt(UART0, UART_IER_RDA_IEN_Msk | UART_IER_TOUT_IEN_Msk);
    NVIC_EnableIRQ(UART02_IRQn);
	
	#if (_debug_log_UART_ == 1)	//debug
	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHXTFreq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetLXTFreq : %8d\r\n",CLK_GetLXTFreq());	
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());
	printf("CLK_GetPCLKFreq : %8d\r\n",CLK_GetPCLKFreq());	
	#endif	

}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    // CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    // CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_HIRC, CLK_CLKDIV_UART(1));
	
    CLK_EnableModuleClock(TMR0_MODULE);
  	CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, 0);

    CLK_EnableModuleClock(TMR1_MODULE);
  	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1_S_HIRC, 0);

    // CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0_S_HCLK, MODULE_NoMsk);
    // CLK_EnableModuleClock(SPI0_MODULE);

    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL1_SPI1_S_HCLK, MODULE_NoMsk);
    CLK_EnableModuleClock(SPI1_MODULE);

    CLK_EnableModuleClock(PDMA_MODULE);

    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    // SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC0_Msk | SYS_GPC_MFP_PC1_Msk | SYS_GPC_MFP_PC2_Msk | SYS_GPC_MFP_PC3_Msk);
    // SYS->GPC_MFP |= SYS_GPC_MFP_PC0_SPI0_SS0 | SYS_GPC_MFP_PC1_SPI0_CLK | SYS_GPC_MFP_PC2_SPI0_MISO0 | SYS_GPC_MFP_PC3_SPI0_MOSI0;
    // SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC0_Msk | SYS_ALT_MFP_PC1_Msk | SYS_ALT_MFP_PC2_Msk | SYS_ALT_MFP_PC3_Msk);
    // SYS->ALT_MFP |= SYS_ALT_MFP_PC0_SPI0_SS0 | SYS_ALT_MFP_PC1_SPI0_CLK | SYS_ALT_MFP_PC2_SPI0_MISO0 | SYS_ALT_MFP_PC3_SPI0_MOSI0;

    SYS->GPC_MFP &= ~(SYS_GPC_MFP_PC8_Msk | SYS_GPC_MFP_PC9_Msk | SYS_GPC_MFP_PC10_Msk | SYS_GPC_MFP_PC11_Msk);
    SYS->GPC_MFP |= SYS_GPC_MFP_PC8_SPI1_SS0 | SYS_GPC_MFP_PC9_SPI1_CLK | SYS_GPC_MFP_PC10_SPI1_MISO0 | SYS_GPC_MFP_PC11_SPI1_MOSI0;
    SYS->ALT_MFP &= ~(SYS_ALT_MFP_PC8_Msk);
    SYS->ALT_MFP |= SYS_ALT_MFP_PC8_SPI1_SS0;


   /* Update System Core Clock */
    SystemCoreClockUpdate();

    /* Lock protected registers */
    SYS_LockReg();
}

void loop(void)
{
	// static uint32_t LOG1 = 0;
	// static uint32_t LOG2 = 0;

    if ((get_systick() % 1000) == 0)
    {
        // printf("%s(systick) : %4d\r\n",__FUNCTION__,LOG2++);    
    }

    if (is_flag_set(flag_timer_period_1000ms))
    {
        set_flag(flag_timer_period_1000ms ,DISABLE);

        // printf("%s(timer) : %4d\r\n",__FUNCTION__,LOG1++);
        PB4 ^= 1;   
    }
    if (is_flag_set(flag_uart_rcv))
    {
        set_flag(flag_uart_rcv ,DISABLE);
		UARTx_Process();
	}

}

int main()
{
    SYS_Init();

	GPIO_Init();
	UART0_Init();
	TIMER1_Init();

    SysTick_enable(1000);

    SpiFlash_Init();

    while(1)
    {
        loop();

    }
}

/*** (C) COPYRIGHT 2017 Nuvoton Technology Corp. ***/
