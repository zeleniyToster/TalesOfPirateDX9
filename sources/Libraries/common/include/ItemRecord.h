//=============================================================================
// FileName: ItemRecord.h
// Creater: ZhangXuedong
// Date: 2004.09.01
// Comment: CItemRecordSet class
//=============================================================================

#ifndef	ITEMRECORD_H
#define ITEMRECORD_H

#include <tchar.h>
#include "util.h"
#include "TableData.h"
#include "CompCommand.h"
#include "JobType.h"

const char cchItemRecordKeyValue = (char)0xfe;	// -1 is taken by "no job requirement" in the job requirement field

#define defITEM_NAME_LEN	80
#define defITEM_MODULE_NUM	5
#define defITEM_MODULE_LEN	19
#define defITEM_ICON_NAME_LEN	17
#define defITEM_ATTREFFECT_NAME_LEN	33

#define defITEM_DESCRIPTOR_NAME_LEN      257
#define defITEM_DESCRIPTOR_SET_LEN      33

#define defITEM_BIND_EFFECT_NUM			8

#define defITEM_BODY 4

enum EItemType
{
	enumItemTypeSword		= 1,	// sword
	enumItemTypeGlave		= 2,	// greatsword
	enumItemTypeBow			= 3,	// bow
	enumItemTypeHarquebus	= 4,	// flintlock
	enumItemTypeFalchion	= 5,	// knife
	enumItemTypeMitten		= 6,	// knuckle
	enumItemTypeStylet		= 7,	// dagger
	enumItemTypeMoneybag	= 8,	// money bag
	enumItemTypeCosh		= 9,	// baton
	enumItemTypeSinker		= 10,	// hammer
	enumItemTypeShield		= 11,	// shield
	enumItemTypeArrow		= 12,	// arrow
	enumItemTypeAmmo		= 13,	// ammunition
	enumItemTypeHeadpiece	= 19,	// helmet
	enumItemTypeHair		= 20,	// hair
	enumItemTypeFace		= 21,	// face
	enumItemTypeClothing	= 22,	// clothes
	enumItemTypeGlove		= 23,	// gloves
	enumItemTypeBoot		= 24,	// boots
	enumItemTypeNecklace	= 25,
	enumItemTypeRing		= 26,
	enumItemTypeTattoo		= 27,
	enumItemTypeConch		= 29,	// shell
	enumItemTypeMedicine	= 31,	// medicine
	enumItemTypeOvum		= 32,	// sea egg
	enumItemTypeUnknownConsumable = 33, // TODO: Proper naming
	enumItemTypeScroll		= 36,	// return scroll
	enumItemTypeGeneral		= 41,	// ordinary item
    enumItemTypeMission     = 42,   // quest item
	enumItemTypeBoat		= 43,	// captain's certificate
	enumItemTypeWing		= 44,	// wings; the client uses them to show an effect
	enumItemTypeTrade		= 45,	// trade certificate
	enumItemTypeBravery		= 46,	// hero's certificate
	enumItemTypeHull		= 51,	// hull
	enumItemTypeEmbolon		= 52,	// ram
	enumItemTypeEngine		= 53,	// engine
	enumItemTypeArtillery	= 54,	// cannon
	enumItemTypeAirscrew	= 55,	// propeller
	enumItemTypeBoatSign	= 56,	// ship flag
	enumItemTypePetFodder	= 57,	// pet food
	enumItemTypePetSock		= 58,	// energy snack
	enumItemTypePet			= 59,	// guardian spirit

	enumItemCloak			= 88,
	enumItemMount			= 90,

	// Add by lark.li 20080514 begin
	enumItemTypeNo			= 99,	// lottery ball
	// End
};

enum EItemPickTo
{
	enumITEM_PICKTO_KITBAG,
	enumITEM_PICKTO_CABIN,
};

