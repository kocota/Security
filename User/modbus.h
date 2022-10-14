#ifndef MODBUS
#define MODBUS

unsigned int CRC16( unsigned char * pucFrame, unsigned int usLen );


//---- Описание регистров статуса -----------------------------

#define VERSION_REG                      0x1000 // (4096) версия прошивки контроллера
#define SECURITY_STATUS_REG              0x1001 // (4097) статус режима охраны (0 - резерв ;1- включена из центра; 2 - отключена из центра; 3 - включена таблеткой 4 - отключена таблеткой; 5 - тревога открытия двери; 6 - процесс постановки на охрану; 7 - не удаётся поставить на охрану; )
#define STATUS_LOOP_REG                  0x1002 // (4098) состояние шлейфов сигнализации (номер бита соответствует номеру шлейфа (6 шлейфов) 1 - замкнут)
#define ERROR_LOOP_REG                   0x1003 // (4099) неисправные шлейфы (номер бита соответствует номеру шлейфа (6 шлейфов))
#define ALARM_LOOP_REG                   0x1004 // (4100) сработавшие шлейфы
#define TIME_CURRENT_YEAR_REG            0x1005 // (4101) текущий год
#define TIME_CURRENT_MONTH_REG           0x1006 // (4102) текущий месяц
#define TIME_CURRENT_DAY_REG             0x1007 // (4103) текущий день
#define TIME_CURRENT_HOUR_REG			 0x1008 // (4104) текущий час
#define TIME_CURRENT_MINUTE_REG          0x1009 // (4105) текущая минута
#define TIME_CURRENT_SECOND_REG          0x100A // (4106) текущая секунда
#define TIME_CURRENT_WEEKDAY_REG         0x100B // (4107) текущий день недели
#define ADDRESS_PROCESSED_EVENT_H_REG    0x100C // (4108) адрес последнего прочитанного события
#define ADDRESS_PROCESSED_EVENT_L_REG    0x100D // (4109) адрес последнего прочитанного события
#define ADDRESS_LAST_EVENT_H_REG         0x100E // (4110) адрес последнего события
#define ADDRESS_LAST_EVENT_L_REG         0x100F // (4111) адрес последнего события
#define SYSTEM_STATUS_REG                0x1010 // (4112) наименование последнего события системы
#define POWER_ON_REG                     0x1011 // (4113) наличие основного питания
#define ERROR_RTC_REG                    0x1012 // (4114) статус работы кварца на 32кГц

#define IBUTTON_COMPLETE_0_REG           0x1015 // (4117) нулевой байт идентифицированной таблетки
#define IBUTTON_COMPLETE_1_REG           0x1016 // (4118) первый байт идентифицированной таблетки
#define IBUTTON_COMPLETE_2_REG           0x1017 // (4119) второй байт идентифицированной таблетки
#define IBUTTON_COMPLETE_3_REG           0x1018 // (4120) третий байт идентифицированной таблетки
#define IBUTTON_COMPLETE_4_REG           0x1019 // (4121) четвертый байт идентифицированной таблетки
#define IBUTTON_COMPLETE_5_REG           0x101A // (4122) пятый байт идентифицированной таблетки
#define IBUTTON_COMPLETE_6_REG           0x101B // (4123) шестой байт идентифицированной таблетки
#define IBUTTON_COMPLETE_7_REG           0x101C // (4124) седьмой байт идентифицированной таблетки

#define CE_303_ERROR_REG                 0x101E // (4126) ошибка опроса счётчика 0 - нет ошибок 1 - таймаут 2 - ошибка данный  10 - при смене номера счётчика
#define CE_303_CURRENT_A_REG             0x101F // (4127) ток фазы А
#define CE_303_CURRENT_B_REG             0x1020 // (4128) ток фазы В
#define CE_303_CURRENT_C_REG             0x1021 // (4129) ток фазы С
#define CE_303_CURRENT_MIL_A_REG         0x1022 // (4130) милиамперы фаза А
#define CE_303_CURRENT_MIL_B_REG         0x1023 // (4131) милиамперы фаза В
#define CE_303_CURRENT_MIL_C_REG         0x1024 // (4132) милиамперы фаза С
#define CE_303_VOLT_A_REG                0x1025 // (4133) напряжение фаза А
#define CE_303_VOLT_B_REG                0x1026 // (4134) напряжение фаза В
#define CE_303_VOLT_C_REG                0x1027 // (4135) напряжение фаза С
#define CE_303_VOLT_MIL_A_REG            0x1028 // (4136) миливольты фаза А
#define CE_303_VOLT_MIL_B_REG            0x1029 // (4137) миливольты фаза В
#define CE_303_VOLT_MIL_C_REG            0x102A // (4138) миливольты фаза С
#define CE_303_POWER_A_REG               0x102B // (4139) мощность фаза А
#define CE_303_POWER_B_REG               0x102C // (4140) мощность фаза В
#define CE_303_POWER_C_REG               0x102D // (4141) мощность фаза С
#define CE_303_POWER_MIL_A_REG           0x102E // (4142) мощность миливатт фаза А
#define CE_303_POWER_MIL_B_REG           0x102F // (4143) мощность миливатт фаза В
#define CE_303_POWER_MIL_C_REG           0x1030 // (4144) мощность миливатт фаза С

