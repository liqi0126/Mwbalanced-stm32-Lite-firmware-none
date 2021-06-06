#include "trace.h"
#include "common.h"
#include "control.h"
#include "infrare.h"

int 	g_iTraceEnd = 0;
char	g_cInfraredResult;
int 	g_iLa;
int 	g_iLc;
int 	g_iRa;
int 	g_iRc;
int		cnt = 0;

void Trace(void) {
	
	getInfraraResult();
	if(cnt < 0)
		cnt = 0;
	if(cnt >= 30 / g_iTimer)
	{
		g_iTraceEnd = 1;
		g_iCarSpeedSet = 20;
		g_fBluetoothDirection = 0;
		g_s32MotorPulseSumTotal = 0;
		g_iCarMotorPulseLeftDirectionCum = 0;
		EnableDirectionControl();
		return;
	}	

	g_iCarSpeedSet = 32;
	if(g_iRa && g_iRc && g_iLa && g_iLc)
	{
		g_fBluetoothDirection = 0;
		cnt ++;
	}
	else if(g_iLc)
	{
		g_fBluetoothDirection = -250;
		cnt -= 1;
	}
	else if(g_iRc)
	{
		g_fBluetoothDirection = 250;
		cnt -= 1;
	}
	else if(g_iLa)
	{
		g_fBluetoothDirection = -130;
		cnt -= 1;
	}
	else if(g_iRa)
	{
		g_fBluetoothDirection = 130;
		cnt -= 1;
	}
	else
	{
		cnt -= 1;
	}
}

void getInfraraResult(void)
{
	g_cInfraredResult = InfraredDetect();
	g_iLa = (g_cInfraredResult & infrared_channel_La) != 0;
	g_iLc = (g_cInfraredResult & infrared_channel_Lc) != 0;
	g_iRa = (g_cInfraredResult & infrared_channel_Ra) != 0;
	g_iRc = (g_cInfraredResult & infrared_channel_Rc) != 0;
}