class CItemRecord : public CRawDataInfo
{
public:
	enum EItemExtendInfo
	{
		enumItemNormalStart		= 0,		// ordinary item
		enumItemNormalEnd		= 4999,
		enumItemFusionEndure   = 23000,      // maximum durability a fusion item may require; modified by ning.yan 20080821 (Da Peng asked for the fusion requirement to change from the source/target slot to a maximum durability)
		enumItemFusionStart		= 5000,		// source slot of the fusion
		enumItemFusionEnd		= 9800,		// target slot of the fusion
		// Modify by lark.li 20080703
		enumItemMax				= 32768,		// maximum number of records in the item table
		// End

	};

	CItemRecord();

	long	lID;							// id
	_TCHAR	szName[defITEM_NAME_LEN];		// name
	char    szICON[defITEM_ICON_NAME_LEN];	// ICON
	_TCHAR	chModule[defITEM_MODULE_NUM][defITEM_MODULE_LEN];	// model
	short	sShipFlag;						// ship flag
	short	sShipType;						// hull model
	short	sType;							// type

	char	chForgeLv;						// refine level
	char	chForgeSteady;					// stability value (the stability of a refineable item; past this value every refine lowers the success chance by a fixed amount)
	char	chExclusiveID;					// whether it has a unique id
    char    chIsTrade;                      // tradable
    char    chIsPick;                       // can be picked up
    char    chIsThrow;                      // can be dropped
	char	chIsDel;						// can be destroyed
    long    lPrice;                         // trade price
	char	chBody[defITEM_BODY];			// body size restriction
	short	sNeedLv;						// required level
    char    szWork[MAX_JOB_TYPE];           // job

	int		nPileMax;						// largest number that fit in one inventory slot
	char	chInstance;						// whether it is instantiated
	char	szAbleLink[enumEQUIP_NUM];		// slot it can be equipped in
	char	szNeedLink[enumEQUIP_NUM];		// slot it requires
	char	chPickTo;						// 0 goes into the backpack when picked up, 1 goes into the cargo hold

	short	sStrCoef;						// strength coefficient bonus
	short	sAgiCoef;						// agility coefficient bonus
	short	sDexCoef;						// focus coefficient bonus
	short	sConCoef;						// constitution coefficient bonus
	short	sStaCoef;						// vitality coefficient bonus
	short	sLukCoef;						// luck coefficient bonus
	short	sASpdCoef;						// attack interval coefficient bonus
	short	sADisCoef;						// attack range coefficient bonus
	short	sMnAtkCoef;						// minimum attack coefficient bonus
	short	sMxAtkCoef;						// maximum attack coefficient bonus
	short	sDefCoef;						// defence coefficient bonus
	short	sMxHpCoef;						// maximum HP coefficient bonus
	short	sMxSpCoef;						// maximum SP coefficient bonus
	short	sFleeCoef;						// evasion rate coefficient bonus
	short	sHitCoef;						// hit rate coefficient bonus
	short	sCrtCoef;						// critical rate coefficient bonus
	short	sMfCoef;						// treasure find rate coefficient bonus
	short	sHRecCoef;						// HP regeneration rate coefficient bonus
	short	sSRecCoef;						// SP regeneration rate coefficient bonus
	short	sMSpdCoef;						// movement speed coefficient bonus
	short	sColCoef;						// resource gathering speed coefficient bonus

