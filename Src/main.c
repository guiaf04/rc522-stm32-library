#include <stdint.h>
#include "stm32f1xx.h"
#include "rc522.h"



uint8_t status;
uint8_t str[MAX_LEN]; // Max_LEN = 16
uint8_t sNum[5];


void GPIO_Config() {
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;


	//PA4- CE,PA5-SCK,PA7-MOSI,PA6-MISO:

	//PA5 SCK will be set as alternate function output pushpull
	GPIOA->CRL |= GPIO_CRL_MODE5_0 | GPIO_CRL_MODE5_1;      	//Output Mode
	GPIOA->CRL |= GPIO_CRL_CNF5_1;         						//Alternate Function
	GPIOA->CRL &=  ~(GPIO_CRL_CNF5_0);
	//PA7 MOSI will be set as alternate function output pushpull
	GPIOA->CRL |= GPIO_CRL_MODE7_0 | GPIO_CRL_MODE7_1;     		//Output Mode
	GPIOA->CRL |= GPIO_CRL_CNF7_1;         						//Alternate Function
	GPIOA->CRL &=  ~(GPIO_CRL_CNF7_0);
	//PA4 CE will be set as General Purpose Output Mode
	GPIOA->CRL |= GPIO_CRL_MODE4_0 | GPIO_CRL_MODE4_1;     		//Output Mode
	GPIOA->CRL &= ~GPIO_CRL_CNF4_1;         					//General Purpose
	GPIOA->CRL &=  ~(GPIO_CRL_CNF4_0);  						//GPIOA->CRL &=  ~(GPIO_CRL_CNF4_0);
	//PA6 MISO will be set as Floating Input Mode
	GPIOA->CRL &= ~(GPIO_CRL_MODE6_0 | GPIO_CRL_MODE6_1);       //Input Mode
	GPIOA->CRL &= ~GPIO_CRL_CNF6_1;        					    //Floating Input
	GPIOA->CRL |=  (GPIO_CRL_CNF6_0);
	//PC13
	GPIOC->CRH = 0xFF0FFFFF;
	GPIOC->CRH = 0x00200000;
	GPIOC->ODR &= ~(1 << 13);
	//PB0
	GPIOB->CRL = 0xFFFFFFF0;
	GPIOB->CRL = 0x00000002;
	GPIOB->ODR &= ~(1 << 0);
}

void SPI_Init() {
	GPIO_Config();
	RCC->APB2ENR |= (1 << 12);      	// Enable SPI1 CLock
	SPI1->CR1 &= ~(1 << 0) | (1 << 1);  // CPOL=0, CPHA=0
	SPI1->CR1 |= (1 << 2);        		// Master Mode
	SPI1->CR1 |= (2 << 3);        		// BR[2:0] = 010: fPCLK/8, PCLK2 = 72MHz, SPI clk = 9MHz
	SPI1->CR1 &= ~(1 << 7);        		// LSBFIRST = 0, MSB first
	SPI1->CR1 |= (1 << 8) | (1 << 9);   // SSM=1, SSi=1 -> Software Slave Management
	SPI1->CR1 &= ~(1 << 10);        	// RXONLY = 0, full-duplex
	SPI1->CR1 &= ~(1 << 11);       		// DFF=0, 8 bit data
	SPI1->CR1 |= (1 << 6);        		// SPE=1, Peripheral enabled
}

int main() {
  SPI_Init();
  MFRC522_Init();

  while (1)
  {
    status = MFRC522_Request(PICC_REQIDL, str);
    status = MFRC522_Anticoll(str);

      if(!status){
         GPIOC->BSRR = (uint32_t)GPIO_PIN_13 << 16;
         for(int i = 0; i < 100000; i++);
       }else {
    	   GPIOC->BSRR = GPIO_PIN_13;
       }
  }
}