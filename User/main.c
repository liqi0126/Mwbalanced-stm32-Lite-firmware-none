/**********************************************************************
��Ȩ���У�	���ش��¿Ƽ���2017.
��		����	http://www.miaowlabs.com
��		����	https://miaowlabs.taobao.com/
�� �� ��: 	main.c
��    ��:   ����ʵ����
��		��:   3.00
�������:   2017.03.01
��		Ҫ: 	

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

//�뼶����
void SecTask()
{
	if(SoftTimer[0])return;
	else{
		SoftTimer[0] = 1000;
	}
	g_RunTime++; 			// ��¼����ʱ��
	g_BatVolt = GetBatVoltage(); // ��ȡ��ص�ѹ
	
	if(StatusFlag)ResponseStatus();
	
	LEDToggle();
}

// direction: 
// 0 ֱ��
// -1 ��ת
// 1 ��ת
int direction = 0;
int turning = 0;
int turningMotorDiff = 0;
int rightVisited = 0;

int pitched = 0;
int pitching = 0;

#define AngleToMotorPulse(x)   ((int)(45 * x / 2))
#define DistanceToMotorPulse(x) ((int)(512 * x / 7))

void Turn(int angle) {
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
		g_iCarSpeedSet = 85;   // ��ǰ���ϰ���, ֱ��
	} else {
		g_iCarSpeedSet = 70;
	}
}


/*
	���������ţ����⣬���ƹ��ܺ�����stm32f10x_it.cִ���ļ��ĵδ�ʱ���жϷ�������ѭ��ִ�С�
*/
int main(void)
{	
	
	BspInit();				//��ʼ��BSP

	PIDInit(); 				//��ʼ��PID
	
	CarUpstandInit(); 	//��ʼ��ϵͳ����
	
	SysTick_Init();			//��ʼ����ʱ��	
	
	if(IsInfrareOK())
		g_iGravity_Offset = 1; //������⵽���Һ���ģ�飬�����ƫ��ֵ��
	
	ShowHomePageInit();
	
	while (1)
	{
		SecTask();			//�뼶����
		
		/*
		if(SoftTimer[1] == 0)
		{// ÿ��20ms ִ��һ��
			SoftTimer[1] = 20;
			ResponseIMU();			
			 DebugService();			
			 Parse(Uart3Buffer);
		}	
		*/
  	
		if(SoftTimer[2] == 0)
		{
			SoftTimer[2] = 10;

			if (turning) {
				if (g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 30 && g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 30) {
						turning = 0;
				}
			} else {
				
				char result;
				float direct = 0;
				float speed = 0;
				
				result = InfraredDetect();
				int La = result & infrared_channel_La;
				int Lc = result & infrared_channel_Lc;
				int Ra = result & infrared_channel_Ra;
				int Rc = result & infrared_channel_Rc;
				
				if(La && Ra)
				{
					g_iCarSpeedSet = 0;
					g_fBluetoothDirection = 0;
				}
				else if(Lc && La)
				{
					g_iCarSpeedSet = 0;
					g_fBluetoothDirection = 800;
				}
				else if(Rc && Ra)
				{
					g_iCarSpeedSet = 0;
					g_fBluetoothDirection = -800;
				}
				else if(Lc)
				{
					g_iCarSpeedSet = 10;
					g_fBluetoothDirection = 400;
				}
				else if(Rc)
				{
					g_iCarSpeedSet = 10;
					g_fBluetoothDirection = -400;
				}
				else if(La)
				{
					g_fBluetoothDirection = 120;
					g_iCarSpeedSet = 15;
				}
				else if(Ra)
				{
					g_fBluetoothDirection = -120;
					g_iCarSpeedSet = 15;
				}
				else
				{
					g_fBluetoothDirection = 0;
					g_iCarSpeedSet = 20;
				}
				
			}
		}
	}
}


/******************* (C) COPYRIGHT 2016 MiaowLabs Team *****END OF FILE************/


