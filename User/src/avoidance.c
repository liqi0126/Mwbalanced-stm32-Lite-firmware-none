#include "avoidance.h"
#include "common.h"
#include "control.h"
#include "ultrasonic.h"
#include "trace.h"

// direction: 
// 0 ֱ��
// -1 ��ת
// 1 ��ת
int direction = 0;
int turning = 0;
int turningMotorDiff = 0;
int finished = 0;
int rightVisited = 0;

int pitched = 0;
int pitching = 0;
int pitchingLeftChecking = 0;
int pitchingRightChecking = 0;

int obstacleDistance = 18;
int pitchSideDistance = 10;
int pitchDistance = 20;

int stopCnt = 0;
int stopTimer = 0;

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

void Avoidance(void) {

	
	if (finished) {
		if(stopTimer++ > 1500)
			g_iCarSpeedSet = 0;
		else
			g_iCarSpeedSet = 20;
		return;
	}
	if (turning) {
		if (g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 200 && g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 200) {
			turning = 0;
			g_fBluetoothDirection = 0;
			g_s32MotorPulseSumCum = 0;
		}
	} else {
		
		if(stopCnt >= 400)
		{
			finished = 1;
			g_iCarSpeedSet = 10;
			EnableDirectionControl();
			return;
		}
		
		if(stopCnt > 0)
			stopCnt-=2;
		
		getInfraraResult();
		if(g_iLa && g_iLc && g_iRa && g_iRc && g_s32MotorPulseSumTotal >= 5000) { // ������ֹͣ
			g_iCarSpeedSet = 5;
			stopCnt+=3;
			return;
		}

		if (direction == 0) {
			if (Distance > 0 && Distance < obstacleDistance) {  // ����ת��
				if (!rightVisited) {	// ������ת
					rightVisited = 1;
					Turn(90);
					direction = 1;
				} else {
					Turn(-90);
					direction = -1;
				}						
			} else {
				MoveForward(Distance);
			}
		} else if (direction == 1) { // ����̽��
			if (pitching) {
				if (Distance > 0 && Distance < obstacleDistance) {
					Turn(90);
					pitching = 0;
				} else {
					Turn(0);
					direction = 0;
					rightVisited = 0;
					pitching = 0;
				}
			} else if (g_s32MotorPulseSumCum >= 2 * DistanceToMotorPulse(pitchDistance)) { // ÿǰ��50cm, �ص�������һ��
				pitching = 1;
				Turn(0);
			} else {
				if (Distance > 0 && Distance < obstacleDistance) { // û�ҵ����ڣ�ת������
					Turn(0);
					direction = 0;
					pitching = 0;
				}
				else {		// ����ǰ��
					MoveForward(Distance);
				}
			}
		} else {
			if (pitching) {
				if (Distance > 0 && Distance < obstacleDistance) {
					Turn(-90);
					pitching = 0;
				} else {
					Turn(0);
					direction = 0;
					rightVisited = 0;
					pitching = 0;
				}
			} else if (g_s32MotorPulseSumCum >= 2 * DistanceToMotorPulse(pitchDistance)) { // ÿǰ��40cm, �ص�������һ��
				pitching = 1;
				Turn(0);
			} else {
				if (Distance > 0 && Distance < obstacleDistance) {
					Turn(0);
					direction = 0;
					rightVisited = 0;
					pitching = 0;
				} else {		// ����ǰ��
					MoveForward(Distance);
				}
			}
		}
	}
}
