#include "stm32f4xx_hal.h"
#include "IbuttonTask.h"
#include "SecurityTask.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "ibutton.h"
#include "modbus.h"
#include "fm25v02.h"


extern volatile uint8_t security_state;
extern osTimerId Ring_Center_TimerHandle;
//extern RTC_HandleTypeDef hrtc;
//extern RTC_TimeTypeDef security_time;
//extern RTC_DateTypeDef security_date;
extern osThreadId IbuttonTaskHandle;
//extern osMutexId UartMutexHandle;
extern osMutexId Fm25v02MutexHandle;
extern status_register_struct status_registers;
extern control_register_struct control_registers;

extern volatile uint8_t security_state_temp;



uint8_t t_data[50];




IbuttonROM_Struct IbuttonROM;
uint32_t t;


//Поток обработки ibutton и постановки на сигнализацию-------------------------
void ThreadIbuttonTask(void const * argument)
{
	uint8_t ibutton_temp[8];
	osThreadSuspend(IbuttonTaskHandle);




	for(;;)
	{
		if( ibutton_reset() == HAL_OK )// Инициализируем ibutton таблетку, проверяем отвечает ли она на сброс, если отвечает, то выполняем чтение ROM
		{

			if( ibutton_read_rom(&IbuttonROM) == HAL_OK ) // Считываем ROM-код таблетки
			{
				if( (ibutton_search_rom(&IbuttonROM) == HAL_OK) && (IbuttonROM.IbuttonROM_High != 0) && (IbuttonROM.IbuttonROM_Low != 0) )
				{

		  			BUZ_ON();
		  			HAL_Delay(20);
		  			BUZ_OFF();

					ibutton_temp[0] = (uint8_t)(IbuttonROM.IbuttonROM_Low); // записываем во временные переменные номер найденной таблетки
					ibutton_temp[1] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>8);
					ibutton_temp[2] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>16);
					ibutton_temp[3] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>24);
					ibutton_temp[4] = (uint8_t)(IbuttonROM.IbuttonROM_High);
					ibutton_temp[5] = (uint8_t)(IbuttonROM.IbuttonROM_High>>8);
					ibutton_temp[6] = (uint8_t)(IbuttonROM.IbuttonROM_High>>16);
					ibutton_temp[7] = (uint8_t)(IbuttonROM.IbuttonROM_High>>24);

					//osMutexWait(Fm25v02MutexHandle, osWaitForever);
					//fm25v02_read(SECURITY_STATUS_REG, &security_state);//Читаем SECURITY_STATE_BYTE, хранящий состояние охранной сигнализации, из памяти в переменную security_state
					//osMutexRelease(Fm25v02MutexHandle);

					//if( (security_state == DISABLED_BY_IBUTTON) || (security_state == DISABLED_BY_SERVER) || (security_state == RESERVED_0) )//Условие если сигнализация выключена
					if( (status_registers.security_status_reg == DISABLED_BY_IBUTTON) || (status_registers.security_status_reg == DISABLED_BY_SERVER) || (status_registers.security_status_reg == RESERVED_0) ) // Если сигнализация выключена, ставим на охрану
			  		{
						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
						//fm25v02_write(SECURITY_STATUS_REG, ENABLED_BY_IBUTTON);
						//osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(IBUTTON_COMPLETE_0_REG, ibutton_temp[0]);
						fm25v02_write(IBUTTON_COMPLETE_1_REG, ibutton_temp[1]);
						fm25v02_write(IBUTTON_COMPLETE_2_REG, ibutton_temp[2]);
						fm25v02_write(IBUTTON_COMPLETE_3_REG, ibutton_temp[3]);
						fm25v02_write(IBUTTON_COMPLETE_4_REG, ibutton_temp[4]);
						fm25v02_write(IBUTTON_COMPLETE_5_REG, ibutton_temp[5]);
						fm25v02_write(IBUTTON_COMPLETE_6_REG, ibutton_temp[6]);
						fm25v02_write(IBUTTON_COMPLETE_7_REG, ibutton_temp[7]);
						osMutexRelease(Fm25v02MutexHandle);

						//security_control_temp = ENABLE_FROM_IBUTTON; // записываем команду в глобальную переменную управления охраной

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(SECURITY_CONTROL_REG, ENABLE_FROM_IBUTTON); // запускаем процесс постановки на охрану
						osMutexRelease(Fm25v02MutexHandle);

						control_registers.security_control_reg = ENABLE_FROM_IBUTTON;

						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
						//fm25v02_write(GPRS_CALL_REG, CALL_ON);
						//osMutexRelease(Fm25v02MutexHandle);

						HAL_Delay(10000); // делаем паузу
						/*
			  			BUZ_ON(); // пикаем зуммером
			  			HAL_Delay(100);
			  			BUZ_OFF();

			  			for(uint8_t i=0; i<8; i++) // моргаем контрольным светодиодом
			  			{
			  				LED_OUT_TOGGLE();
			  				HAL_Delay(1000);
			  			}

			  			LED2_ON();
			  			LED_OUT_ON();
			  			*/


			  		}
					//else if( (security_state == ENABLED_BY_IBUTTON) || (security_state == ENABLED_BY_SERVER) )
					else if( (status_registers.security_status_reg == ENABLED_BY_IBUTTON) || (status_registers.security_status_reg == ENABLED_BY_SERVER) || ( status_registers.security_status_reg == DOOR_OPEN_ALARM ) ) // если сигнализация включена, снимаем с охраны
			  		{
						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
			  			//fm25v02_write(SECURITY_STATUS_REG, DISABLED_BY_IBUTTON);
			  			//osMutexRelease(Fm25v02MutexHandle);

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(IBUTTON_COMPLETE_0_REG, ibutton_temp[0]);
						fm25v02_write(IBUTTON_COMPLETE_1_REG, ibutton_temp[1]);
						fm25v02_write(IBUTTON_COMPLETE_2_REG, ibutton_temp[2]);
						fm25v02_write(IBUTTON_COMPLETE_3_REG, ibutton_temp[3]);
						fm25v02_write(IBUTTON_COMPLETE_4_REG, ibutton_temp[4]);
						fm25v02_write(IBUTTON_COMPLETE_5_REG, ibutton_temp[5]);
						fm25v02_write(IBUTTON_COMPLETE_6_REG, ibutton_temp[6]);
						fm25v02_write(IBUTTON_COMPLETE_7_REG, ibutton_temp[7]);
						osMutexRelease(Fm25v02MutexHandle);

						//security_control_temp = DISABLE_FROM_IBUTTON; // записываем команду в глобальную переменную управления охраной

						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(SECURITY_CONTROL_REG, DISABLE_FROM_IBUTTON); // запускаем процесс постановки на охрану
						osMutexRelease(Fm25v02MutexHandle);

						control_registers.security_control_reg = DISABLE_FROM_IBUTTON;

						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
						//fm25v02_write(GPRS_CALL_REG, CALL_ON);
						//osMutexRelease(Fm25v02MutexHandle);

						HAL_Delay(10000);

						/*
			  			BUZ_ON(); // пикаем зуммером
			  			HAL_Delay(100);
			  			BUZ_OFF();

			  			for(uint8_t i=0; i<40; i++) // моргаем контрольным светодиодом
			  			{
			  				LED_OUT_TOGGLE();
			  				HAL_Delay(200);
			  			}

			  			LED2_OFF();
			  			LED_OUT_OFF();
			  			*/

			  		}

					else if( status_registers.security_status_reg == ARMING_ERROR )
					{
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
			  			fm25v02_write(SECURITY_STATUS_REG, security_state_temp);
			  			osMutexRelease(Fm25v02MutexHandle);

			  			status_registers.security_status_reg = security_state_temp;

			  			HAL_Delay(10000);

					}



				}
			}


		}

		osDelay(10);
	}
}
//------------------------------------------------------------


