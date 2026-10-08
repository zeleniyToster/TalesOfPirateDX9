#include "stdafx.h"
#define MAX_BIRTHPOINT 12

struct SBirthPoint // a single spawn point
{
	char szMapName[16];  // map name
	int  x;				 // map coordinates
	int  y;
};

struct SBirthplace // birthplace, holding several spawn points
{
	SBirthPoint PointList[MAX_BIRTHPOINT];
	int nCount;
	
	SBirthplace():nCount(0) {}
	
	void	Add(const char *pszMapName, int x, int y)
	{
		if(nCount>=MAX_BIRTHPOINT) return; // limited number of spawn points
		
		strcpy(PointList[nCount].szMapName, pszMapName);
		PointList[nCount].x = x;
		PointList[nCount].y = y;
		nCount++;
	}
};


class CBirthMgr // birthplace management
{
public:

	~CBirthMgr();
	void		 AddBirthPoint(const char *pszLocation, const char *pszMapName, int x, int y);
	SBirthPoint* GetRandBirthPoint(const char *pszLocation);
	void		 ClearAll();
	
protected:

	std::map<std::string, SBirthplace*>  _LocIdx;
};

// add a single spawn point
inline void CBirthMgr::AddBirthPoint(const char *pszLocation, const char *pszMapName, int x, int y)
{
	SBirthplace *p = NULL;
	std::map<std::string, SBirthplace*>::iterator it = _LocIdx.find(pszLocation);
	if(it!=_LocIdx.end()) // the birthplace already exists
	{
		p = (*it).second;
	}
	else
	{
		// the newly created birthplace
		p = new SBirthplace;
		_LocIdx[pszLocation] = p;
	}
	
	p->Add(pszMapName, x, y);
}

inline SBirthPoint* CBirthMgr::GetRandBirthPoint(const char *pszLocation)
{
	std::map<std::string, SBirthplace*>::iterator it = _LocIdx.find(pszLocation);
	if(it!=_LocIdx.end())
	{
		SBirthplace *p = (*it).second;
		int nSel = rand()%(p->nCount);
		SBirthPoint *pPoint = &(p->PointList[nSel]);
		//LG("birth", "选中了随机出生点[%s] %d %d\n", pPoint->szMapName, pPoint->x, pPoint->y);
		return pPoint;
	}
	return NULL;
}

extern CBirthMgr g_BirthMgr;

inline SBirthPoint* GetRandBirthPoint(const char *pszChaName, const char *pszLocation)
{
	SBirthPoint* pBirth = g_BirthMgr.GetRandBirthPoint(pszLocation);
	if(pBirth==NULL)
	{
		//LG("birth_error", "无效的出生地[%s], Cha = [%s], 将被强行送往白银城\n", pszLocation, pszChaName);
		LG("birth_error", "invalid birth place[%s], Cha = [%s],will force to silver city\n", pszLocation, pszChaName);
		pBirth = g_BirthMgr.GetRandBirthPoint(RES_STRING(GM_BIRTHPLACE_H_00001));
	}
	return pBirth;
}
