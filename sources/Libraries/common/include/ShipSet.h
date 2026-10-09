//
#pragma once

#include <tchar.h>
#include "util.h"
#include "TableData.h"

// button value for the hull change command
enum ID_BTN_BOAT
{
    ID_BTN_HEADLEFT =      0,
    ID_BTN_HEADRIGHT,
    ID_BTN_POWERLEFT,
    ID_BTN_POWERRIGHT,
    ID_BTN_WEAPONLEFT,
    ID_BTN_WEAPONRIGHT,

    ID_BTN_ITEM_NUM,
};
#define BOAT_MAXSIZE_MINNAME	2   // minimum length of a ship name
#define BOAT_MAXSIZE_BOATNAME	17	// maximum length of a ship name
#define BOAT_MAXSIZE_NAME		64	// maximum length of a ship part name
#define BOAT_MAXSIZE_DESP		128 // maximum description size in bytes for a ship part
#define BOAT_MAXSIZE_PART		3   // maximum number of parts on a ship
#define BOAT_MAXNUM_PARTITEM	16  // largest number of options per replaceable ship part
#define BOAT_MAXNUM_MOTOR		4	// largest number of engines per engine type
#define BOAT_MAXNUM_MODEL		8	// maximum number of models per ship
#define MAX_CHAR_BOAT			3	// maximum number of ships a character may own

// ship selection list data
struct BOAT_BERTH_DATA
{
	BYTE byID[MAX_CHAR_BOAT];
	BYTE byState[MAX_CHAR_BOAT];
	char szName[MAX_CHAR_BOAT][BOAT_MAXSIZE_BOATNAME + BOAT_MAXSIZE_DESP];
};

enum BOAT_STATE
{
	BS_GOOD		= 0,				// in good condition
	BS_NOSP		= 1,				// needs SP replenished
	BS_NOHP		= 2,				// needs repairs
	BS_DEAD		= 3,				// ship has sunk
};

// the character builds and configures an owned ship
struct BOAT_DATA
{
	char szName[BOAT_MAXSIZE_BOATNAME];		// ship name
	USHORT sBoat;				// ship info id
	USHORT sHeader;				// bow
	USHORT sBody;				// hull
	USHORT sEngine;				// engine
	USHORT sCannon;				// cannons
	USHORT sEquipment;			// ship equipment
	USHORT sBerth;				// port the ship is moored at
	USHORT sCapacity;			// cargo hold capacity
	DWORD  dwOwnerID;			// id of the ship's owner
};

// attributes of the ship as loaded by the character
struct BOAT_LOAD_INFO
{
	DWORD  dwID;				// ship database id
	char   szName[BOAT_MAXSIZE_BOATNAME];			// ship name
	USHORT sHeader;				// bow
	USHORT sBody;				// hull
	USHORT sEngine;				// engine
	USHORT sCannon;				// cannons
	USHORT sEquipment;			// ship equipment
	USHORT sBerth;				// port the ship is moored at
};

struct xShipBuildInfo
{
	char szName[BOAT_MAXSIZE_NAME];			// ship name
	char szDesp[BOAT_MAXSIZE_DESP];			// description (item level)
	char szBerth[BOAT_MAXSIZE_NAME];		// name of the port the ship is moored at
	char szHeader[BOAT_MAXSIZE_NAME];		// bow name
	char szBody[BOAT_MAXSIZE_NAME];			// hull name
	char szEngine[BOAT_MAXSIZE_NAME];		// engine name
	char szCannon[BOAT_MAXSIZE_NAME];		// name of the mounted cannons
	char szEquipment[BOAT_MAXSIZE_NAME];	// name of the accessory

	// model id of the ship assembly
	USHORT sPosID;				// model animation info id
	union
	{
		struct
		{
			DWORD dwBody;		// hull
			DWORD dwHeader;		// bow
			DWORD dwEngine;		// ship engine
			DWORD dwFlag;		// ship flag
			DWORD dwMotor[BOAT_MAXNUM_MOTOR];
		};

		DWORD dwBuf[BOAT_MAXNUM_MODEL];
	};

