// NpcRecord.cpp Created by knight-gongjian 2004.11.24.
//---------------------------------------------------------

#include <NpcRecord.h>
using namespace std;

//---------------------------------------------------------

BOOL CNpcRecordSet::_ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList)
{   
	if(ParamList.size()==0) return FALSE;

	CNpcRecord *pInfo = (CNpcRecord*)pRawDataInfo;

	int m = 0, n = 0;
	string strList[8];
	string strLine;

	// npc display name
	_tcsncpy(pInfo->szName, pInfo->szDataName, NPC_MAXSIZE_NAME);
	pInfo->szName[NPC_MAXSIZE_NAME - 1] = _TEXT('\0');

	// npc type information
	pInfo->sNpcType = Str2Int(ParamList[m++]);
	
	// npc character information
	pInfo->sCharID = Str2Int(ParamList[m++]);

	// colour type shown by the client
	pInfo->byShowType = (BYTE)Str2Int(ParamList[m++]);

	// npc position
	Util_ResolveTextLine( ParamList[m++].c_str(), strList, 8, ',' ); 
	pInfo->dwxPos0 = Str2Int(strList[0]);
	pInfo->dwyPos0 = Str2Int(strList[1]);

	Util_ResolveTextLine( ParamList[m++].c_str(), strList, 8, ',' ); 
	pInfo->dwxPos1 = Str2Int(strList[0]);
	pInfo->dwyPos1 = Str2Int(strList[1]);
	
	pInfo->sDir = Str2Int(ParamList[m++]);
	pInfo->sParam1 = Str2Int(ParamList[m++]);
	pInfo->sParam2 = Str2Int(ParamList[m++]);

	// npc file name
	_tcsncpy(pInfo->szNpc, ParamList[m++].c_str(), NPC_MAXSIZE_NAME);
	pInfo->szNpc[NPC_MAXSIZE_NAME - 1] = _TEXT('\0');

	// npc dialogue handler
	_tcsncpy(pInfo->szMsgProc, ParamList[m++].c_str(), NPC_MAXSIZE_MSGPROC);
	pInfo->szMsgProc[NPC_MAXSIZE_MSGPROC - 1] = _TEXT('\0');
	
	// npc dialogue handler
	_tcsncpy(pInfo->szMisProc, ParamList[m++].c_str(), NPC_MAXSIZE_MSGPROC);
	pInfo->szMisProc[NPC_MAXSIZE_MSGPROC - 1] = _TEXT('\0');

	return TRUE;
}
