// weather is essentially the ground state the system produces, such as wind, thunder or fog
// on a client under that ground state it plays a range effect


// the weather system currently has two purposes
// one, recharging the navigator's shell
// two, affecting player movement speed, life and the like, to enrich how the game looks

#pragma once

class SubMap;

class CWeather
{
public:

	CWeather(BYTE btType) // the weather type must be passed in when constructing
	:_dwLastLocationTick(0), _dwFrequence(2)
	{
		_btType = btType;
		_dwStateLastTime = 20; // 20 seconds by default
	}
	
	void	SetRange(int sx, int sy, int w, int h);	// specifies a large coordinate range
	void	RandLocation(SubMap *pMap);				// picks a random location within the given range to appear
	void	SetFrequence(DWORD dwFre);				// sets the appearance frequency, in seconds
	void	SetStateLastTime(DWORD dwTime);			// sets the state duration, in seconds

protected:

	BYTE	_btType;  // weather type: 0 wind, 1 thunder, 2 fog
	int		_sx;	  // records the coordinate range
	int		_sy;
	int		_w;
	int		_h;
	DWORD	_dwLastLocationTick;
	DWORD	_dwFrequence;
	DWORD	_dwStateLastTime;
};

inline void CWeather::SetRange(int sx, int sy, int w, int h)
{
	_sx = sx;
	_sy = sy;
	_w  = w;
	_h  = h;
}

inline void CWeather::SetFrequence(DWORD dwFre)
{
	_dwFrequence = dwFre;
}

inline void CWeather::SetStateLastTime(DWORD dwTime)
{
	_dwStateLastTime = dwTime;
}


// weather management class; it can add, remove and iterate over every weather, and one instance is needed per map object
class CWeatherMgr 
{
public:
	
	~CWeatherMgr();
	void	AddWeatherRange(CWeather *pWeather);
	void	RemoveWeather(CWeather *pWeather);
	void	Run(SubMap *pMap);	// called by the timer; internally it calls RandLocation on each Weather
	void	ClearAll();

protected:

	std::list<CWeather*>  _WeatherList;
};

inline void CWeatherMgr::AddWeatherRange(CWeather *pWeather)
{
	_WeatherList.push_back(pWeather);
}

inline void CWeatherMgr::RemoveWeather(CWeather *pWeather)
{
	_WeatherList.remove(pWeather);
	delete pWeather;
}

inline void CWeatherMgr::ClearAll()
{
	for(std::list<CWeather*>::iterator it = _WeatherList.begin(); it!=_WeatherList.end(); it++)
	{
		CWeather *pCur = (*it);
		delete pCur;
	}

	_WeatherList.clear();
}