#define CE_303_TOTAL_POWER_H_REG         0x1031 // (4145) суммарная мощность, старший байт
#define CE_303_TOTAL_POWER_L_REG         0x1032 // (4146) суммарная мощность, младший байт
#define CE_303_TOTAL_POWER_MIL_REG       0x1033 // (4147) суммарная мощность миливатт

#define SIGNAL_LEVEL                     0x1034 // (4148) уровень сигнала
#define ICCID_NUMBER_REG1                0x1035 // (4149) ICCID номер 1
#define ICCID_NUMBER_REG2                0x1036 // (4150) ICCID номер 2
#define ICCID_NUMBER_REG3                0x1037 // (4151) ICCID номер 3
#define ICCID_NUMBER_REG4                0x1038 // (4152) ICCID номер 4
#define ICCID_NUMBER_REG5                0x1039 // (4153) ICCID номер 5
#define ICCID_NUMBER_REG6                0x103A // (4154) ICCID номер 6
#define ICCID_NUMBER_REG7                0x103B // (4155) ICCID номер 7
#define ICCID_NUMBER_REG8                0x103C // (4156) ICCID номер 8

// -------------------------------------------------------


//---- Описание регистров управления ---------------------

#define SECURITY_CONTROL_REG             0x1090 // (4240) включение охранной функции ( 0 - отключить из центра; 1- включить из центра; 2 -отключить с таблетки; 3 - включить с таблетки )
#define CONTROL_LOOP_REG                 0x1091 // (4241) перечень контролируемых шлейфов ( 1- контролировать; 0 - не контролировать (номер бита соответствует номеру шлейфа) )
#define FILTER_TIME_LOOP_REG             0x1092 // (4242) фильтр ложных срабатываний ( время срабатывания (шаг 0,01 секунды) )
#define QUANTITY_FALSE_LOOP_REG          0x1093 // (4243) количество ложных срабатываений ( количество срабатываний с временем меньше фильтра до подачи сигнала неисправность шлейфа )
#define TIME_FALSE_LOOP_REG              0x1094 // (4244) время обнуления ложных срабатываний ( время в минутах )
#define ALARM_LOOP_CLEAR_REG             0x1095 // (4245) сбросить сработавшие шлейфы ( 1 - сброс, возвращается в 0 автоматически )
#define FALSE_LOOP_CLEAR_REG             0x1096 // (4246) сброс неисправностей шлейфов
#define SECURITY_TIME_MAX_REG            0x1097 // (4247) время постановки на охрану в секундах
#define TIME_UPDATE_REG                  0x1098	// (4248) переменная установки нового времени, сбрасывается автоматически
#define TIME_YEAR_REG                    0x1099 // (4249) переменная года который необходимо установить
#define TIME_MONTH_REG                   0x109A // (4250) переменная месяца который необходимо установить
#define TIME_DAY_REG                     0x109B // (4251) переменная дня который необходимо установить
#define TIME_HOUR_REG                    0x109C // (4252) переменная часа который необходимо установить
#define TIME_MINUTE_REG                  0x109D // (4253) переменная минуты которую необходимо установить
#define TIME_SECONDS_REG                 0x109E // (4254) переменная секунд которую надо установить
#define TIME_WEEKDAY_REG                 0x109F // (4255) переменная дня недели который необходимо установить
#define MODBUS_IDLE_TIME_MAX_REG         0x10A0 // (4256) максимальное время сеанса связибез передачи данных в минутах
#define TIME_CONNECTION_TEST_REG         0x10A1 // (4257) время между тестовыми звонками на свой номер
#define EVENT_READ_REG                   0x10A2 // (4258) инициализирует чтение из памяти
#define EVENT_ADDRESS_HIGH_REG           0x10A3 // (4259) старший байт адреса чтения памяти
#define EVENT_ADDRESS_LOW_REG            0x10A4 // (4260) младший байт адреса чтения памяти
#define MODEM_RING_TRY_LOAD_REG          0x10A5 // (4261) количество попыток дозвона в центр до паузы
#define MODEM_RING_PAUSE_LOAD_REG        0x10A6 // (4262) время паузы после неудачных попыток дозвона в минутах
#define MODEM_RING_PAUSE2_LOAD_REG       0x10A7 // (4263) время между звонками в центр в секундах
#define RING_MINUTE_TIME_REG             0x10A8 // (4264) время с последнего звонка до перезагрузки модема в минутах
#define RING_HOUR_TIME_REG               0x10A9 // (4265) время с последнего звонка до перезагрузки модема в часах

