/**********************************************************************
版权所有：	喵呜创新科技，2017.
官		网：	http://www.miaowlabs.com
淘		宝：	https://miaowlabs.taobao.com/
文 件 名: 	main.c
作    者:   喵呜实验室
版		本:   3.00
完成日期:   2017.03.01
概		要: 	

***********************************************************************/
#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"
#include "control.h"
#include "debug.h"
#include "communicate.h"
#include "dataflash.h"
#include "common.h"
#include "motor.h"
#include "display.h"
#include "bsp.h"
#include "ADC.h"
#include "ultrasonic.h"
#include "infrare.h"
#include "manage.h"

//秒级任务
void SecTask()
{
	if(SoftTimer[0])return;
	else{
		SoftTimer[0] = 1000;
	}
	g_RunTime++; 			// 记录运行时间
	g_BatVolt = GetBatVoltage(); // 读取电池电压
	
	if(StatusFlag)ResponseStatus();
	
	LEDToggle();
}

// direction: 
// 0 直行
// -1 左转
// 1 右转
int direction = 0;
int turning = 0;
int turningMotorDiff = 0;
int rightVisited = 0;

int pitched = 0;
int pitching = 0;

#define AngleToMotorPulse(x)   ((int)(45 * x / 2))
#define DistanceToMotorPulse(x) ((int)(512 * x / 7))

void Turn(int angle) {
	g_iCarSpeedSet = 0;
	turning = 1;
	g_iCarMotorPulseDiffCumSet = AngleToMotorPulse(angle);
	if (g_iCarMotorPulseDiffCumSet < g_s32MotorPulseDiffCum) {
		g_fBluetoothDirection = -50;
	} else {
		g_fBluetoothDirection = 50;
	}
}

void MoveForward(int distance) {
	if (distance >= 100) {
		g_iCarSpeedSet = 85;   // 向前无障碍物, 直行
	} else {
		g_iCarSpeedSet = 70;
	}
}


/*
	主函数入门，另外，控制功能函数在stm32f10x_it.c执行文件的滴答定时器中断服务函数里循环执行。
*/
int main(void)
{	
	
	BspInit();				//初始化BSP

	PIDInit(); 				//初始化PID
	
	CarUpstandInit(); 	//初始化系统参数
	
	SysTick_Init();			//初始化定时器	
	
	if(IsInfrareOK())
		g_iGravity_Offset = 1; //若果检测到悬挂红外模块，则更改偏移值。
	
	ShowHomePageInit();
	
	while (1)
	{
		SecTask();			//秒级任务
		
		/*
		if(SoftTimer[1] == 0)
		{// 每隔20ms 执行一次
			SoftTimer[1] = 20;
			ResponseIMU();			
			 DebugService();			
			 Parse(Uart3Buffer);
		}	
		*/
  	
		if(SoftTimer[2] == 0)
		{
			SoftTimer[2] = 10;

			char result;
			float direct = 0;
			float speed = 0;

			result = InfraredDetect();
			
			if(result & infrared_channel_Lc)
				direct = -10;
			else if(result & infrared_channel_Lb)
				direct = -6;
			else if(result & infrared_channel_La)
				direct = -4;
			else if(result & infrared_channel_Rc)
				direct = 10;
			else if(result & infrared_channel_Rb)
				direct = 6;
			else if(result & infrared_channel_Ra)
				direct = 4;
			else
				direct = 0.0;

			speed = 3;
		}
	}
}


/******************* (C) COPYRIGHT 2016 MiaowLabs Team *****END OF FILE************/


