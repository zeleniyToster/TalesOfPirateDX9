//=============================================================================
// FileName: SkillStateRecord.h
// Creater: ZhangXuedong
// Date: 2005.02.04
// Comment: Skill State Record
//=============================================================================

#ifndef SKILLSTATERECORD_H
#define SKILLSTATERECORD_H

#include <tchar.h>
#include "util.h"
#include "TableData.h"

const char cchSkillStateRecordKeyValue = (char)0xff;

#define defSKILLSTATE_NAME_LEN		17
#define defSKILLSTATE_SCRIPT_NAME	32
#define defSKILLSTATE_ACT_NUM		3
#define defSKILLSTATE_DESC_NAME_LEN	255

class CSkillStateRecord : public CRawDataInfo
{
public:
	// CSkillStateRecord();

	char	chID;									// id
	char	szName[defSKILLSTATE_NAME_LEN];			// name
	short	sFrequency;								// interval between ticks, in seconds
	char	szOnTransfer[defSKILLSTATE_SCRIPT_NAME];// script applying the state to the target
	char	szAddState[defSKILLSTATE_SCRIPT_NAME];	// script for the use effect
	char	szSubState[defSKILLSTATE_SCRIPT_NAME];	// script for the removal effect
	char	chAddType;								// replacement policy
	bool	bCanCancel;								// whether the client may cancel it
	bool	bCanMove;								// whether the character can move
	bool	bCanMSkill;								// whether magic skills can be used
	bool	bCanGSkill;								// whether normal skills can be used
	bool	bCanTrade;								// whether trading is allowed
	bool	bCanItem;								// whether items can be used
	bool	bCanUnbeatable;							// whether the character is invulnerable
	bool	bCanItemmed;							// whether items can be used on the character
	bool	bCanSkilled;							// whether skills can be used on the character
	bool	bNoHide;								// whether the character is not invisible
	bool	bNoShow;								// whether the character is not force-revealed
	bool	bOptItem;								// whether items can be manipulated
	bool	bTalkToNPC;								// whether the character can talk to NPCs
	char	bFreeStateID;							// id of the effect that is removed

	// client side presentation
	char	chScreen;								// screen effect
	char    nActBehave[defSKILLSTATE_ACT_NUM];		// action presentation
	short	sChargeLink;							// client presentation flag, used to show the shell charging effect
    short   sAreaEffect;                            // lasting ground effect
	bool	IsShowCenter;							// ground state effect; whether only the centre point is drawn
	bool	IsDizzy;								// whether the stun effect is shown
	short	sEffect;								// effect presentation for the entity the effect is applied to
	short	sDummy1;								// dummy point used to play the effect on the affected player
	short	sBitEffect;								// hit effect shown while in this state
	short	sDummy2;								// dummy point used to play the effect
	short	sIcon;									// icon shown for the effect while it is attached to the character
	char	szIcon[defSKILLSTATE_NAME_LEN][10];		// icons for pots per level 
	char	szDesc[defSKILLSTATE_DESC_NAME_LEN];
	int		lColour;
public:
	void	RefreshPrivateData();

	int		GetActNum()			{ return _nActNum;		}

public:
	int		_nActNum;								// number of action presentations

};

class CSkillStateRecordSet : public CRawDataSet
{
public:

	static CSkillStateRecordSet* I() { return _Instance; }

	CSkillStateRecordSet(int nIDStart, int nIDCnt, int nCol = 128)
		:CRawDataSet(nIDStart, nIDCnt, nCol)
	{
		_Instance = this;
		_Init();
	}

protected:

	static CSkillStateRecordSet* _Instance; // acts like a singleton: keeps a reference to itself

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return new CSkillStateRecord[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (CSkillStateRecord*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(CSkillStateRecord);
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

inline CSkillStateRecord* GetCSkillStateRecordInfo( int nTypeID )
{
	return (CSkillStateRecord*)CSkillStateRecordSet::I()->GetRawDataInfo(nTypeID);
}

#endif // SKILLSTATERECORD_H