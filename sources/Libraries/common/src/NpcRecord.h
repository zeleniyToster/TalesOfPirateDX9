// NpcRecord.h Created by knight-gongjian 2004.11.24.
//---------------------------------------------------------
#pragma once

#ifndef _NPCRECORD_H_
#define _NPCRECORD_H_

#include <tchar.h>
#include "util.h"
#include "TableData.h"

//---------------------------------------------------------

#define NPC_MAX_NAMESIZE			128

class CNpcRecord : public CRawDataInfo
{
public:
	char szNpc[NPC_MAX_NAMESIZE];		// npc file name
	char szName[NPC_MAX_NAMESIZE];		// npc name shown on the map
	USHORT ulCharID;	// index into the character list
	USHORT x, y;		// npc position
	USHORT w, h;
	USHORT usParam1, usParam2;
};

class CNpcRecordSet : public CRawDataSet
{
public:
	CNpcRecordSet(int nIDStart, int nIDCnt, int nCol = 128)
	: CRawDataSet(nIDStart, nIDCnt, nCol)
	{
		_Init();
	}

protected:

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return new CNpcRecord[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (CNpcRecord*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(CNpcRecord);
	}

	virtual void*	_CreateNewRawData(CRawDataInfo *pInfo)
	{
		return NULL;
	}

	virtual void  _DeleteRawData(CRawDataInfo *pInfo)
	{
		SAFE_DELETE(pInfo->pData);
	}

	virtual BOOL _ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList);

};
//---------------------------------------------------------

#endif // _NPCRECORD_H_