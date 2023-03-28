#ifndef __CODE_H
#define __CODE_H

#include "main.h"
#include "lcd.h"
#include "i2c_hal.h"
#include "freeRTOS.h"
#include "task.h"
#define key1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define key2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define key3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define key4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)


enum LCD_DISPLAY
{
	SHOP,PRICE,REP
};
typedef enum LCD_DISPLAY lcd_display_t;

struct PARAMETER 
{
		uint8_t num[2];			//0:x,1:y
		double pri[2];
		uint8_t rep[2];
	uint8_t storage;
};
typedef struct PARAMETER para_t;

struct TIMER
{
	uint8_t timer_on;
	uint16_t timer_count;
};
typedef struct TIMER timer;

enum E2PROM
{
	pri_on,rep_on,off
};
typedef enum E2PROM ee;

void lcd_set(void);
void lcd_main(void);
void key_main(void);
void pwm_set(void);
void pwm_main(void);
void ee_take(void);
void ee_write(void);
void led_main(void);
void tim_5s(void);
void uart_buy(void);


#endif