#define ID_HIGH_REG                      0x10AB // (4267) ID устройства, старший байт
#define ID_LOW_REG                       0x10AC // (4268) ID устройства, младший байт
#define LAMP_CONTROL_REG                 0x10AD // (4269) регистр управления лампочной
#define METER_POLLING_REG                0x10AE // (4270) включение опроса счётчика ( 1 - счётчик опрашивается 0 - нет )
#define IP_1_REG                         0x10AF // (4271) IP сервера
#define IP_2_REG                         0x10B0 // (4272) IP сервера
#define IP_3_REG                         0x10B1 // (4273) IP сервера
#define IP_4_REG                         0x10B2 // (4274) IP сервера
#define PORT_HIGH_REG                    0x10B3 // (4275) номер порта сервера, старший байт
#define PORT_LOW_REG                     0x10B4 // (4276) номер порта сервера, младший байт
#define METER_ID_HIGH_REG                0x10B5 // (4277) номер прибора учета, старший байт
#define METER_ID_LOW_REG                 0x10B6 // (4278) номер прибора учета, младший байт
#define GPRS_CALL_REG                    0x10B7 // (4279) флаг что устройство по GPRS звонит в центр ( 1 - звонит, сбросить в 0 )

//--------------------------------------------------------

//----структура переменной статусных регистров---------------------
typedef struct
{
	uint16_t version_reg;
	uint16_t security_status_reg;
	uint16_t status_loop_reg;
	uint16_t error_loop_reg;
	uint16_t alarm_loop_reg;
	uint16_t time_current_year_reg;
	uint16_t time_current_month_reg;
	uint16_t time_current_day_reg;
	uint16_t time_current_hour_reg;
	uint16_t time_current_minute_reg;
	uint16_t time_current_second_reg;
	uint16_t time_current_weekday_reg;
	uint16_t address_processed_event_h_reg;
	uint16_t address_processed_event_l_reg;
	uint16_t system_status_reg;
	uint16_t power_on_reg;
	uint16_t error_rtc_reg;
	uint16_t reserved_1;
	uint16_t reserved_2;
	uint16_t ibutton_complite_0_reg;
	uint16_t ibutton_complite_1_reg;
	uint16_t ibutton_complite_2_reg;
	uint16_t ibutton_complite_3_reg;
	uint16_t ibutton_complite_4_reg;
	uint16_t ibutton_complite_5_reg;
	uint16_t ibutton_complite_6_reg;
	uint16_t ibutton_complite_7_reg;
	uint16_t reserved_3;
	uint16_t ce303_error_reg;
	uint16_t ce303_current_a_reg;
	uint16_t ce303_current_b_reg;
	uint16_t ce303_current_c_reg;
	uint16_t ce303_current_mil_a_reg;
	uint16_t ce303_current_mil_b_reg;
	uint16_t ce303_current_mil_c_reg;
	uint16_t ce303_volt_a_reg;
	uint16_t ce303_volt_b_reg;
	uint16_t ce303_volt_c_reg;
	uint16_t ce303_volt_mil_a_reg;
	uint16_t ce303_volt_mil_b_reg;
	uint16_t ce303_volt_mil_c_reg;

	uint16_t ce303_total_power_h_reg;
	uint16_t ce303_total_power_l_reg;
	uint16_t ce303_total_power_mil_reg;

	uint16_t signal_level_reg;
	uint16_t iccid_number_reg1;
	uint16_t iccid_number_reg2;
	uint16_t iccid_number_reg3;
	uint16_t iccid_number_reg4;
	uint16_t iccid_number_reg5;
	uint16_t iccid_number_reg6;
	uint16_t iccid_number_reg7;
	uint16_t iccid_number_reg8;

} status_register_struct;
//------------------------------------------------------------------

//----структура переменной управляющих регистров--------------------
typedef struct
{
	uint16_t security_control_reg;
	uint16_t control_loop_reg;
	uint16_t filter_time_loop_reg;
	uint16_t quantity_false_loop_reg;
	uint16_t time_false_loop_reg;
	uint16_t alarm_loop_clear_reg;
	uint16_t false_loop_clear_reg;
	uint16_t security_time_max_reg;
	uint16_t time_update_reg;
	uint16_t time_year_reg;
	uint16_t time_month_reg;
	uint16_t time_day_reg;
	uint16_t time_hour_reg;
	uint16_t time_minute_reg;
	uint16_t time_seconds;
	uint16_t time_weekday_reg;
	uint16_t modbus_idle_time_max_reg;
	uint16_t time_connection_test_reg;
	uint16_t event_read_reg;
	uint16_t event_address_high_reg;
	uint16_t event_address_low_reg;
	uint16_t modem_ring_try_load_reg;
	uint16_t modem_ring_pause_load_reg;
	uint16_t modem_ring_pause2_load_reg;
	uint16_t ring_minute_time_reg;
	uint16_t ring_hour_time_reg;

	uint16_t id_high_reg;
	uint16_t id_low_reg;
	uint16_t lamp_control_reg;
	uint16_t meter_polling_reg;
	uint16_t ip1_reg;
	uint16_t ip2_reg;
	uint16_t ip3_reg;
	uint16_t ip4_reg;
	uint16_t port_high_reg;
	uint16_t port_low_reg;
	uint16_t meter_id_high_reg;
	uint16_t meter_id_low_reg;
	uint16_t gprs_call_reg;

} control_register_struct;
//------------------------------------------------------------------


#endif
