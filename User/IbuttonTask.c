#include "stm32f4xx_hal.h"
#include "IbuttonTask.h"
#include "SecurityTask.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "ibutton.h"
#include "modbus.h"


extern volatile uint8_t security_state;
extern osTimerId Ring_Center_TimerHandle;
//extern RTC_HandleTypeDef hrtc;
//extern RTC_TimeTypeDef security_time;
//extern RTC_DateTypeDef security_date;
extern osThreadId IbuttonTaskHandle;
//extern osMutexId UartMutexHandle;
extern osMutexId Fm25v02MutexHandle;



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
					ibutton_temp[0] = (uint8_t)(IbuttonROM.IbuttonROM_Low);
					ibutton_temp[1] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>8);
					ibutton_temp[2] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>16);
					ibutton_temp[3] = (uint8_t)(IbuttonROM.IbuttonROM_Low>>24);
					ibutton_temp[4] = (uint8_t)(IbuttonROM.IbuttonROM_High);
					ibutton_temp[5] = (uint8_t)(IbuttonROM.IbuttonROM_High>>8);
					ibutton_temp[6] = (uint8_t)(IbuttonROM.IbuttonROM_High>>16);
					ibutton_temp[7] = (uint8_t)(IbuttonROM.IbuttonROM_High>>24);

					osMutexWait(Fm25v02MutexHandle, osWaitForever);
					fm25v02_read(SECURITY_STATUS_REG, &security_state);//Читаем SECURITY_STATE_BYTE, хранящий состояние охранной сигнализации, из памяти в переменную security_state
					osMutexRelease(Fm25v02MutexHandle);

					if( (security_state == DISABLED_BY_IBUTTON) || (security_state == DISABLED_BY_SERVER) || (security_state == RESERVED_0) )//Условие если сигнализация выключена
			  		{
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(SECURITY_STATUS_REG, ENABLED_BY_IBUTTON);
						osMutexRelease(Fm25v02MutexHandle);

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

						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
			  			//fm25v02_write(GPRS_CALL_REG, 0x01); // записываем в память флаг, что устройство делает запрос на сервер
			  			//osMutexRelease(Fm25v02MutexHandle);
			  			/*
			  			osMutexWait(UartMutexHandle, osWaitForever); // отправляем запрос на сервер
			  			request_to_server();
			  			osMutexRelease(UartMutexHandle);
			  			*/
			  			//NVIC_SystemReset();
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(GPRS_CALL_REG, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);
						osTimerStart(Ring_Center_TimerHandle, 10);

			  		}
					else if( (security_state == ENABLED_BY_IBUTTON) || (security_state == ENABLED_BY_SERVER) )
			  		{
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
			  			fm25v02_write(SECURITY_STATUS_REG, DISABLED_BY_IBUTTON);
			  			osMutexRelease(Fm25v02MutexHandle);

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

						//osMutexWait(Fm25v02MutexHandle, osWaitForever);
			  			//fm25v02_write(GPRS_CALL_REG, 0x01); // записываем в память флаг, что устройство делает запрос на сервер
			  			//osMutexRelease(Fm25v02MutexHandle);
			  			/*
			  			osMutexWait(UartMutexHandle, osWaitForever); // отправляем запрос на сервер
			  			request_to_server();
			  			osMutexRelease(UartMutexHandle);
			  			*/
						osMutexWait(Fm25v02MutexHandle, osWaitForever);
						fm25v02_write(GPRS_CALL_REG, CALL_ON);
						osMutexRelease(Fm25v02MutexHandle);
						osTimerStart(Ring_Center_TimerHandle, 10);

			  		}
				}
			}

				/*{
					fm25v02_read(SECURITY_STATE_BYTE, &security_state);//Читаем SECURITY_STATE_BYTE, хранящий состояние охранной сигнализации, из памяти в переменную security_state
					if( security_state == SECURITY_OFF)//Условие если сигнализация выключена
			  		{
						fm25v02_write(SECURITY_STATE_BYTE, SECURITY_ON);
			  			HAL_RTC_GetTime(&hrtc, &security_time , RTC_FORMAT_BCD);
			  			security_time.Hours = RTC_Bcd2ToByte(security_time.Hours);
			  			security_time.Minutes = RTC_Bcd2ToByte(security_time.Minutes);
			  			security_time.Seconds = RTC_Bcd2ToByte(security_time.Seconds);

			  			HAL_RTC_GetDate(&hrtc, &security_date, RTC_FORMAT_BCD);
			  			security_date.Date = RTC_Bcd2ToByte(security_date.Date);
			  			security_date.Month = RTC_Bcd2ToByte(security_date.Month);
			  			security_date.Year = RTC_Bcd2ToByte(security_date.Year);
			  			if( IbuttonROM.IbuttonROM_Low == 0x86075F01)
			  				{
			  					printf("Security ON by %s   time: %u:%u:%u %u/%u/%u \n", "Yagin Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
			  			  	}
			  			if( IbuttonROM.IbuttonROM_Low == 0x5AD95901)
			  			  	{
			  					printf("Security ON by %s   time: %u:%u:%u %u/%u/%u \n", "Banshikov Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
			  			  	}

			  			BUZ_ON();
			  			HAL_Delay(100);
			  			BUZ_OFF();
			  			for(uint8_t i=0; i<8; i++)
			  			{
			  				LED_OUT_TOGGLE();
			  				HAL_Delay(1000);
			  			}
			  			LED2_ON();
			  			LED_OUT_ON();
			  		}
					else
			  		{
			  			fm25v02_write(SECURITY_STATE_BYTE, SECURITY_OFF);
			  			HAL_RTC_GetTime(&hrtc, &security_time , RTC_FORMAT_BCD);
			  			security_time.Hours = RTC_Bcd2ToByte(security_time.Hours);
			  			security_time.Minutes = RTC_Bcd2ToByte(security_time.Minutes);
			  			security_time.Seconds = RTC_Bcd2ToByte(security_time.Seconds);

			  			HAL_RTC_GetDate(&hrtc, &security_date, RTC_FORMAT_BCD);
			  			security_date.Date = RTC_Bcd2ToByte(security_date.Date);
			  			security_date.Month = RTC_Bcd2ToByte(security_date.Month);
			  			security_date.Year = RTC_Bcd2ToByte(security_date.Year);
			  			if( IbuttonROM.IbuttonROM_Low == 0x86075F01)
			  				{
			  					printf("Security OFF by %s   time: %u:%u:%u %u/%u/%u \n", "Yagin Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
			  			  	}
			  			if( IbuttonROM.IbuttonROM_Low == 0x5AD95901)
			  			  	{
			  				  	printf("Security OFF by %s   time: %u:%u:%u %u/%u/%u \n", "Banshikov Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
			  			  	}

			  			BUZ_ON();
			  			HAL_Delay(100);
			  			BUZ_OFF();
			  			for(uint8_t i=0; i<40; i++)
			  			{
			  				LED_OUT_TOGGLE();
			  				HAL_Delay(200);
			  			}
			  			LED2_OFF();
			  			LED_OUT_OFF();
			  		  }
				}*/


		}

		osDelay(10);
	}
}
//------------------------------------------------------------


/*
	HAL_RTC_GetTime(&hrtc, &security_time , RTC_FORMAT_BCD);
	security_time.Hours = RTC_Bcd2ToByte(security_time.Hours);
	security_time.Minutes = RTC_Bcd2ToByte(security_time.Minutes);
	security_time.Seconds = RTC_Bcd2ToByte(security_time.Seconds);

	HAL_RTC_GetDate(&hrtc, &security_date, RTC_FORMAT_BCD);
	security_date.Date = RTC_Bcd2ToByte(security_date.Date);
	security_date.Month = RTC_Bcd2ToByte(security_date.Month);
	security_date.Year = RTC_Bcd2ToByte(security_date.Year);
	if( IbuttonROM.IbuttonROM_Low == 0x86075F01)
		{
			printf("Security ON by %s   time: %u:%u:%u %u/%u/%u \n", "Yagin Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
	  	}
	if( IbuttonROM.IbuttonROM_Low == 0x5AD95901)
	  	{
			printf("Security ON by %s   time: %u:%u:%u %u/%u/%u \n", "Banshikov Aleksandr", security_time.Hours, security_time.Minutes, security_time.Seconds, security_date.Date, security_date.Month, security_date.Year);
	  	}
	*/

