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

int pitching = 0;

#define AngleToMotorPulse(x)   ((int)(45 * x / 2))
#define DistanceToMotorPulse(x) ((int)(512 * x / 7))

void Turn(int angle) {
	g_iCarSpeedSet = 0;
	turning = 1;
	g_iCarMotorPulseDiffCumSet = AngleToMotorPulse(angle);
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
			Read_Distane();
			// ShowHomePage();

			if (turning) {
				if (g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 30 && g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 30) {
						turning = 0;
						g_s32MotorPulseSumCum = 0;
				}
			} else {
				if (direction == 0) {
					if (Distance > 0 && Distance <= 30) {  // 否则转弯
						if (!rightVisited) {								// 先向右转
							rightVisited = 1;
							Turn(90);
							direction = 1;
						} else {
							Turn(-90);
							direction = -1;
						}						
					} else {
						g_iCarSpeedSet = 50;   // 向前无障碍物, 直行
					}
				} else if (direction == 1) { // 向右探索
					if (pitching) {
						pitching = 0;
						if (Distance > 0 && Distance < 30) { // 否则回到右边继续前进
							Turn(90);
						} else { // 检查到了空,前进
							direction = 0;
							rightVisited = 0;
						}
					} else if (g_s32MotorPulseSumCum >= DistanceToMotorPulse(40)) { // 每前进40cm, 回到正面检查一次
						pitching = 1;
						Turn(0);
					} else {
						if (Distance > 0 && Distance < 30) { // 没找到出口, 转向左边
							Turn(-90);
							direction = -1;
						}
						else {		// 向右前进
							g_iCarSpeedSet = 50;
						}
					}
				} else {
					if (pitching) {
						pitching = 0;
						if (Distance > 0 && Distance < 30) {
							Turn(-90);
						} else {
							direction = 0;
							rightVisited = 0;
						}
					} else if (g_s32MotorPulseSumCum >= DistanceToMotorPulse(40)) { // 每前进40cm, 回到正面检查一次
						// TODO: 不需要检查检查过的地方
						pitching = 1;
						Turn(0);
					} else {
						if (Distance > 0 && Distance < 30) {
						}
						else {		// 向左前进
							g_iCarSpeedSet = 50;
						}
					}
				}
			}

		}
	}
}


/******************* (C) COPYRIGHT 2016 MiaowLabs Team *****END OF FILE************/


