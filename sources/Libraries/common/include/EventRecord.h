//=============================================================================
// FileName: EventRecord.h
// Creater: ZhangXuedong
// Date: 2004.11.24
// Comment:
//=============================================================================

#ifndef EVENTRECORD_H
#define EVENTRECORD_H

#include <tchar.h>
#include "util.h"
#include "TableData.h"
#include "point.h"

// event trigger type
enum EEventTouchType
{
	enumEVENTT_RANGE	= 1,		// triggered by radius
};

enum EEventExecType
{
	enumEVENTE_SMAP_ENTRY	= 1,	// static map switch point
	enumEVENTE_DMAP_ENTRY	= 2,	// dynamic map switch point
};

enum EEventType
{
	enumEVENT_TYPE_ACTION = 1,	// 
	enumEVENT_TYPE_ENTITY,
};

// trigger type; decides how the client fires it
enum EEventArouseType
{
	enumEVENT_AROUSE_DISTANCE,		// triggered by radius
	enumEVENT_AROUSE_CLICK,			// triggered by click
};

const char cchEventRecordKeyValue = (char)0xff;

#define defEVENT_NAME_LEN	18

class CEventRecord : public CRawDataInfo
{
public:
	//CEventRecord();

	long	lID;						// id
	char	szName[defEVENT_NAME_LEN];	// name
    short   sEventType;                 // event type; selects how the protocol message is sent, see EEventType
	short	sArouseType;				// trigger type
	short	sArouseRadius;				// trigger radius
	short	sEffect;					// effect played on trigger
	short	sMusic;						// sound played on trigger
	short   sBornEffect;				// effect played on spawn
	short	sCursor;					// cursor shown for the event
	char	chMainChaType;				// what the event click applies to (0 = both, 1 = character, 2 = ship)

	bool	IsValid( int MainChaType )	{ return chMainChaType==0 || MainChaType==chMainChaType;	}
};

class CEventRecordSet : public CRawDataSet
{
public:

	static CEventRecordSet* I() { return _Instance; }

	CEventRecordSet(int nIDStart, int nIDCnt, int nCol = 128)
		:CRawDataSet(nIDStart, nIDCnt, nCol)
	{
		_Instance = this;
		_Init();
	}

protected:

	static CEventRecordSet* _Instance; // acts like a singleton: keeps a reference to itself

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return new CEventRecord[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (CEventRecord*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(CEventRecord);
	}

	virtual void*	_CreateNewRawData(CRawDataInfo *pInfo)
	{
		return NULL;
	}

	virtual void  _DeleteRawData(CRawDataInfo *pInfo)
	{
		SAFE_DELETE(pInfo->pData);
	}

	virtual BOOL _ReadRawDataInfo(CRawDataInfo *pRawDataInfo, std::vector<std::string> &ParamList);
};

inline CEventRecord* GetEventRecordInfo( int nTypeID )
{
	return (CEventRecord*)CEventRecordSet::I()->GetRawDataInfo(nTypeID);
}

#endif // EVENTRECORD_H