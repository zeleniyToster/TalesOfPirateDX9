//=============================================================================
// FileName: SwitchMapRecord.cpp
// Creater: ZhangXuedong
// Date: 2004.11.23
// Comment:
//=============================================================================

#include "SwitchMapRecord.h"

using namespace std;

CSwitchMapRecordSet * CSwitchMapRecordSet::_Instance = NULL;

BOOL CSwitchMapRecordSet::_ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList)
{   
	if(ParamList.size()==0) return FALSE;

	CSwitchMapRecord *pInfo = (CSwitchMapRecord*)pRawDataInfo;

	int m = 0, n = 0;
    string strList[80];
	string strLine;

	// id
	pInfo->lID = pInfo->nID;
	// id of the bound entity
	pInfo->lEntityID = Str2Int(pInfo->szDataName);
	// event id
	pInfo->lEventID = Str2Int(ParamList[m++]);
	// entity position
	memset(&pInfo->SEntityPos, 0, sizeof(Point));
	strLine = ParamList[m++];
	n = Util_ResolveTextLine(strLine.c_str(), strList, 80, ',');
	pInfo->SEntityPos.x = Str2Int(strList[0]);
	pInfo->SEntityPos.y = Str2Int(strList[1]);
	// entity direction
	pInfo->sAngle = Str2Int(ParamList[m++]);
	// target map name
	_tcsncpy(pInfo->szTarMapName, ParamList[m++].c_str(), defMAP_NAME_LEN);
	pInfo->szTarMapName[defMAP_NAME_LEN - 1] = _TEXT('\0');
	// position on the target map
	memset(&pInfo->STarPos, 0, sizeof(Point));
	strLine = ParamList[m++];
	n = Util_ResolveTextLine(strLine.c_str(), strList, 80, ',');
	pInfo->STarPos.x = Str2Int(strList[0]);
	pInfo->STarPos.y = Str2Int(strList[1]);

	return TRUE;
}
