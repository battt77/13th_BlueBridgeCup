#ifndef __CODE_H
#define __CODE_H


#define key1 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)
#define key2 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)
#define key3 HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)
#define key4 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)

#define shop 0
#define price 1
#define rep 2

void EE_init(void);
void EE_rd(void);
void lcd_menu(void);
void key_menu(void);
void pwm_main(void);
void led_main(void);
#endif
