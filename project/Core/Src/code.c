#include "main.h"


int s_num[2]={0,0};
float s_pri[2]={1.0,1.0};
int s_rep[2]={10,10};
int s_rep_tmp[2]={10,10};

int lcd_dis=shop;

int count_pwm=0;
int pwm_en=0;

int led1_en=0;
int count_led=0;

int tim_en=0;
int led2_en=0;
int count_led2=0;

char rec[1];

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart1;
//led part
void led_main()
{
	static int led_fir=1;
	if(led_fir)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
		led_fir=0;
	}
	if(s_rep[0]==0&&s_rep[1]==0)
	{
		tim_en=1;
	}
	else
	{
		tim_en=0;
	}
	
	if(led1_en&&tim_en)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, GPIO_PIN_RESET);
		if(led2_en)
		{
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
			led2_en=0;
		}
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(10);
	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
	}
	else if(led1_en&&!tim_en)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(10);
	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
	}
	else if(!led1_en&&tim_en)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
		if(led2_en)
		{
			HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_9);
			led2_en=0;
		}
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(10);
	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_All,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_SET);
		HAL_Delay(10);
	  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2, GPIO_PIN_RESET);
	}
}
//EE part
uint8_t x_rd(uint8_t addr)
{
	unsigned char val;
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	
	val=I2CReceiveByte();
	I2CWaitAck();
	I2CStop();
	return val;
}


void x_wr(unsigned char info,unsigned char addr)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	
	I2CSendByte(addr);
	I2CWaitAck();
	I2CSendByte(info);
	I2CWaitAck();
	I2CStop();
}

void EE_wr_rep()
{
	x_wr(s_rep[0],0);
	HAL_Delay(10);
	x_wr(s_rep[1],1);
}

void EE_wr_pri()
{
	int s_pri_ten[2];
	int i;
	for(i=0;i<2;i++)
	{
		s_pri_ten[i]=s_pri[i]*10;
	}
	x_wr(s_pri_ten[0],2);
	HAL_Delay(10);
	x_wr(s_pri_ten[1],3);
}

void EE_rd()
{
	static int fir=1;
	float s_pri_tmp[2];
	int i;
	if(fir)
	{
	  s_rep[0]=x_rd(0);
		s_rep[1]=x_rd(1);
		s_pri_tmp[0]=x_rd(2);
		s_pri_tmp[1]=x_rd(3);
		for(i=0;i<2;i++)
		{
			s_pri[i]=s_pri_tmp[i]/10;
		}
		fir=0;
	}
}

void EE_init()
{
	x_wr(10,0);
	HAL_Delay(10);
	x_wr(10,1);
	HAL_Delay(10);
	x_wr(10,2);
	HAL_Delay(10);
	x_wr(10,3);
}
	

//lcd_part
void lcd_fir()
{
	LCD_DisplayStringLine(Line0,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line2,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line5,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line6,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line7,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line8,(uint8_t*)"                    ");
	LCD_DisplayStringLine(Line9,(uint8_t*)"                    ");
}

void lcd_shop()
{
	char x_mes[21];
	char y_mes[21];
	sprintf(x_mes,"     X:%d            ",s_num[0]);
	sprintf(y_mes,"     Y:%d            ",s_num[1]);
	LCD_DisplayStringLine(Line1,(uint8_t*)"        SHOP        ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_mes);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_mes);
}

void lcd_price()
{
	char x_mes[21];
	char y_mes[21];
	sprintf(x_mes,"     X:%.1f           ",s_pri[0]);
	sprintf(y_mes,"     Y:%.1f           ",s_pri[1]);
	LCD_DisplayStringLine(Line1,(uint8_t*)"        PRICE       ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_mes);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_mes);
}

void lcd_rep()
{
	char x_mes[21];
	char y_mes[21];
	sprintf(x_mes,"     X:%d           ",s_rep[0]);
	sprintf(y_mes,"     Y:%d           ",s_rep[1]);
	LCD_DisplayStringLine(Line1,(uint8_t*)"        REP         ");
	LCD_DisplayStringLine(Line3,(uint8_t*)x_mes);
	LCD_DisplayStringLine(Line4,(uint8_t*)y_mes);
}

void lcd_menu()
{
	static int lcd_first=1;
	if(lcd_first)
	{
		lcd_fir();
		lcd_first=0;
	}
	if(lcd_dis==shop)
	{
		lcd_shop();
	}
	else if(lcd_dis==price)
	{
		lcd_price();
	}
	else if(lcd_dis==rep)
	{
		lcd_rep();
	}
}


//pwm_part
void pwm_main()
{
	if(pwm_en==1)
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,150);
	}
	else
	{
		__HAL_TIM_SetCompare(&htim2,TIM_CHANNEL_2,25);
	}
}

//uart_part
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	char loop[20];
	sprintf(loop,"X:%.1f,Y:%.1f",s_pri[0],s_pri[1]);
	if(!strcmp(rec,"?"))
	{
		HAL_UART_Transmit(&huart1,(unsigned char*)loop,strlen(loop),1000);
	}
	HAL_UART_Receive_IT(&huart1,(unsigned char*)rec,1);
}

//key part
int key_scan()
{
	static int key_flag=1;
	if((key1==0||key2==0||key3==0||key4==0)&&key_flag)
	{
		HAL_Delay(10);
		key_flag=0;
		if(key1==0) return 1;
		else if(key2==0) return 2;
		else if(key3==0) return 3;
		else if(key4==0) return 4;
	}
	else if(key1==1&&key2==1&&key3==1&&key4==1) key_flag=1;
	return 0;
}

void key1_f()
{
	int j;
	lcd_dis++;
	if(lcd_dis>2)
	{
		lcd_dis=shop;
	}
	
	if(lcd_dis==shop)
	{
		for(j=0;j<2;j++)
		{
			s_rep_tmp[j]=s_rep[j];
		}
	}
	
}

void key_add(int typ)
{
	if(lcd_dis==shop)
	{
		if(s_rep_tmp[typ]!=0)
		{
			s_num[typ]++;
			s_rep_tmp[typ]--;
		}
	}
	else if(lcd_dis==price)
	{
		if(s_pri[typ]>2.0)
		{
			s_pri[typ]=1.0;
		}
		else
		{
			s_pri[typ]+=0.1;
		}
		EE_wr_pri();
	}
	else if(lcd_dis==rep)
	{
		s_rep[typ]++;
		EE_wr_rep();
	}
}

void key_cfom()
{
	int i,j;
	char loop[20];
	float cost;
	cost=(float)s_num[0]*s_pri[0]+(float)s_num[1]*s_pri[1];
	sprintf(loop,"X:%d,Y:%d,Z:%.1f",s_num[0],s_num[1],cost);
	for(i=0;i<2;i++)
	{
		s_num[i]=0;
	}
	for(j=0;j<2;j++)
	{
		s_rep[j]=s_rep_tmp[j];
	}
	pwm_en=1;
	led1_en=1;
	HAL_UART_Transmit(&huart1,(unsigned char*)loop,strlen(loop),1000);
}

void key_menu()
{
	int key_num=0;
	key_num=key_scan();
	switch(key_num)
	{
		case 1: key1_f();break;
		case 2: key_add(0);break;
		case 3: key_add(1);break;
		case 4: if(lcd_dis==shop)key_cfom();break;
	}
}
