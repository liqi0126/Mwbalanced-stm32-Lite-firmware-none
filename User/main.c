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


int stage = 0;

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

		// �Զ������4������
		if (stage == 0) { // ǰ��1m
			// TODO: �Ӹ���ť�жϿ�ʼ
			if (g_s32LeftMotorPulseCum + g_s32RightMotorPulseCum > 20000) {
				g_iCarSpeedSet = 0;
				SoftTimer[3] = 1500;
				stage = 1;
			} else {
				g_iCarSpeedSet = 50;
			}
		} else if (stage == 1) { // ��ͣ1-3s
			if (SoftTimer[3] == 0) {
				stage = 2;
			}
		} else if (stage == 2) { // ����1m
			if (g_s32LeftMotorPulseCum + g_s32RightMotorPulseCum <= 0) {
				g_iCarSpeedSet = 0;
				SoftTimer[3] = 1500;
				stage = 3;
			} else {
				g_iCarSpeedSet = -50;
			}
		} else if (stage == 3) { // ��ͣ1-3s
			if (SoftTimer[3] == 0) {
				stage = 4;
			}
		} else if (stage == 4) { // ��ת90-180��
			if (g_s32RightMotorPulseCum - g_s32LeftMotorPulseCum >= 2500) {
				g_iCarSpeedSet = 0;
				g_fBluetoothDirection = 0;
				SoftTimer[3] = 1500;
				DirectionControlEnable();
				stage = 5;
			} else {
				g_iCarSpeedSet = 50;
				g_fBluetoothDirection = -150;
				DirectionControlDisable();
			}
		} else if (stage == 5) { // ��ͣ1-3s
			if (SoftTimer[3] == 0) {
				stage = 6;
			}
		} else if (stage == 6) { // ��ת90-180��
			if (g_s32RightMotorPulseCum - g_s32LeftMotorPulseCum <= -2500) {
				g_iCarSpeedSet = 0;
				g_fBluetoothDirection = 0;
				SoftTimer[3] = 1500;
				DirectionControlEnable();
				stage = 7;
			} else {
				g_iCarSpeedSet = 50;
				g_fBluetoothDirection = 150;
				DirectionControlDisable();
			}
		} else {								 // ֹͣ
			if (SoftTimer[3] == 0) {
				g_iCarSpeedSet = 0;
				g_fBluetoothDirection = 0;
			}
		}
		
		
		
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