	short	sStrValu[2];					// strength constant bonus (minimum, maximum)
	short	sAgiValu[2];					// agility constant bonus
	short	sDexValu[2];					// focus constant bonus
	short	sConValu[2];					// constitution constant bonus
	short	sStaValu[2];					// vitality constant bonus
	short	sLukValu[2];					// luck constant bonus
	short	sASpdValu[2];					// attack interval constant bonus
	short	sADisValu[2];					// attack range constant bonus
	short	sMnAtkValu[2];					// minimum attack constant bonus
	short	sMxAtkValu[2];					// maximum attack constant bonus
	short	sDefValu[2];					// defence constant bonus
	short	sMxHpValu[2];					// maximum HP constant bonus
	short	sMxSpValu[2];					// maximum SP constant bonus
	short	sFleeValu[2];					// evasion rate constant bonus
	short	sHitValu[2];					// hit rate constant bonus
	short	sCrtValu[2];					// critical rate constant bonus
	short	sMfValu[2];						// treasure find rate constant bonus
	short	sHRecValu[2];					// HP regeneration rate constant bonus
	short	sSRecValu[2];					// SP regeneration rate constant bonus
	short	sMSpdValu[2];					// movement speed constant bonus
	short	sColValu[2];					// resource gathering speed constant bonus

	short	sPDef[2];						// physical resistance
	short	sLHandValu;						// the item works in the left hand
	short	sEndure[2];						// durability
	short	sEnergy[2];						// energy
	short	sHole;							// number of holes
	char	szAttrEffect[defITEM_ATTREFFECT_NAME_LEN];	// use effect
	short	sDrap;							// drap
	short	sEffect[defITEM_BIND_EFFECT_NUM][2];		// weapon attach effect and the matching dummy
	short	sItemEffect[2];					// item appearance effect
	short	sAreaEffect[2];					// effect shown while it lies on the ground
	short	sUseItemEffect[2];				// effect shown while it is used
    _TCHAR  szDescriptor[defITEM_DESCRIPTOR_NAME_LEN];   // description

	float		nCooldown;
	//short	sSeteffect[enumEQUIP_NUM][2];
	//_TCHAR  szDescriptor[5][defITEM_DESCRIPTOR_SET_LEN];

public:
    const char* GetIconFile();              // obtain the full icon file name
	static bool	IsVaildFusionID(CItemRecord* pItem); // add by ning.yan  2008-08-21
	
	bool		GetIsPile()							{ return nPileMax>1;	}

	// check whether the character's body size allows equipping it
	bool		IsAllowEquip( unsigned int nChaID );

	// check whether there is no body size restriction
	bool		IsAllEquip()						{ return chBody[0]==-1;	}

	void		RefreshData();

	int			GetTypeValue( int nType );

	bool		IsFaceItem()						{ return sItemEffect[0]!=0;	}

	bool		IsSendUseItem()						{ return sUseItemEffect[0]!=0;	}

	bool IsWeapon() const;
	bool IsArmor() const;
	bool IsNecklace() const;
	bool IsRing() const;
	bool IsGlove() const;
	bool IsBoot() const;
	bool IsHair() const;
	bool IsConsumable() const;
	bool IsMission() const;
	bool IsLockable() const;

public:
	short		sEffNum;					// number of weapon attach effects

private:
	bool		_IsBody[5];

};

class CItemRecordSet : public CRawDataSet
{
public:

	static CItemRecordSet* I() { return _Instance; }

	CItemRecordSet(int nIDStart, int nIDCnt, int nCol = 128)
		:CRawDataSet(nIDStart, nIDCnt, nCol)
	{
		_Instance = this;
		_Init();
	}

protected:

	static CItemRecordSet* _Instance; // acts like a singleton: keeps a reference to itself

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return new CItemRecord[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (CItemRecord*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(CItemRecord);
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
	virtual void _ProcessRawDataInfo(CRawDataInfo *pInfo);
};

inline CItemRecord* GetItemRecordInfo( int nTypeID )
{
	return (CItemRecord*)CItemRecordSet::I()->GetRawDataInfo(nTypeID);
}

inline CItemRecord* GetItemRecordInfo(const char *pszItemName)
{
    return (CItemRecord*)CItemRecordSet::I()->GetRawDataInfo(pszItemName);
}

inline bool CItemRecord::IsAllowEquip( unsigned int nChaID )	
{ 
	if( nChaID<5 ) return _IsBody[nChaID];

	return false;
}

#endif //ITEMRECORD_H