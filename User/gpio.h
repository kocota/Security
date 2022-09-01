#ifndef GPIO
#define GPIO

#include "stm32f4xx_hal.h"

#define BUZ_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_SET)  // Включение зуммера Z1
#define BUZ_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, GPIO_PIN_RESET)// Выключение зуммера Z1

#define LED_OUT_ON()  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET)  // Включение светодиода на pin2 разъема ХР25
#define LED_OUT_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET)// Выключение светодиода на pin2 разъема ХР25

#define LED_OUT_TOGGLE() HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12)// Инверсия светодиода на pin2 разъема ХР25

#define LED_VD3_ON()  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET)  // Включение светодиода VD3
#define LED_VD3_OFF() HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET)// Выключение светодиода VD3
#define LED_VD4_ON()  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET)   // Включение светодиода VD4
#define LED_VD4_OFF() HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET) // Выключение светодиода VD4
#define LED_VD5_ON()  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_SET)  // Включение светодиода VD5
#define LED_VD5_OFF() HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6, GPIO_PIN_RESET)// Выключение светодиода VD5

#define LED_VD3_TOGGLE() HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13)// Инверсия светодиода LED8
#define LED_VD4_TOGGLE() HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_8)// Инверсия светодиода LED8
#define LED_VD5_TOGGLE() HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_6)// Инверсия светодиода LED8

#define LED1_OFF()  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_SET) // Включение светодиода LED1
#define LED1_ON() HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2, GPIO_PIN_RESET) // Выключение светодиода LED1
#define LED2_OFF()  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_SET) // Включение светодиода LED2
#define LED2_ON() HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3, GPIO_PIN_RESET) // Выключение светодиода LED2
#define LED3_OFF()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET) // Включение светодиода LED3
#define LED3_ON() HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET) // Выключение светодиода LED3
#define LED4_OFF()  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_SET) // Включение светодиода LED4
#define LED4_ON() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4, GPIO_PIN_RESET) // Выключение светодиода LED4
#define LED5_OFF()  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_SET) // Включение светодиода LED5
#define LED5_ON() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5, GPIO_PIN_RESET) // Выключение светодиода LED5
#define LED6_OFF()  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET) // Включение светодиода LED6
#define LED6_ON() HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET) // Выключение светодиода LED6
#define LED7_OFF()  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_10, GPIO_PIN_SET)// Включение светодиода LED7
#define LED7_ON() HAL_GPIO_WritePin(GPIOI, GPIO_PIN_10, GPIO_PIN_RESET)// Выключение светодиода LED7
#define LED8_OFF()  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_11, GPIO_PIN_SET)// Включение светодиода LED8
#define LED8_ON() HAL_GPIO_WritePin(GPIOI, GPIO_PIN_11, GPIO_PIN_RESET)// Выключение светодиода LED8

#define LED1_TOGGLE() HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_2) // Инверсия светодиода LED1
#define LED2_TOGGLE() HAL_GPIO_TogglePin(GPIOH, GPIO_PIN_3) // Инверсия светодиода LED2
#define LED3_TOGGLE() HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0) // Инверсия светодиода LED3
#define LED4_TOGGLE() HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_4) // Инверсия светодиода LED4
#define LED5_TOGGLE() HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_5) // Инверсия светодиода LED5
#define LED6_TOGGLE() HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_3) // Инверсия светодиода LED6
#define LED7_TOGGLE() HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_10)// Инверсия светодиода LED7
#define LED8_TOGGLE() HAL_GPIO_TogglePin(GPIOI, GPIO_PIN_11)// Инверсия светодиода LED8

void m95_power_on (void);
void m95_power_off (void);



#endif
