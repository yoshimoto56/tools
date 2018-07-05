#ifdef __cplusplus
extern "C"
{
#endif
	#include <stm32f4xx_hal_conf.h>
	#include <usbd_core.h>
	#include <usbd_cdc.h>
	#include "usbd_cdc_if.h"
	#include <usbd_desc.h>

	//TODO
	int speed = 100; //0-100 mm/s
	int stroke = 50; //0-150 mm

	USBD_HandleTypeDef USBD_Device;
	void SysTick_Handler(void);
	void OTG_FS_IRQHandler(void);
	void OTG_HS_IRQHandler(void);
	extern PCD_HandleTypeDef hpcd;	
	
	int VCP_read(void *pBuffer, int size);
	int VCP_write(const void *pBuffer, int size);
	extern char g_VCPInitialized;	
	
#ifdef __cplusplus
}
#endif

static void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	__PWR_CLK_ENABLE();
 
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
 
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);
 
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
}

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&hpcd);
}
#elif defined(USE_USB_HS)
void OTG_HS_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&hpcd);
}
#else
#error USB peripheral type not defined
#endif


//Digital Output
void ConfigureDO()
{
	__GPIOD_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;

	GPIO_InitStructure.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);	
	GPIO_InitStructure.Pin = GPIO_PIN_9;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);
}


//タイミングに合わせて出力
void syncout(int _p)
{
	char byte = NULL;	
	int count = 0;
	int count_max = stroke * 100;
	if (_p == -1)count = count_max;
	for (;;)
	{
		if (speed != 0)
		{
			if (count < count_max)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_SET);
				HAL_Delay(100.0 / speed);
			}
			else if (count_max <= count && count < 2 * count_max)
			{
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
				HAL_Delay(100.0 / speed);
			}
			count++;
			if (count == 2 * count_max)
			{
				count = 0;
			}
		}
		else{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);			
		}
		VCP_read(&byte, 1);
		if (byte=='q')
		{
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET);			
			VCP_write(">", 1);
			break;
		}		
	}
}

//https://github.com/gakuseishitsu/stm32_music_visualizer/blob/master/README.md

int main(void)
{
	HAL_Init();
	ConfigureDO();
	SystemClock_Config();

	USBD_Init(&USBD_Device, &VCP_Desc, 0);
	
	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, & USBD_CDC_LinearActuator_fops);
	USBD_Start(&USBD_Device);

	HAL_Delay(10000);
	VCP_write(">", 1);
	char byte;
	char buf[8];
	int cnt;
	for (;;)
	{
		cnt = 0;
		for (;;)
		{
			if (VCP_read(&byte, 1) == 0)
			{
				if(cnt!=6)
					continue;
			}
			buf[cnt] = byte;
			cnt++;
			if (cnt == 7) {
				break;
			}
		}
		char sgn;
		int p = 1;
		sscanf(buf, "%c%03d%03d", &sgn, &speed, &stroke);
		if (sgn == 'N' || sgn == 'n')
		{
			p = -1;
		}
		syncout(p);	
	}
}