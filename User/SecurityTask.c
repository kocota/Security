#include "SecurityTask.h"
#include "cmsis_os.h"
#include "fm25v02.h"
#include "gpio.h"
#include "m95.h"
#include "modbus.h"


extern osTimerId Ring_Center_TimerHandle;
extern uint8_t security_state;
extern osThreadId IbuttonTaskHandle;
extern osMutexId Fm25v02MutexHandle;
//extern osMutexId UartMutexHandle;
extern status_register_struct status_registers;
//extern control_register_struct control_registers;

uint8_t button_state=0;

void ThreadSecurityTask(void const * argument)
{
	uint8_t alarm_loop_reg_temp;

	osMutexWait(Fm25v02MutexHandle, osWaitForever);
	fm25v02_read(SECURITY_STATUS_REG, &security_state);//Читаем байт состояния охранной сигнализации из памяти
	osMutexRelease(Fm25v02MutexHandle);
	status_registers.security_status_reg = security_state;

	if( (status_registers.security_status_reg == DISABLED_BY_IBUTTON) || (status_registers.security_status_reg == DISABLED_BY_SERVER) || (status_registers.security_status_reg == RESERVED_0) )//Проверяем состояние охранной сигнаизации и включаем или выключаем светодиоды
	{
		for(uint8_t i=0; i<40; i++)
		{
			LED_OUT_TOGGLE();
			HAL_Delay(200);
		}
		LED2_OFF();
		LED_OUT_OFF();
	}
	else if( (status_registers.security_status_reg == ENABLED_BY_IBUTTON) || (status_registers.security_status_reg == ENABLED_BY_SERVER) )
	{
		for(uint8_t i=0; i<8; i++)
		{
			LED_OUT_TOGGLE();
			HAL_Delay(1000);
		}
		LED2_ON();
		LED_OUT_ON();
	}

	osThreadResume(IbuttonTaskHandle);

	for(;;)
	{

		//(control_registers.control_loop_reg)&0x01
		if( (status_registers.security_status_reg == ENABLED_BY_IBUTTON) || (status_registers.security_status_reg == ENABLED_BY_SERVER) ) // если режим охраны включен таблеткой или из центра
		{
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_RESET) /*&& ( (control_registers.control_loop_reg)&0x04 != 0x00 )*/ )
			{
				if(button_state<10)
				{
					button_state++;
					if(button_state==10)
					{
						BUZ_ON();
						//LED8_ON();


						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(ALARM_LOOP_REG, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x04;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(ALARM_LOOP_REG, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);
						/*
			  			osMutexWait(UartMutexHandle, osWaitForever);
			  			request_to_server();
			  			osMutexRelease(UartMutexHandle);
			  			*/
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(GPRS_CALL_REG, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);
						//osTimerStart(Ring_Center_TimerHandle, 1);
						/*
						for(uint8_t i=0; i<40; i++)
						{
							LED_OUT_TOGGLE();
							HAL_Delay(200);
						}
						LED2_OFF();
						LED_OUT_OFF();
						*/
					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_SET) /*&& ( (control_registers.control_loop_reg)&0x04 != 0x00 )*/ )
			{
				if(button_state>0)
				{
					button_state--;
					if(button_state==0)
					{

						/*
						for(uint8_t i=0; i<8; i++)
						{
							LED_OUT_TOGGLE();
							HAL_Delay(1000);
						}
						LED2_ON();
						LED_OUT_ON();
						*/
					}
				}

			}

		}




		osDelay(10);

	}
}
