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
	uint8_t phase_temp;
	//uint8_t lighting_alarm_reg_temp;

	uint8_t phase_a1_off_state=0;
	uint8_t phase_a2_off_state=0;
	uint8_t phase_b1_off_state=0;
	uint8_t phase_b2_off_state=0;
	uint8_t phase_c1_off_state=0;
	uint8_t phase_c2_off_state=0;

	uint8_t phase_a1_on_state=0;
	uint8_t phase_a2_on_state=0;
	uint8_t phase_b1_on_state=0;
	uint8_t phase_b2_on_state=0;
	uint8_t phase_c1_on_state=0;
	uint8_t phase_c2_on_state=0;

	uint8_t phase_a1_alarm_state=0;
	uint8_t phase_a2_alarm_state=0;
	uint8_t phase_b1_alarm_state=0;
	uint8_t phase_b2_alarm_state=0;
	uint8_t phase_c1_alarm_state=0;
	uint8_t phase_c2_alarm_state=0;

	osMutexWait(Fm25v02MutexHandle, osWaitForever); // обнуляем регистр статуса освещения и переменную регитра статуса освещения
	fm25v02_write(2*LIGHTING_STATUS_REG, 0x00);
	fm25v02_write(2*LIGHTING_STATUS_REG+1, 0x00);
	status_registers.lighting_status_reg = 0x0000;
	osMutexRelease(Fm25v02MutexHandle);


	for(;;)
	{
		if(control_registers.lighting_switching_reg == LIGHTING_ON) // если функция освещения включена
		{

			//if( HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET ) // если нет наличия фазы А1
			if( HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_4) == GPIO_PIN_RESET ) // если нет наличия фазы А1
			{
				if(phase_a1_alarm_state<100)
				{
					phase_a1_alarm_state++;
					if(phase_a1_alarm_state==100)
					{
						phase_a1_alarm_state = 0;

						if( ((status_registers.lighting_alarm_reg)&0x0001) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x01; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0001; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}

				if(phase_a1_off_state<50)
				{
					phase_a1_off_state++;
					if(phase_a1_off_state == 50)
					{
						phase_a1_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0001) == 0x0001 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xFE;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFFE;
							osMutexRelease(Fm25v02MutexHandle);
							LED3_OFF();
						}
					}
				}

			}
			//else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET) // если есть наличие фазы А1
			else if(HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_4) == GPIO_PIN_SET) // если есть наличие фазы А1
			{
				phase_a1_alarm_state = 0;
				phase_a1_off_state = 0;

				if(phase_a1_on_state<10)
				{
					phase_a1_on_state++;
					if(phase_a1_on_state==10)
					{
						//phase_a1_off_state = 0;
						phase_a1_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0001) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x01; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0001; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED3_ON();
						}

					}
				}

			}
			//if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
			//{
			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_RESET ) // если нет наличия фазы А2
			{
				if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
				{
				if(phase_a2_alarm_state<100)
				{
					phase_a2_alarm_state++;
					if(phase_a2_alarm_state==100)
					{
						phase_a2_alarm_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_alarm_reg)&0x0008) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x08; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0008; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}
				}

				if(phase_a2_off_state<50)
				{
					phase_a2_off_state++;
					if(phase_a2_off_state == 50)
					{
						phase_a2_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0008) == 0x0008 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xF7;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFF7;
							osMutexRelease(Fm25v02MutexHandle);
							LED6_OFF();
						}
					}
				}

			}
			else if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_SET) // если есть наличие фазы А2
			{

				phase_a2_alarm_state = 0;
				phase_a2_off_state = 0;

				if(phase_a2_on_state<10)
				{
					phase_a2_on_state++;
					if(phase_a2_on_state==10)
					{
						//phase_a2_off_state = 0;
						phase_a2_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0008) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x08; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0008; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED6_ON();
						}

					}
				}

			}

			//}
			/*
			else
			{
				osMutexWait(Fm25v02MutexHandle, osWaitForever);
				fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // вычитываем значение регистра статуса освещения
				phase_temp = phase_temp&0xC7; // обнуляем биты фазы А2,В2,С2
				fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем обнуленное значение в регистр статуса освещения
				status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFC7; // обнуляем значние битов А2,В2,С2 переменной регистра статуса освещения
				osMutexRelease(Fm25v02MutexHandle);
				LED6_OFF();
				LED7_OFF();
				LED8_OFF();
			}
			*/

			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_RESET ) // если нет наличия фазы В1
			{
				if(phase_b1_alarm_state<100)
				{
					phase_b1_alarm_state++;
					if(phase_b1_alarm_state==100)
					{
						phase_b1_alarm_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_alarm_reg)&0x0002) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x02; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0002; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}

				if(phase_b1_off_state<50)
				{
					phase_b1_off_state++;
					if(phase_b1_off_state == 50)
					{
						phase_b1_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0002) == 0x0002 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xFD;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFFD;
							osMutexRelease(Fm25v02MutexHandle);
							LED4_OFF();
						}
					}
				}
			}
			else if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_SET) // если есть наличие фазы В1
			{
				phase_b1_alarm_state = 0;
				phase_b1_off_state = 0;

				if(phase_b1_on_state<10)
				{
					phase_b1_on_state++;
					if(phase_b1_on_state==10)
					{
						//phase_b1_off_state = 0;
						phase_b1_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0002) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x02; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0002; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED4_ON();
						}

					}
				}
			}

			//if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
			//{
			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_RESET ) // если нет наличия фазы В2
			{
				if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
				{
				if(phase_b2_alarm_state<100)
				{
					phase_b2_alarm_state++;
					if(phase_b2_alarm_state==100)
					{
						phase_b2_alarm_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_alarm_reg)&0x0010) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x10; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0010; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}
				}

				if(phase_b2_off_state<50)
				{
					phase_b2_off_state++;
					if(phase_b2_off_state == 50)
					{
						phase_b2_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0010) == 0x0010 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xEF;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFEF;
							osMutexRelease(Fm25v02MutexHandle);
							LED7_OFF();
						}
					}
				}
			}
			else if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_SET) // если есть наличие фазы В2
			{
				phase_b2_alarm_state = 0;
				phase_b2_off_state = 0;

				if(phase_b2_on_state<10)
				{
					phase_b2_on_state++;
					if(phase_b2_on_state==10)
					{
						//phase_b2_off_state = 0;
						phase_b2_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0010) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x10; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0010; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED7_ON();
						}

					}
				}
			}

			//}

			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_RESET ) // если нет наличия фазы С1
			{
				if(phase_c1_alarm_state<100)
				{
					phase_c1_alarm_state++;
					if(phase_c1_alarm_state==100)
					{
						phase_c1_alarm_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_alarm_reg)&0x0004) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x04; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0004; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}

				if(phase_c1_off_state<50)
				{
					phase_c1_off_state++;
					if(phase_c1_off_state == 50)
					{
						phase_c1_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0004) == 0x0004 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xFB;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFFB;
							osMutexRelease(Fm25v02MutexHandle);
							LED5_OFF();
						}
					}
				}
			}
			else if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_SET) // если есть наличие фазы С1
			{
				phase_c1_alarm_state = 0;
				phase_c1_off_state = 0;

				if(phase_c1_on_state<10)
				{
					phase_c1_on_state++;
					if(phase_c1_on_state==10)
					{
						//phase_c1_off_state = 0;
						phase_c1_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0004) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x04; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0004; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED5_ON();
						}

					}
				}
			}

			//if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
			//{
			if( HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_RESET ) // если нет наличия фазы С2
			{
				if( ((control_registers.light_control_reg)&0x0010) == 0x0010 ) // если контроль второй линии включен
				{
				if(phase_c2_alarm_state<100)
				{
					phase_c2_alarm_state++;
					if(phase_c2_alarm_state==100)
					{
						phase_c2_alarm_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_alarm_reg)&0x0020) == 0x0000 ) // проверяем наличие бита аварии фазы А1, если бит не выставлен, устанавливаем бит
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_ALARM_REG+1, &phase_temp); // читаем значение из старшего байта регистра аварий освещения
							phase_temp = phase_temp|0x20; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_ALARM_REG+1, phase_temp); // записываем младший байт регистра аварий освещения
							status_registers.lighting_alarm_reg = status_registers.lighting_alarm_reg|0x0020; // выставляем бит аварии фазы А1 в переменной регистра аварий освещения
							osMutexRelease(Fm25v02MutexHandle);
						}

					}
				}
				}

				if(phase_c2_off_state<50)
				{
					phase_c2_off_state++;
					if(phase_c2_off_state == 50)
					{
						phase_c2_off_state = 0;
						if( ((status_registers.lighting_status_reg)&0x0020) == 0x0020 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp);
							phase_temp = phase_temp&0xDF;
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp);
							status_registers.lighting_status_reg = status_registers.lighting_status_reg&0xFFDF;
							osMutexRelease(Fm25v02MutexHandle);
							LED8_OFF();
						}
					}
				}
			}
			else if(HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_SET) // если нет наличия фазы С2
			{
				phase_c2_alarm_state = 0;
				phase_c2_off_state = 0;

				if(phase_c2_on_state<10)
				{
					phase_c2_on_state++;
					if(phase_c2_on_state==10)
					{
						//phase_c2_off_state = 0;
						phase_c2_on_state = 0; // выставляем среднее значение между 0 и 10

						if( ((status_registers.lighting_status_reg)&0x0020) == 0x0000 )
						{
							osMutexWait(Fm25v02MutexHandle, osWaitForever);
							fm25v02_read(2*LIGHTING_STATUS_REG+1, &phase_temp); // читаем значение регистра статуса освещения
							phase_temp = phase_temp|0x20; // устанавливаем 0-й бит статуса фазы А1
							fm25v02_write(2*LIGHTING_STATUS_REG+1, phase_temp); // записываем младший байт регистра статуса освещения
							status_registers.lighting_status_reg = status_registers.lighting_status_reg|0x0020; // выставляем бит фазы А1 в переменной регистра статуса освещения
							osMutexRelease(Fm25v02MutexHandle);
							LED8_ON();
						}

					}
				}
			}

			//}

		}


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
