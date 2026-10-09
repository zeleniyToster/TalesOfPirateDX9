#include "StdAfx.h"
#include "Weather.h"
#include "Submap.h"


// weather forecast timer; records the time of the last forecast for each kind of weather
DWORD g_dwLastWeatherTick[20] = { 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0 };

// produces a ground state at random coordinates within the given large range
void CWeather::RandLocation(SubMap *pMap)
{
	DWORD dwCurTick = GetTickCount();

	if( (dwCurTick - _dwLastLocationTick) < (_dwFrequence * 1000)) return;

	_dwLastLocationTick = dwCurTick;
	
	// picks a random coordinate within the range and produces a ground state
		
	int sx = _sx + rand()%_w;
	int sy = _sy + rand()%_h;
	int nw = 1 +  rand()%3; // size of the ground range produced each time, in units of 2 metres
	int nh = 1 +  rand()%3;

	Rect	SRange = {{sx * 2 * 100, sy * 2 * 100}, {(sx + nw) * 2 * 100, (sy + nh) * 2 * 100}};
	short	sStateParam[defSKILL_STATE_PARAM_NUM];	// state number, level, duration (milliseconds)

    sStateParam[0] = _btType;
    sStateParam[1] = 1;
    sStateParam[2] = (short)_dwStateLastTime;

	pMap->RangeAddState(&SRange, 0, g_pCSystemCha->GetID(), g_pCSystemCha->GetHandle(), enumSKILL_TYPE_ENEMY, enumSKILL_TAR_LORS, enumSKILL_EFF_HELPFUL, sStateParam);

	CSkillStateRecord *pEff = GetCSkillStateRecordInfo(_btType);
	if(pEff)
	{
		if( (dwCurTick - g_dwLastWeatherTick[_btType]) > 1000 * g_Config.m_lWeather) // one weather forecast every 2 minutes, for each kind of weather
		{
			g_dwLastWeatherTick[_btType] = dwCurTick; 
			char szText[128]; 
			//sprintf(szText, "天气:本海域%d %d附近将发生%s", SRange.ltop.x / 100,  SRange.ltop.y / 100, pEff->szDataName); 
			
			// Modify by lark.li 20080618 begin
			CFormatParameter param(3);
			char val[25];
			sprintf(val,"%d", SRange.ltop.x / 100);
			param.setString(0, val);
			sprintf(val,"%d", SRange.ltop.y / 100);
			param.setString(1, val);
			param.setString(2, pEff->szDataName);

			//char szParamMsg[255];
			RES_FORMAT_STRING(GM_WEATHER_CPP_00001, param, szText);
			//sprintf(szText, RES_STRING(GM_WEATHER_CPP_00001), SRange.ltop.x / 100,  SRange.ltop.y / 100, pEff->szDataName); 
			// End

			g_pGameApp->LocalNotice( szText );
			//LG("weather", "[%s]预报[%s], time = %d\n", pMap->GetName(), szText, dwCurTick / 1000);
			LG("weather", "[%s]predict[%s], time = %d\n", pMap->GetName(), szText, dwCurTick / 1000);
		}
	}
	
	// LG("weather", "产生天气状态%d, [%d %d] w = %d h = %d\n", _btType, sx * 2, sy * 2, nw, nh);
}


CWeatherMgr::~CWeatherMgr()
{
	for(std::list<CWeather*>::iterator it = _WeatherList.begin(); it!=_WeatherList.end(); it++)
	{
		CWeather *pWeather = (*it);
		delete pWeather;
	}

	_WeatherList.clear();
}

void CWeatherMgr::Run(SubMap *pMap)
{
	for(std::list<CWeather*>::iterator it = _WeatherList.begin(); it!=_WeatherList.end(); it++)
	{
		CWeather *pWeather = (*it);
		pWeather->RandLocation(pMap);
	}
}