	BYTE  byHeader;	   // selection index record for the bow
	BYTE  byEngine;	   // selection index record for the engine
	BYTE  byCannon;	   // selection index record for the cannons
	BYTE  byEquipment; // selection index record for the accessory
	BYTE  byIsUpdate;  // whether ship parts may be replaced
	DWORD dwMoney;	   // value
    DWORD dwMinAttack; // minimum attack
    DWORD dwMaxAttack; // maximum attack
    DWORD dwCurEndure; // current durability
    DWORD dwMaxEndure; // maximum durability
    DWORD dwSpeed;	   // movement speed
    DWORD dwDistance;  // maximum attack range
    DWORD dwDefence;   // defence
	DWORD dwCurSupply; // current supplies
	DWORD dwMaxSupply; // maximum supplies
	DWORD dwConsume;   // ship supply consumption
    DWORD dwAttackTime;// attack interval
	USHORT	sCapacity; // cargo hold capacity
};

struct xShipAttrInfo
{
	// base attributes of a ship as built
	DWORD dwMoney;	   // value
    DWORD dwMinAttack; // minimum attack
    DWORD dwMaxAttack; // maximum attack
    DWORD dwCurEndure; // current durability
    DWORD dwMaxEndure; // maximum durability
    DWORD dwSpeed;	   // movement speed
    DWORD dwDistance;  // maximum attack range
    DWORD dwDefence;   // defence
	DWORD dwCurSupply; // current supplies
	DWORD dwMaxSupply; // maximum supplies
    DWORD dwAttackTime;// attack interval
	USHORT	sCapacity; // cargo hold capacity

	// other attributes
	DWORD dwResume;			// ship durability regeneration
	DWORD dwResist;	        // ship resistance
	DWORD dwScope;			// blast radius
	DWORD dwConsume;		// ship supply consumption
	DWORD dwCannonSpeed;	// shell speed

	// ship attribute coefficients
};

struct xShipInfo: public CRawDataInfo
{
	char szName[BOAT_MAXSIZE_NAME];	// ship name
	char szDesp[BOAT_MAXSIZE_DESP];	// description (item level)
	USHORT sItemID;					// captain's certificate item id
	USHORT sCharID;					// ship character attribute id
	USHORT sPosID;					// model animation info id
	BYTE byIsUpdate;				// whether the ship's parts may be swapped
	USHORT sNumHeader;
	USHORT sNumEngine;
	USHORT sNumCannon;
	USHORT sNumEquipment;
	USHORT sHeader[BOAT_MAXNUM_PARTITEM];		// bow
	USHORT sEngine[BOAT_MAXNUM_PARTITEM];		// engine
	USHORT sCannon[BOAT_MAXNUM_PARTITEM];		// cannons
	USHORT sEquipment[BOAT_MAXNUM_PARTITEM];	// accessory
	USHORT sBody;			// hull
	USHORT sLvLimit;		// level limit
	USHORT sNumPfLimit;
	USHORT sPfLimit[BOAT_MAXNUM_PARTITEM];		// job limit

	USHORT sEndure;			// ship durability
	USHORT sResume;			// ship durability regeneration
	USHORT sDefence;		// ship defence
	USHORT sResist;			// ship resistance
    USHORT sMinAttack;		// minimum attack
    USHORT sMaxAttack;		// maximum attack
	USHORT sDistance;		// attack range
	USHORT sTime;			// reload time
	USHORT sScope;			// blast radius
	USHORT sCapacity;		// cargo hold capacity
	USHORT sSupply;			// ship supplies
	USHORT sConsume;		// ship supply consumption
	USHORT sCannonSpeed;	// shell speed
	USHORT sSpeed;			// ship movement speed
	USHORT sParam;			// remarks

