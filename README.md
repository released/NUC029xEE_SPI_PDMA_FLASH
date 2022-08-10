# NUC029xEE_SPI_PDMA_FLASH
 NUC029xEE_SPI_PDMA_FLASH


update @ 2022/08/10

1. initial SPI1 with PDMA interrupt enable (SPI1 TX : channel 2 , SPI2 RX :channel 3)

	PC8_SPI1_SS0 , 	PC9_SPI1_CLK , 	PC10_SPI1_MISO0 , 	PC11_SPI1_MOSI0

2. Enable define : ENABLE_AUTO_SS or ENABLE_AUTO_SS , to see different CS pin behavior

3. use terminal to test SPI flash function

press letter ? , display support function as below , 

![image](https://github.com/released/NUC029xEE_SPI_PDMA_FLASH/blob/main/question_mark.jpg)	

press digit 1 , to do SPI flash erase chip

press digit 2 , to generate TX buffer for test

![image](https://github.com/released/NUC029xEE_SPI_PDMA_FLASH/blob/main/digit_1_2.jpg)	


press digit 4 , to do SPI flash page write with PDMA (SPI TX data from digit 2) 

press digit 5 , to do SPI flash page read with PDMA , and display RX buffer result

![image](https://github.com/released/NUC029xEE_SPI_PDMA_FLASH/blob/main/digit_4_5.jpg)	


press digit 6 , to do SPI flash page write/read flow 

![image](https://github.com/released/NUC029xEE_SPI_PDMA_FLASH/blob/main/digit_6.jpg)	


press digit 7 , to do SPI flash page write/read flow with PDMA 

![image](https://github.com/released/NUC029xEE_SPI_PDMA_FLASH/blob/main/digit_7.jpg)	


