#include "ArmingTask.h"
#include "cmsis_os.h"
#include "modbus.h"
#include "fm25v02.h"

extern osThreadId EventWriteTaskHandle;
extern osMutexId Fm25v02MutexHandle;
extern control_register_struct control_registers;
extern status_register_struct status_registers;
extern volatile uint8_t security_control_temp;

void ThreadArmingTask(void const * argument)
{
	uint16_t arming_time_counter = 0; // счетчик времени процесса постановки на охрану
	uint8_t control_loop_arming = 0; // переменная для сравнения с перечнем контролируемых шлейфов

	uint8_t button_state_arm1=0; // переменные состояния шлейфов при процессе постановки на охрану, если переменная примет значение 10, значит шлейф замкнут
	uint8_t button_state_arm2=0;
	uint8_t button_state_arm3=0;
	uint8_t button_state_arm4=0;
	uint8_t button_state_arm5=0;
	uint8_t button_state_arm6=0;
	uint8_t button_state_arm7=0;
	uint8_t button_state_arm8=0;

	uint8_t state_arm1 = 0;
	uint8_t state_arm2 = 0;
	uint8_t state_arm3 = 0;
	uint8_t state_arm4 = 0;
	uint8_t state_arm5 = 0;
	uint8_t state_arm6 = 0;
	uint8_t state_arm7 = 0;
	uint8_t state_arm8 = 0;



	for(;;)
	{

		if( status_registers.security_status_reg == ARMING_PROCESS ) // если запущен процесс потановки на охрану
		{
			//----Контроль шлейфа №1--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x01) != 0x00 ) )
			{
				if(button_state_arm1<10){button_state_arm1++;}
				else if(button_state_arm1>=10){state_arm1=1;}

			}
			else if( (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x01) != 0x00 ) )
			{
				if(button_state_arm1>0){button_state_arm1=0; state_arm1=0;}

			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №2--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x02) != 0x00 ) )
			{
				if(button_state_arm2<10){button_state_arm2++;}
				else if(button_state_arm2>=10){state_arm2=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_11) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x02) != 0x00 ) )
			{
				if(button_state_arm2>0){button_state_arm2=0; state_arm2=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №3--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x04) != 0x00 ) )
			{
				if(button_state_arm3<10){button_state_arm3++;}
				else if(button_state_arm3>=10){state_arm3=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_12) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x04) != 0x00 ) )
			{
				if(button_state_arm3>0){button_state_arm3=0; state_arm3=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №4--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x08) != 0x00 ) )
			{
				if(button_state_arm4<10){button_state_arm4++;}
				else if(button_state_arm4>=10){state_arm4=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_13) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x08) != 0x00 ) )
			{
				if(button_state_arm4>0){button_state_arm4=0; state_arm4=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №5--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x10) != 0x00 ) )
			{
				if(button_state_arm5<10){button_state_arm5++;}
				else if(button_state_arm5>=10){state_arm5=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_14) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x10) != 0x00 ) )
			{
				if(button_state_arm5>0){button_state_arm5=0; state_arm5=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №6--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x20) != 0x00 ) )
			{
				if(button_state_arm6<10){button_state_arm6++;}
				else if(button_state_arm6>=10){state_arm6=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOF, GPIO_PIN_15) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x20) != 0x00 ) )
			{
				if(button_state_arm6>0){button_state_arm6=0; state_arm6=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №7--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_0) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x40) != 0x00 ) )
			{
				if(button_state_arm7<10){button_state_arm7++;}
				else if(button_state_arm7>=10){state_arm7=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_0) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x40) != 0x00 ) )
			{
				if(button_state_arm7>0){button_state_arm7=0; state_arm7=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			//----Контроль шлейфа №8--------------------------------------------------------------------------------------------------------------------------------
			if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_SET) && ( (control_registers.control_loop_reg&0x80) != 0x00 ) )
			{
				if(button_state_arm8<10){button_state_arm8++;}
				else if(button_state_arm8>=10){state_arm8=1;}
			}
			else if( (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_1) == GPIO_PIN_RESET) && ( (control_registers.control_loop_reg&0x80) != 0x00 ) )
			{
				if(button_state_arm8>0){button_state_arm8=0; state_arm8=0;}
			}
			//--------------------------------------------------------------------------------------------------------------------------------------------------

			arming_time_counter++;
			if( arming_time_counter >800)
			{
				arming_time_counter = 0;
				control_loop_arming = 0;
				control_loop_arming = state_arm1|(state_arm2<<1)|(state_arm3<<2)|(state_arm4<<3)|(state_arm5<<4)|(state_arm6<<5)|(state_arm7<<6)|(state_arm8<<7);
				if( control_loop_arming == control_registers.control_loop_reg)
				{
					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*SECURITY_CONTROL_REG, 0x00);
					fm25v02_write(2*SECURITY_CONTROL_REG+1, SECURITY_CONTROL_DEFAULT);
					fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
					fm25v02_write(2*SECURITY_STATUS_REG+1, security_control_temp);
					osMutexRelease(Fm25v02MutexHandle);

					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*GPRS_CALL_REG, 0x00);
					fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
					osMutexRelease(Fm25v02MutexHandle);

					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
					fm25v02_write(2*SYSTEM_STATUS_REG+1, TURN_ON_STATE_ALARM);
					status_registers.system_status_reg = TURN_ON_STATE_ALARM;
					osMutexRelease(Fm25v02MutexHandle);

					osThreadResume(EventWriteTaskHandle);
				}
				else if( control_loop_arming != control_registers.control_loop_reg)
				{
					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*SECURITY_CONTROL_REG, 0x00);
					fm25v02_write(2*SECURITY_CONTROL_REG+1, SECURITY_CONTROL_DEFAULT);
					fm25v02_write(2*SECURITY_STATUS_REG, 0x00);
					fm25v02_write(2*SECURITY_STATUS_REG+1, ARMING_ERROR);
					osMutexRelease(Fm25v02MutexHandle);

					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*GPRS_CALL_REG, 0x00);
					fm25v02_write(2*GPRS_CALL_REG+1, CALL_ON);
					osMutexRelease(Fm25v02MutexHandle);

					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_write(2*SYSTEM_STATUS_REG, 0x00);
					fm25v02_write(2*SYSTEM_STATUS_REG+1, TURN_ON_ALARM_ERROR);
					status_registers.system_status_reg = TURN_ON_ALARM_ERROR;
					osMutexRelease(Fm25v02MutexHandle);

					osThreadResume(EventWriteTaskHandle);
				}
			}
		}


		osDelay(10); // вызываем процесс один раз в 10 мс
	}
}
