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
int rightVisited = 0;

int pitched = 0;
int pitching = 0;
int pitchingLeftChecking = 0;
int pitchingRightChecking = 0;


int obstacleDistance = 30;
int pitchDistance = 10;

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
	getInfraraResult();
	if (turning) {
		if (g_s32MotorPulseDiffCum <= g_iCarMotorPulseDiffCumSet + 100 && g_s32MotorPulseDiffCum >= g_iCarMotorPulseDiffCumSet - 100) {
			turning = 0;
			g_fBluetoothDirection = 0;
			g_s32MotorPulseSumCum = 0;
		}
	} else {
		if (direction == 0) {
			if (Distance > 0 && Distance <= obstacleDistance) {  // 否则转弯
				if (!rightVisited) {								// 先向右转
					rightVisited = 1;
					Turn(90);
					direction = 1;
				} else {
					Turn(-90);
					direction = -1;
				}						
			}else if(g_iLa || g_iLc || g_iRa || g_iRc) // 红外检测停止
			{
				// TODO : 停止
				g_iCarSpeedSet = 0;
				
			} else {
				MoveForward(Distance);
			}
		} else if (direction == 1) { // 向右探索
			if (pitching) {
				if (pitchingRightChecking) {
					pitchingRightChecking = 0;
					if (Distance > 30) {
						pitchingLeftChecking = 1;
						Turn(-45);
					} else {
						Turn(90);
						pitching = 0;
					}
				} else if (pitchingLeftChecking) {
					pitchingLeftChecking = 0;
					if (Distance > 30) {
						Turn(0);
						direction = 0;
						rightVisited = 0;
						pitching = 0;
					} else {
						Turn(90);
						pitching = 0;
					}
				} else if (Distance > 0 && Distance < obstacleDistance) { // 否则回到右边继续前进
					pitching = 0;
					Turn(90);
				} else {
					pitchingRightChecking = 1;
					Turn(45);
				}
			} else if (g_s32MotorPulseSumCum >= 2 * DistanceToMotorPulse(pitchDistance)) { // 每前进50cm, 回到正面检查一次
				pitching = 1;
				Turn(0);
			} else {
				if (Distance > 0 && Distance < 10) { // 没找到出口, 转向左边
					Turn(0);
					direction = 0;
				}
				else {		// 向右前进
					MoveForward(Distance);
				}
			}
		} else {
			if (pitching) {
				if (pitchingRightChecking) {
					pitchingRightChecking = 0;
					if (Distance > 20) {
						pitchingLeftChecking = 1;
						Turn(-45);
					} else {
						Turn(-90);
						pitching = 0;
					}
				} else if (pitchingLeftChecking) {
					pitchingLeftChecking = 0;
					if (Distance > 20) { 
						Turn(0);
						direction = 0;
						rightVisited = 0;
						pitching = 0;
					} else {
						Turn(-90);
						pitching = 0;
					}
				} else if (Distance > 0 && Distance < obstacleDistance) { // 否则回到右边继续前进
					pitching = 0;
					Turn(-90);
				} else {
					pitchingRightChecking = 1;
					Turn(45);
				}
			} else if (g_s32MotorPulseSumCum >= 2 * DistanceToMotorPulse(pitchDistance)) { // 每前进40cm, 回到正面检查一次
				// TODO: 不需要检查检查过的地方
				pitching = 1;
				Turn(0);
			} else {
				if (Distance > 0 && Distance < obstacleDistance) {
					Turn(0);
					direction = 0;
					rightVisited = 0;
				}
				else {		// 向左前进
					MoveForward(Distance);
				}
			}
		}
	}
}
