#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

int N = 10;
SPI_HandleTypeDef    SPI_Init_user = {0};

void Initialize(void)
{
	 __HAL_RCC_GPIOA_CLK_ENABLE();
   __HAL_RCC_GPIOB_CLK_ENABLE();
   __HAL_RCC_GPIOD_CLK_ENABLE();
	
	//pin 5 A
	 GPIO_InitTypeDef GPIO_InitStructA = {0};
   GPIO_InitStructA.Pin = GPIO_PIN_5; 
   GPIO_InitStructA.Mode = GPIO_MODE_AF_PP;  
   GPIO_InitStructA.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStructA.Pull = GPIO_NOPULL; 
   HAL_GPIO_Init(GPIOA, &GPIO_InitStructA);
				 
	//pin 4,5 B
	 GPIO_InitTypeDef GPIO_InitStructB = {0};
   GPIO_InitStructB.Pin = GPIO_PIN_4|GPIO_PIN_5; 
   GPIO_InitStructB.Mode = GPIO_MODE_AF_PP;  
   GPIO_InitStructB.Speed = GPIO_SPEED_FREQ_LOW; 
   GPIO_InitStructB.Pull = GPIO_NOPULL; 
   HAL_GPIO_Init(GPIOB, &GPIO_InitStructB);
	 
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
	 
 //pin 7 D
	 GPIO_InitTypeDef GPIO_InitStructD = {0};
   GPIO_InitStructD.Pin = GPIO_PIN_7; 
   GPIO_InitStructD.Mode = GPIO_MODE_OUTPUT_PP; 
   GPIO_InitStructD.Speed = GPIO_SPEED_FREQ_LOW;
   GPIO_InitStructD.Pull = GPIO_NOPULL; 
   HAL_GPIO_Init(GPIOD, &GPIO_InitStructD);

  //spi
	__HAL_RCC_SPI1_CLK_ENABLE();
	SPI_Init_user.Instance = SPI1;
  SPI_Init_user.Init.Mode = SPI_MODE_MASTER;
  SPI_Init_user.Init.Direction = SPI_DIRECTION_2LINES;
  SPI_Init_user.Init.DataSize = SPI_DATASIZE_8BIT;
  SPI_Init_user.Init.CLKPolarity = SPI_POLARITY_HIGH;
  SPI_Init_user.Init.CLKPhase = SPI_PHASE_2EDGE;
  SPI_Init_user.Init.NSS = SPI_NSS_SOFT;
  SPI_Init_user.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  SPI_Init_user.Init.FirstBit = SPI_FIRSTBIT_MSB;
  SPI_Init_user.Init.TIMode = SPI_TIMODE_DISABLE;
  SPI_Init_user.Init.CRCCalculation = SPI_CRCCALCULATION_ENABLED;
  SPI_Init_user.Init.CRCPolynomial = 7;
  HAL_SPI_Init(&SPI_Init_user);
}

void InitializeClock()
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
}

void disableCS(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
}

void enableCS(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);
}

//Status register
uint8_t ReadRegister (void)
{
	//RSR
  enableCS();
	uint8_t buf=0x05;
	uint8_t status;
	HAL_SPI_TransmitReceive(&SPI_Init_user, &buf, &status, 1, 1000);
  disableCS();
	return status;
}

void WriteToRegister(uint8_t data)
{
	//ewsr
	uint8_t buf = 0x50;
  enableCS();
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
	enableCS();
	//wrsr
	buf = 0x01;
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	HAL_SPI_Transmit(&SPI_Init_user,&data,1,1000);
	disableCS();
}


void Clear()
{
	//wren
	enableCS();
	uint8_t buf=0x06;
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
	//chip erase
	enableCS();
	buf=0x60;
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
}

void WriteDataArray(uint32_t address, uint8_t* data)
{
	//wren
	enableCS();
	uint8_t buf = 0x06;
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
	enableCS();
	
	//aai
	buf = 0xAD; 
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
	enableCS();
	
  uint8_t address1 = address>>16;
  uint8_t address2 = address>>8;
	uint8_t address3 = address;
	HAL_SPI_Transmit(&SPI_Init_user,&address1,1,1000);
	HAL_SPI_Transmit(&SPI_Init_user,&address2,1,1000);
	HAL_SPI_Transmit(&SPI_Init_user,&address3,1,1000);
	
	HAL_SPI_Transmit(&SPI_Init_user,&data[0],1,1000);
	HAL_SPI_Transmit(&SPI_Init_user,&data[1],1,1000);
	disableCS();
	
	for(int i=1; i<N/2; ++i)
	{ 
	   HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	   HAL_SPI_Transmit(&SPI_Init_user,&data[i*2],1,1000);
		 HAL_SPI_Transmit(&SPI_Init_user,&data[i*2+1],1,1000);
		 disableCS();
	}	
	
  //wrdi
	buf = 0x04;
	enableCS();
	HAL_SPI_Transmit(&SPI_Init_user,&buf,1,1000);
	disableCS();
}
