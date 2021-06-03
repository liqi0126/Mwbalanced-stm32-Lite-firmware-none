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
int deg90 = 2000;

int pitching = 0;

#define AngleToMotorPulse(x)   (x)
#define DistanceToMotorPulse(x) (x)

void Turn(int angle) {
	g_iCarSpeedSet = 0;
	turning = 1;
	g_iCarMotorPulseDiffCumSet = AngleToMotorPulse(angle);
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
 
	
	if (turning 
		&& g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 30
		&& g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 30) {
		turning = 0;
	} else {
		if (direction == 0) { 
			if (Distance >= 30) { 								// ��ǰ���ϰ���, ֱ��
				g_iCarSpeedSet = 50;
			} else {															// ����ת��
				if (!rightVisited) {								// ������ת
					rightVisited = 1;
					Turn(90);
				} else {
					Turn(-90);
				}
			}
		} else if (direction == 1) { // ����̽��
			if (pitching) {
				pitching = 0;
				if (Distance >= 30) { // ��鵽�˿�,ǰ��
					direction = 0;
				} else {							// ����ص��ұ߼���ǰ��
					Turn(90);
				}
			} else if (g_s32MotorPulseSumCum >= DistanceToMotorPulse(40)) { // ÿǰ��40cm, �ص�������һ��
				pitching = 1;
				Turn(0);
			} else {
				if (Distance >= 30) {		// ����ǰ��
					g_iCarSpeedSet = 50;
				} else {								// û�ҵ�����, ת�����
					Turn(-90);
					direction = -1;
				}
			}
		} else {
			if (pitching) {
				pitching = 0;
				if (Distance >= 30) {
					direction = 0;
				} else {
					Turn(-90);
				}
			} else if (g_s32MotorPulseSumCum >= DistanceToMotorPulse(40)) { // ÿǰ��40cm, �ص�������һ��
				// TODO: ����Ҫ�������ĵط�
				pitching = 1;
				Turn(0);
			} else {
				if (Distance >= 30) {
					g_iCarSpeedSet = 50;
				} else {
					// TODO: ?????
				}
			}
		}
	}
	
	while (1)
	{
		
		SecTask();			//�뼶����
		
		if(SoftTimer[1] == 0)
		{// ÿ��20ms ִ��һ��
			SoftTimer[1] = 20;
			ResponseIMU();			
			DebugService();			
			Parse(Uart3Buffer);
		}			
  	
		if(SoftTimer[2] == 0)
		{
			SoftTimer[2] = 20;
			ShowHomePage();
	
			Read_Distane();

			if(g_CarRunningMode == ULTRA_FOLLOW_MODE){
				if(IsUltraOK())UltraControl(0);	//����������ģʽ
	 		}
			if(g_CarRunningMode == ULTRA_AVOID_MODE){
				if(IsUltraOK())UltraControl(1);	//����������ģʽ
	 		}
			else if(g_CarRunningMode == INFRARED_TRACE_MODE){
				TailingControl();
			}
		}
	}
}


/******************* (C) COPYRIGHT 2016 MiaowLabs Team *****END OF FILE************/


