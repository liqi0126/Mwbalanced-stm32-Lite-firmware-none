#include "trace.h"
#include "common.h"
#include "control.h"
#include "infrare.h"

int 	g_iTraceEnd = 1;
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
	if(cnt >= 15)
	{
		g_iTraceEnd = 1;
		EnableDirectionControl();
		return;
	}	
	
	if(g_iRa && g_iRc && g_iLa && g_iLc)
	{
		g_iCarSpeedSet = -1;
		g_fBluetoothDirection = 0;
		cnt ++;
	}
	/*
	else if(Rc && Ra)
	{
		g_iCarSpeedSet = 5;
		g_fBluetoothDirection = -480;
	}
	else if(Lc && La)
	{
		g_iCarSpeedSet = 5;
		g_fBluetoothDirection = 480;
	}
	*/

	else if(g_iLa)
	{
		g_fBluetoothDirection = 120;
		g_iCarSpeedSet = 13;
		cnt -= 1;
	}
	else if(g_iRa)
	{
		g_fBluetoothDirection = -120;
		g_iCarSpeedSet = 13;
		cnt -= 1;
	}
	else if(g_iLc)
	{
		g_iCarSpeedSet = 8;
		g_fBluetoothDirection = 400;
		cnt -= 1;
	}
	else if(g_iRc)
	{
		g_iCarSpeedSet = 8;
		g_fBluetoothDirection = -400;
		cnt -= 1;
	}

	else
	{
		g_fBluetoothDirection = 0;
		g_iCarSpeedSet = 18;
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
