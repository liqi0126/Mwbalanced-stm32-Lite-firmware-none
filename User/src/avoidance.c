#include "avoidance.h"
#include "common.h"
#include "control.h"
#include "ultrasonic.h"
#include "trace.h"

// direction: 
// 0 直行
// -1 左转
// 1 右转
int direction = 0;
int turning = 0;
int turningMotorDiff = 0;
int finished = 0;
int rightVisited = 0;

int obstacleDistance = 25;
int obstacleTriggle = 8;
int obstacleCnt = 0;


int stopCnt = 0;
int stopTimer = 0;

void Turn(int angle) {
	g_iCarSpeedSet = 32;
	turning = 1;
	g_iCarMotorPulseDiffCumSet = AngleToMotorPulse(angle);
	if (g_iCarMotorPulseDiffCumSet < g_s32MotorPulseDiffCum) {
		g_fBluetoothDirection = -10;
	} else {
		g_fBluetoothDirection = 10;
	}
}

void Avoidance(void) {	
	if (finished) {
		if(stopTimer++ > 100) {
			g_iCarSpeedSet = 0;
			g_tCarAnglePID.P = 0;
			g_tCarAnglePID.I = 0;
			g_tCarAnglePID.D = 0;
			g_tCarSpeedPID.P = 0;
			g_tCarSpeedPID.I = 0;
			g_tCarAnglePID.D = 0;
			g_tPulseDiffPID.P = 0;
			g_tPulseDiffPID.I = 0;
			g_tPulseDiffPID.D = 0;
		}
		else {
			g_iCarSpeedSet = 70;
		}
		return;
	}
	if (turning) {
		if (g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 200 && g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 200) {
			turning = 0;
			g_fBluetoothDirection = 0;
			g_s32MotorPulseSumCum = 0;
		}
	} else {
		if(stopCnt >= 300 / g_iTimer)
		{
			finished = 1;
			g_iCarSpeedSet = 10;
			EnableDirectionControl();
			return;
		}
		
		if(stopCnt > 0) {
			stopCnt-=2;
		}
			
		getInfraraResult();
		if(g_iLa && g_iLc && g_iRa && g_iRc && g_s32MotorPulseSumTotal >= 5000) { // 红外检测停止
			g_iCarSpeedSet = 5;
			stopCnt+=3;
			return;
		}

		if (direction == 0) {
			if (obstacleCnt > obstacleTriggle) {
				obstacleCnt = 0;
				if (g_iCarMotorPulseLeftDirectionCum >= 0) {	// 先向右转
					Turn(90);
					direction = 1;
				} else {
					Turn(-90);
					direction = -1;
				}			
			}
			if (Distance > 0 && Distance < obstacleDistance && g_fCarAngle > -10.) {  // 否则转弯
				obstacleCnt++;
			} else {
				MoveForward(Distance);
			}
		} else if (direction == 1) { // 向右探索
			if (obstacleCnt > obstacleTriggle) {
				obstacleCnt = 0;
				Turn(0);
				direction = 0;				
			} else if (Distance > 0 && Distance < obstacleDistance && g_fCarAngle > -10.) { // 转回正面
				obstacleCnt++;
			} else {		// 向右前进
				MoveForward(Distance);
			}
		} else {
			if (obstacleCnt > obstacleTriggle) {
				obstacleCnt = 0;
				Turn(0);
				direction = 0;
			} else if (Distance > 0 && Distance < obstacleDistance && g_fCarAngle > -10.) { // 转回正面
				obstacleCnt++;
			} else {		// 向左前进
				MoveForward(Distance);
			}
		}
	}
}
