#ifndef M95
#define M95

#define ACTIVE     1
#define NOT_ACTIVE 0

#define AT_OK    0
#define AT_ERROR 1

#define TCP 0
#define UDP 1

#define IP_INITIAL     2
#define IP_START       3
#define IP_CONFIG      4
#define IP_IND         5
#define IP_GPRSACT     6
#define IP_STATUS      7
#define TCP_CONNECTING 8
#define UDP_CONNECTING 9
#define IP_CLOSE       10
#define CONNECT_OK     11
#define PDP_DEACT      12

// Описание регистров статуса-----------------------------
#define VERSION_REG                      0x1000 // (4096) версия прошивки контроллера
#define SECURITY_REG                     0x1001 // (4097) статус режима охраны
#define STATUS_LOOP_REG                  0x1002 // (4098) состояние шлейфов сигнализации
#define ERROR_LOOP_REG                   0x1003 // (4099) неисправные шлейфы
#define ALARM_LOOP_REG                   0x1004 // (4100) сработавшие шлейфы
#define TIME_CURRENT_YEAR_REG            0x1005 // (4101) текущий год
#define TIME_CURRENT_MONTH_REG           0x1006 // (4102) текущий месяц
#define TIME_CURRENT_DAY_REG             0x1007 // (4103) текущий день
#define TIME_CURRENT_HOUR_REG
#define TIME_CURRENT_MINUTE_REG
#define TIME_CURRENT_SECOND_REG
#define TIME_CURRENT_WEEKDAY_REG
#define ADDRESS_PROCESSED_EVENT_H_REG
#define ADDRESS_PROCESSED_EVENT_L_REG
#define ADDRESS_LAST_EVENT_H_REG
#define ADDRESS_LAST_EVENT_L_REG
#define SYSTEM_STATUS_REG
#define POWER_ON_REG
#define ERROR_RTC_REG
#define IBUTTON_COMPLETE_0_REG
#define IBUTTON_COMPLETE_1_REG
#define IBUTTON_COMPLETE_2_REG
#define IBUTTON_COMPLETE_3_REG
#define IBUTTON_COMPLETE_4_REG
#define IBUTTON_COMPLETE_5_REG
#define IBUTTON_COMPLETE_6_REG
#define IBUTTON_COMPLETE_7_REG
#define CE_303_ERROR_REG
#define CE_303_CURRENT_A
#define CE_303_CURRENT_B
#define CE_303_CURRENT_C
#define CE_303_CURRENT_MIL_A
#define CE_303_CURRENT_MIL_B
#define CE_303_CURRENT_MIL_C
#define CE_303_VOLT_A
#define CE_303_VOLT_B
#define CE_303_VOLT_C
#define CE_303_VOLT_MIL_A
#define CE_303_VOLT_MIL_B
#define CE_303_VOLT_MIL_C
#define CE_303_POWER_A
#define CE_303_POWER_B
#define CE_303_POWER_C
#define CE_303_POWER_MIL_A
#define CE_303_POWER_MIL_B
#define CE_303_POWER_MIL_C
#define CE_303_TOTAL_POWER_H
#define CE_303_TOTAL_POWER_L
#define CE_303_TOTAL_POWER_MIL

// -------------------------------------------------------


void ThreadM95Task(void const * argument);



#endif
