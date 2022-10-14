#include "IbuttonTask.h"
#include "cmsis_os.h"
#include "gpio.h"
#include "ibutton.h"
#include "stdio.h"
#include "fm25v02.h"
#include "security.h"
#include "m95.h"

extern volatile uint8_t security_state;
extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef security_time;
extern RTC_DateTypeDef security_date;
extern osThreadId IbuttonTaskHandle;
extern osMutexId UartMutexHandle;

uint8_t t_data[50];




IbuttonROM_Struct IbuttonROM;
uint32_t t;


//Поток обработки ibutton и постановки на сигнализацию-------------------------
void ThreadIbuttonTask(void const * argument)
{
	osThreadSuspend(IbuttonTaskHandle);

	printf("%lu \n", HAL_GetTick());
	fm25v02_fast_read(0x0000, t_data, 25);
	printf("%lu \n", HAL_GetTick());

	IbuttonROM.IbuttonROM_High = 0x92000A04;
	IbuttonROM.IbuttonROM_Low = 0x5AD95901;

	printf("%lu \n", HAL_GetTick());
	ibutton_delete_rom(&IbuttonROM);
	printf("%lu \n", HAL_GetTick());





	for(;;)
	{
		if( ibutton_reset() == HAL_OK )// Инициализируем ibutton таблетку, проверяем отвечает ли она на сброс, если отвечает, то выполняем чтение ROM
		{

			if( ibutton_read_rom(&IbuttonROM) == HAL_OK ) // Считываем ROM-код таблетки
			{
				/*
				if( ibutton_write_rom(&IbuttonROM) == HAL_OK )
				{
					BUZ_ON();
					HAL_Delay(100);
					BUZ_OFF();
				}
				*/

				if( ibutton_search_rom(&IbuttonROM) == HAL_OK )
				{
					//LED_VD4_TOGGLE();
					fm25v02_read(SECURITY_STATUS_REG, &security_state);//Читаем SECURITY_STATE_BYTE, хранящий состояние охранной сигнализации, из памяти в переменную security_state
					if( (security_state == DISABLED_BY_IBUTTON) || (security_state == DISABLED_BY_SERVER) || (security_state == RESERVED_0) )//Условие если сигнализация выключена
			  		{
						fm25v02_write(SECURITY_STATUS_REG, ENABLED_BY_IBUTTON);

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

			  			osMutexWait(UartMutexHandle, osWaitForever);
			  			request_to_server();
			  			osMutexRelease(UartMutexHandle);

			  		}
					else if( (security_state == ENABLED_BY_IBUTTON) || (security_state == ENABLED_BY_SERVER) )
			  		{
			  			fm25v02_write(SECURITY_STATUS_REG, DISABLED_BY_IBUTTON);

			  			/*r
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
			  			*/

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

			  			osMutexWait(UartMutexHandle, osWaitForever);
			  			request_to_server();
			  			osMutexRelease(UartMutexHandle);

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

