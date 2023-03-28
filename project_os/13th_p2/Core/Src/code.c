#include "code.h"

lcd_display_t lcd_dt=SHOP;
para_t				parat={{0,0},{1.0,1.0},{10,10},0};
timer 				timer_t={0,0};
ee 						ee_t=off; 
 
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;

//led part
void led_buy()
{
	if(timer_t.timer_on)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, GPIO_PIN_SET);
	}
}

uint8_t led_flag=0;
void led_empty()
{
	if(parat.rep[0]==0&&parat.rep[1]==0)
	{
		if(led_flag)
		{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
			led_flag=0;
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
			led_flag=1;
		}
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
	}
}

void led_main()
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
	led_buy();
	led_empty();
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
}


//IIC part
void IIC_wr(uint8_t address,uint8_t info)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(address);
	I2CWaitAck();
	I2CSendByte(info);
	I2CWaitAck();
	I2CStop();
}

uint8_t IIC_rd(uint8_t address)
{
	uint8_t val;
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(address);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	val=I2CReceiveByte();
	I2CWaitAck();
	I2CStop();
	
	return val;
}

uint8_t send_num=0;
void ee_write()
{
	para_t*point;
	point=&parat;
	if(ee_t==pri_on)
	{
		IIC_wr(send_num,parat.pri[send_num]*10);
		send_num++;
		if(send_num==2)
		{
			send_num=0;
			ee_t=off;
		}
	}
	else if(ee_t==rep_on)
	{
		IIC_wr(send_num+2,parat.rep[send_num]);
		send_num++;
		if(send_num==2)
		{
			send_num=0;
			ee_t=off;
		}
	}
}

void ee_take()
{
	int i=0;
	para_t* point;
	point=&parat;
	for(i=0;i<2;i++)
	{
		point->pri[i]=(double)IIC_rd(i)/10;
		HAL_Delay(10);
		point->rep[i]=IIC_rd(i+2);
		HAL_Delay(10);
	}
}



//pwm part
void pwm_set()
{
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
}

void pwm_main()
{
	if(timer_t.timer_on)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,150-1);	
	}
	else
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,25-1);
	}
	
}
//usart part
void uart_buy()
{
	char mes[25];
	double total;
	total=parat.num[0]*parat.pri[0]+parat.num[1]*parat.pri[1];
	sprintf(mes,"X:%d,Y:%d,Z:%3.1f",parat.num[0],parat.num[1],total);
	HAL_UART_Transmit(&huart1,(uint8_t*)mes,strlen(mes),1000);
}

//key part
uint8_t key_flag=1;
uint8_t key_scan()
{
	if((!key1||!key2||!key3||!key4)&&key_flag)
	{
		key_flag=0;
		vTaskDelay(10);
		if(!key1)return 1;
		else if(!key2) return 2;
		else if (!key3) return 3;
		else if(!key4) return 4;
	}
	else if(key1&&key2&&key3&&key4) key_flag=1;
	return 0;
}

void key_switch()
{
	switch(lcd_dt)
	{
		case SHOP:
			lcd_dt=PRICE;
		break;
		case PRICE:
			lcd_dt=REP;
		break;
		case REP:
			lcd_dt=SHOP;
		break;
		default:
			break;
	}
}

void key_add(uint8_t custom)
{
	para_t *point;
	point=&parat;
	switch(lcd_dt)
	{
		case SHOP:
			point->num[custom]++;
			if(point->num[custom]>point->rep[custom])
			{
				point->num[custom]=0;
			}
		break;
		case PRICE:
			point->pri[custom]+=0.1;
		  if(point->pri[custom]>2.1)
			{
				point->pri[custom]=1;
			}
		ee_t=pri_on;
		break;
		case REP:
			point->rep[custom]+=1;
			ee_t=rep_on;
		break;
		default:break;
	}
	
}


void key_buy()
{
	para_t *point;
	timer *point1;
	char mes[20];
	double total;
	point1=&timer_t;
	point=&parat;
	total=parat.num[0]*parat.pri[0]+parat.num[1]*parat.pri[1];
	sprintf(mes,"X:%d,Y:%d,Z:%3.1f",parat.num[0],parat.num[1],total);
	HAL_UART_Transmit(&huart1,(uint8_t*)mes,strlen(mes),1000);
	point->rep[0]-=point->num[0];
	point->rep[1]-=point->num[1];
	point->num[0]=0;
	point->num[1]=0;
	point1->timer_on=1;

}

void key_main()
{
	uint8_t key_val;
	key_val=key_scan();
	switch(key_val)
	{
		case 1:
			key_switch();
		break;
		case 2:
			key_add(0);
		break;
		case 3:
			key_add(1);
		break;
		case 4:
			key_buy();
		break;
		default:
			break;
	}
}

//lcd part
void lcd_set()
{
	LCD_Init();
	LCD_SetTextColor(White);
	LCD_SetBackColor(Black);
	LCD_DisplayStringLine(Line0,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line2,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line5,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line6,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line7,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line8,(uint8_t*)"                    ");
}

void lcd_shop()
{
	uint8_t x_num[25],y_num[25];
	sprintf((char*)x_num,"     X:%d            ",parat.num[0]);
	sprintf((char*)y_num,"     Y:%d            ",parat.num[1]);
 	LCD_DisplayStringLine(Line1,(uint8_t*)"        SHOP        ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_num);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_num);
}

void lcd_pri()
{
	uint8_t x_pri[25],y_pri[25];
	sprintf((char*)x_pri,"     X:%3.1f          ",parat.pri[0]);
	sprintf((char*)y_pri,"     Y:%3.1f          ",parat.pri[1]);
 	LCD_DisplayStringLine(Line1,(uint8_t*)"        PRICE       ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_pri);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_pri);
}

void lcd_rep()
{
	uint8_t x_rep[25],y_rep[25];
	sprintf((char*)x_rep,"     X:%2d            ",parat.rep[0]);
	sprintf((char*)y_rep,"     Y:%2d            ",parat.rep[1]);
 	LCD_DisplayStringLine(Line1,(uint8_t*)"        REP        ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_rep);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_rep);
}


void lcd_main()
{
	char rtct[25];
	RTC_TimeTypeDef rtc_time;
	RTC_DateTypeDef rtc_date;
	switch(lcd_dt)
	{
		case SHOP:
			lcd_shop();
		break;
		case PRICE:
			lcd_pri();
		break;
		case REP:
			lcd_rep();
		break;
		default:
			break;	
	}
}
