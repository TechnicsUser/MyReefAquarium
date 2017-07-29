#include "sys.h"
#include "lcd.h"
#include "gui.h"
#include "test.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//测试硬件：单片机STC12LE5A60S2,晶振30M  单片机工作电压3.3V
//QDtech-TFT液晶驱动 for C51
//xiao冯@ShenZhen QDtech co.,LTD
//公司网站:www.qdtech.net
//淘宝网站：http://qdtech.taobao.com
//我司提供技术支持，任何技术问题欢迎随时交流学习
//固话(传真) :+86 0755-23594567 
//手机:15989313508（冯工） 
//邮箱:QDtech2008@gmail.com 
//Skype:QDtech2008
//技术交流QQ群:324828016
//创建日期:2013/5/13
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 深圳市全动电子技术有限公司 2009-2019
//All rights reserved
//********************************************************************************

/**************************************************************************************
//=======================================液晶屏数据线接线==========================================//
//P2组高8位数据口,DB8-DB15依次连接P2^0-P2^7;8位模式下只使用高8位
#define  LCD_DataPortH P2 
//P0组低8位数据口,DB0-DB7依次连接P0^0-P0^7;请确认P0口已经上拉10K电阻,不宜太小，最小4.7K,推荐10K.    
#define  LCD_DataPortL P0     
//=======================================液晶屏控制线接线==========================================//
CS=P1^3;		//片选	
RS=P1^2;  		//数据/命令切换
WR=P1^1;		//写控制
RD=P1^0;		//读控制
RESET=P3^3;	 	//复位 
LCD_BL=P3^2;	//背光控制
//=========================================触摸屏触接线=========================================//
//不使用触摸或者模块本身不带触摸，则可不连接
DCLK	  =    P3^6; //触摸屏SPI总线时钟信号接P3.6  
TCS       =    P3^7; //触摸片选TCS接P3.7
DIN       =    P3^4; //MOSI接P3.4	
DOUT      =    P3^5; //MISO接P3.5																						   
Penirq    =    P4^0; //PEN引脚接P4.0，如单片机无P4组，请自行更改其他可用IO并修改代码定义
**************************************************************************************************/	

//STC单片机IO推挽输出设置定义
//其他普通C51单片机无此配置请屏蔽下面这条语句
sfr P3M1  = 0xB1;	//P3M1.n,P3M0.n 	=00--->Standard,	01--->push-pull
sfr P3M0  = 0xB2;	//					=10--->pure input,	11--->open drain

//主函数
void main(void)
{ 

	//设置STC_12LE5A60S2单片机的P3.2为推挽输出，用于控制背光亮灭
	//其他普通C51单片机无此配置请屏蔽下面这条语句
	P3M1 &= ~(1<<2),P3M0 |=  (1<<2); 

	//液晶屏初始化
	LCD_Init();
	//循环进行各项测试	
	while(1)
	{	
		main_test(); 		//测试主界面
		Test_Color();  		//简单刷屏填充测试
		Test_FillRec();		//GUI矩形绘图测试
		Test_Circle(); 		//GUI画圆测试
		English_Font_test();//英文字体示例测试
		Chinese_Font_test();//中文字体示例测试
		Pic_test();			//图片显示示例测试
		//Touch_Test();		//触摸屏手写测试
	}   
}
