//=============================================================================
// FileName: EventRecord.cpp
// Creater: ZhangXuedong
// Date: 2004.11.24
// Comment:
//=============================================================================

#include "EventRecord.h"
using namespace std;

CEventRecordSet * CEventRecordSet::_Instance = NULL;

BOOL CEventRecordSet::_ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList)
{   
	if(ParamList.size()==0) return FALSE;

	CEventRecord *pInfo = (CEventRecord*)pRawDataInfo;

	int m = 0, n = 0;
    string strList[80];
	string strLine;

	// id
	pInfo->lID = pInfo->nID;
	// name
	_tcsncpy(pInfo->szName, pInfo->szDataName, defEVENT_NAME_LEN);
	pInfo->szName[defEVENT_NAME_LEN - 1] = _TEXT('0');

    // event type
    pInfo->sEventType = Str2Int(ParamList[m++]);
	// trigger type
	pInfo->sArouseType = Str2Int(ParamList[m++]);
	// trigger radius
	pInfo->sArouseRadius = Str2Int(ParamList[m++]);
	// effect played on trigger
	pInfo->sEffect = Str2Int(ParamList[m++]);
	// sound played on trigger
	pInfo->sMusic = Str2Int(ParamList[m++]);
	// effect played on spawn
	pInfo->sBornEffect = Str2Int(ParamList[m++]);	
	// cursor shown for the event
	pInfo->sCursor = Str2Int(ParamList[m++]);		
	// when the mouse cursor is valid (0 both, 1 character, 2 ship)
	pInfo->chMainChaType = Str2Int(ParamList[m++]);		
	return TRUE;
}
