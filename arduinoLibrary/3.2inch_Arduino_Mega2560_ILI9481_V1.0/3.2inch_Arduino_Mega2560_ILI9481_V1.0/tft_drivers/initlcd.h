void LCD_GPIO_Init(void)
{
	......
//Init the GPIOs for LCD	
}
void LCD_RESET(void)
{
	LCD_RST_CLR;
	delay_ms(100);	
	LCD_RST_SET;
	delay_ms(50);
}
void LCD_Init(void)
{
	//LCD_GPIO_Init();//Init the GPIOs for LCD
	//LCD_RESET();//You should ResetLCD before Init.

	LCD_Write_COM(0x11);
	delay_ms(20);
	LCD_Write_COM(0xD0);
	LCD_Write_DATA(0x07);
	LCD_Write_DATA(0x42);
	LCD_Write_DATA(0x18);
	
	LCD_Write_COM(0xD1);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x07);//07
	LCD_Write_DATA(0x10);
	
	LCD_Write_COM(0xD2);
	LCD_Write_DATA(0x01);
	LCD_Write_DATA(0x02);
	
	LCD_Write_COM(0xC0);
	LCD_Write_DATA(0x10);
	LCD_Write_DATA(0x3B);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x02);
	LCD_Write_DATA(0x11);
	
	LCD_Write_COM(0xC5);
	LCD_Write_DATA(0x03);
	
	LCD_Write_COM(0xC8);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x32);
	LCD_Write_DATA(0x36);
	LCD_Write_DATA(0x45);
	LCD_Write_DATA(0x06);
	LCD_Write_DATA(0x16);
	LCD_Write_DATA(0x37);
	LCD_Write_DATA(0x75);
	LCD_Write_DATA(0x77);
	LCD_Write_DATA(0x54);
	LCD_Write_DATA(0x0C);
	LCD_Write_DATA(0x00);
	
	LCD_Write_COM(0x36);
	LCD_Write_DATA(0x0A);
	
	LCD_Write_COM(0x3A);
	LCD_Write_DATA(0x55);
	
	LCD_Write_COM(0x2A);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x01);
	LCD_Write_DATA(0x3F);
	
	LCD_Write_COM(0x2B);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x00);
	LCD_Write_DATA(0x01);
	LCD_Write_DATA(0xE0);
	delay_ms(120);
	LCD_Write_COM(0x29);
	LCD_Write_COM(0x002c); 
}

