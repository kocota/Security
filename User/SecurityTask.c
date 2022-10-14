#include "security.h"
#include "cmsis_os.h"
#include "fm25v02.h"
#include "gpio.h"
#include "m95.h"
#include "modbus.h"


extern uint8_t security_state;
extern osThreadId IbuttonTaskHandle;

uint8_t button_state=0;

void ThreadSecurityTask(void const * argument)
{

	fm25v02_read(SECURITY_STATUS_REG, &security_state);//Читаем байт состояния охранной сигнализации из памяти

	if( (security_state == DISABLED_BY_IBUTTON) || (security_state == DISABLED_BY_SERVER) || (security_state == RESERVED_0) )//Проверяем состояние охранной сигнаизации и включаем или выключаем светодиоды
	{
		for(uint8_t i=0; i<40; i++)
		{
			LED_OUT_TOGGLE();
			HAL_Delay(20);
		}
		LED2_OFF();
		LED_OUT_OFF();
	}
	else if( (security_state == ENABLED_BY_IBUTTON) || (security_state == ENABLED_BY_SERVER) )
	{
		for(uint8_t i=0; i<8; i++)
		{
			LED_OUT_TOGGLE();
			HAL_Delay(100);
		}
		LED2_ON();
		LED_OUT_ON();
	}

	osThreadResume(IbuttonTaskHandle);

	for(;;)
	{

		//fm25v02_read(SECURITY_STATUS_REG, &security_state);
		if( (security_state == ENABLED_BY_IBUTTON) || (security_state == ENABLED_BY_SERVER) ) // если режим охраны включен таблеткой или из центра
		{
			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_RESET)
			{
				if(button_state<10)
				{
					button_state++;
					if(button_state==10)
					{
						for(uint8_t i=0; i<40; i++)
						{
							LED_OUT_TOGGLE();
							HAL_Delay(200);
						}
						LED2_OFF();
						LED_OUT_OFF();
					}
				}

			}
			else
			{
				if(button_state>0)
				{
					button_state--;
					if(button_state==0)
					{
						for(uint8_t i=0; i<8; i++)
						{
							LED_OUT_TOGGLE();
							HAL_Delay(1000);
						}
						LED2_ON();
						LED_OUT_ON();
					}
				}

			}

		}




		osDelay(10);

	}
}