	xShipInfo()
	{
		memset( szName, 0, BOAT_MAXSIZE_NAME );
		memset( szDesp, 0, BOAT_MAXSIZE_DESP );
		sItemID = 0;
		sCharID = 0;
		sPosID = 0;
		byIsUpdate = 0;
		sNumHeader = 0;
		sNumEngine = 0;
		sNumCannon = 0;
		sNumEquipment = 0;
		memset( sHeader, 0, sizeof(USHORT)*BOAT_MAXNUM_PARTITEM );
		memset( sEngine, 0, sizeof(USHORT)*BOAT_MAXNUM_PARTITEM );
		memset( sCannon, 0, sizeof(USHORT)*BOAT_MAXNUM_PARTITEM );
		memset( sEquipment, 0, sizeof(USHORT)*BOAT_MAXNUM_PARTITEM );
		sBody = 0;
		sLvLimit = 0;		// level limit
		sNumPfLimit = 0;
		memset( sPfLimit, 0, sizeof(USHORT)*BOAT_MAXNUM_PARTITEM );	


		sEndure = 0;
		sResume = 0;
		sDefence = 0;
		sResist = 0;
		sMinAttack = 0;
		sMaxAttack = 0;
		sDistance = 0;
		sTime = 0;
		sScope = 0;
		sCapacity = 0;
		sSupply = 0;			
		sConsume = 0;		
		sCannonSpeed = 0;	
		sSpeed = 0;
		sParam = 0;
	}
};

struct xShipPartInfo: public CRawDataInfo
{
	char szName[BOAT_MAXSIZE_NAME];	// ship part name
	char szDesp[BOAT_MAXSIZE_DESP];	// description (item level)
    DWORD dwModel;			// part model id
	USHORT sMotor[BOAT_MAXNUM_MOTOR];	// ship engine
	DWORD  dwPrice;			// price

	USHORT sEndure;			// ship durability
	USHORT sResume;			// ship durability regeneration
	USHORT sDefence;		// ship defence
	USHORT sResist;			// ship resistance
    USHORT sMinAttack;		// minimum attack
    USHORT sMaxAttack;		// maximum attack
	USHORT sDistance;		// attack range
	USHORT sTime;			// reload time
	USHORT sScope;			// blast radius
	USHORT sCapacity;		// cargo hold capacity
	USHORT sSupply;			// ship supplies
	USHORT sConsume;		// ship supply consumption
	USHORT sCannonSpeed;	// shell speed
	USHORT sSpeed;			// ship movement speed
	USHORT sParam;			// remarks
	
	xShipPartInfo()
	{
		memset( szName, 0, BOAT_MAXSIZE_NAME );
		memset( szDesp, 0, BOAT_MAXSIZE_DESP );
		dwModel = 0;
		memset( sMotor, 0, sizeof(USHORT)*BOAT_MAXNUM_MOTOR );
		dwPrice = 0;
		sEndure = 0;
		sResume = 0;
		sDefence = 0;
		sResist = 0;
		sMinAttack = 0;
		sMaxAttack = 0;
		sDistance = 0;
		sTime = 0;
		sScope = 0;
		sCapacity = 0;
		sSupply = 0;			
		sConsume = 0;		
		sCannonSpeed = 0;	
		sSpeed = 0;
		sParam = 0;
	}
};

// ship construction attribute table
class xShipSet : public CRawDataSet
{
public:
	xShipSet(int nIDStart, int nIDCnt)
		:CRawDataSet(nIDStart, nIDCnt)
	{
		_Instance = this;
		_Init();
	}
	
	static xShipSet* I() { return _Instance; }

protected:

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return (CRawDataInfo*)new xShipInfo[nCnt];
	}
	
	static xShipSet* _Instance; // acts like a singleton: keeps a reference to itself

	virtual void _DeleteRawDataArray()
	{
		delete[] (xShipInfo*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(xShipInfo);
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

// ship part base attribute table
class xShipPartSet : public CRawDataSet
{
public:
	
	static xShipPartSet* I() { return _Instance; }

	
	xShipPartSet(int nIDStart, int nIDCnt)
		:CRawDataSet(nIDStart, nIDCnt)
	{
		_Instance = this;
		_Init();
	}

protected:

	static xShipPartSet* _Instance;
	
	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return (CRawDataInfo*)new xShipPartInfo[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (xShipPartInfo*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(xShipPartInfo);
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

inline xShipInfo* GetShipInfo( int nTypeID )
{
	return (xShipInfo*)xShipSet::I()->GetRawDataInfo(nTypeID);
}

inline xShipPartInfo* GetShipPartInfo( int nTypeID )
{
	return (xShipPartInfo*)xShipPartSet::I()->GetRawDataInfo(nTypeID);
}
