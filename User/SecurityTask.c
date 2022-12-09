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
extern osThreadId EventWriteTaskHandle;

extern status_register_struct status_registers;
extern control_register_struct control_registers;

uint8_t button_state1=0;
uint8_t button_state2=0;
uint8_t button_state3=0;
uint8_t button_state4=0;
uint8_t button_state5=0;
uint8_t button_state6=0;
uint8_t button_state7=0;
uint8_t button_state8=0;

void ThreadSecurityTask(void const * argument)
{
	uint8_t alarm_loop_reg_temp;


	osMutexWait(Fm25v02MutexHandle, osWaitForever); // обнуляем регистр статуса освещения и переменную регитра статуса освещения
	fm25v02_write(2*LIGHTING_STATUS_REG, 0x00);
	fm25v02_write(2*LIGHTING_STATUS_REG+1, 0x00);
	status_registers.lighting_status_reg = 0x0000;
	osMutexRelease(Fm25v02MutexHandle);


	for(;;)
	{

		if( (status_registers.security_status_reg == ENABLED_BY_IBUTTON) || (status_registers.security_status_reg == ENABLED_BY_SERVER) ) // если режим охраны включен таблеткой или из центра
		{

			//----Контроль шлейфа №1--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x01) != 0x00 ) )
			{
				if(button_state1<10)
				{
					button_state1++;
					if(button_state1==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x01;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x01) != 0x00 ) )
			{
				if(button_state1>0)
				{
					button_state1 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №2--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x02) != 0x00 ) )
			{
				if(button_state2<10)
				{
					button_state2++;
					if(button_state2==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x02;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x02) != 0x00 ) )
			{
				if(button_state2>0)
				{
					button_state2 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №3--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x04) != 0x00 ) )
			{
				if(button_state3<10)
				{
					button_state3++;
					if(button_state3==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x04;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x04) != 0x00 ) )
			{
				if(button_state3>0)
				{
					button_state3 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №4--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x08) != 0x00 ) )
			{
				if(button_state4<10)
				{
					button_state4++;
					if(button_state4==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x08;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x08) != 0x00 ) )
			{
				if(button_state4>0)
				{
					button_state4 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №5--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x10) != 0x00 ) )
			{
				if(button_state5<10)
				{
					button_state5++;
					if(button_state5==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x10;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x10) != 0x00 ) )
			{
				if(button_state5>0)
				{
					button_state5 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №6--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x20) != 0x00 ) )
			{
				if(button_state6<10)
				{
					button_state6++;
					if(button_state6==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x20;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x20) != 0x00 ) )
			{
				if(button_state6>0)
				{
					button_state6 == 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №7--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_0) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x40) != 0x00 ) )
			{
				if(button_state7<10)
				{
					button_state7++;
					if(button_state7==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x40;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_0) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x40) != 0x00 ) )
			{
				if(button_state7>0)
				{
					button_state7 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №8--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x80) != 0x00 ) )
			{
				if(button_state8<10)
				{
					button_state8++;
					if(button_state8==10)
					{
						if(control_registers.mute_reg == MUTE_OFF)
						{
							BUZ_ON();
						}

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_read(2*ALARM_LOOP_REG+1, &alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						alarm_loop_reg_temp = alarm_loop_reg_temp|0x80;

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*ALARM_LOOP_REG, 0x00);
						fm25v02_write(2*ALARM_LOOP_REG+1, alarm_loop_reg_temp);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
						fm25v02_write(2*SECURITY_STATUS_REG+1, DOOR_OPEN_ALARM);
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
						fm25v02_write(2*SYSTEM_STATUS_REG+1, ALARM_STATE);
						status_registers.system_status_reg = ALARM_STATE;
						osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(2*GPRS_CALL_REG, 0x00);
						fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);

						osThreadResume(EventWriteTaskHandle);

					}
				}

			}
			else if ( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x80) != 0x00 ) )
			{
				if(button_state8>0)
				{
					button_state8 = 0;
					LED5_OFF();

				}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------



		}

		osDelay(10);

	}
}
