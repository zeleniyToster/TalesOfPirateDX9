//=============================================================================
// FileName: FightAble.h
// Creater: ZhangXuedong
// Date: 2004.09.15
// Comment: CFightAble class
//=============================================================================

#ifndef FIGHTABLE_H
#define FIGHTABLE_H

#include "Attachable.h"
#include "ChaAttr.h"
#include "CharacterRecord.h"
#include "SkillRecord.h"
#include "GameCommon.h"
#include "SkillState.h"
#include "SkillBag.h"
#include "TryUtil.h"
#include "Timer.h"
#include "SkillTemp.h"

enum EItemInstance // this must stay in sync with the scripts
{
	enumITEM_INST_BUY	= 0,		// shop trade
	enumITEM_INST_MONS	= 1,		// monster loot
	enumITEM_INST_COMP	= 2,		// synthesis
	enumITEM_INST_TASK	= 3,		// obtained from a mission
};

enum EFightChaType
{
	enumFIGHT_CHA_SRC		= 0,	// attacker
	enumFIGHT_CHA_TAR		= 1,	// the one being hit
	enumFIGHT_CHA_SPLASH	= 2,	// the one splashed
};

struct SFireUnit
{
#ifdef defPROTOCOL_HAVE_PACKETID
	dbc::uLong	ulPacketID;	// packet ID
#endif
	dbc::uChar	uchFightID;

	CFightAble	*pCFightSrc;
	dbc::uLong	ulID;
	Point		SSrcPos;
	dbc::Long	lTarInfo1;
	dbc::Long	lTarInfo2;

	dbc::Short		sExecTime;	// execution count
	CSkillRecord	*pCSkillRecord;
	CSkillTempData	*pCSkillTData;
};

struct SFightInit
{
	CSkillRecord	*pCSkillRecord;
	SSkillGrid		*pSSkillGrid;
	CSkillTempData	*pCSkillTData;
	// lInfo1,lInfo2 are WorldID and Handle respectively if the target is an entity, otherwise they are the x and y of the coordinates
	struct
	{
		dbc::Char		chTarType;	// 0, no target. 1, the target is an entity. 2, the target is coordinates
		dbc::Long		lTarInfo1;
		dbc::Long		lTarInfo2;
	};

	dbc::Short		sStopState;		// state after the skill stops (enumEXISTS_WAITING, enumEXISTS_SLEEPING)
};

/*
*	可战斗实体
*	lark.li
*/
class	CFightAble : public CAttachable
{
public:
	struct SFightProc
	{
		enum class Request
		{
			None,
			StopAttack,
			StartAttack
		};

		dbc::Short	sState;
		Request	sRequestState;

		bool		bCrt;			// critical hit
		bool		bMiss;			// Miss

		long		lERangeBParam[defSKILL_RANGE_BASEP_NUM];	// base area parameters (centre coordinates, direction)
	};

	dbc::Short	GetFightState(void) {return m_SFightProc.sState;}
	dbc::Short	GetFightStopState(void) {return m_SFightInit.sStopState;}

	bool	DesireFightBegin(SFightInit *);
	void	DesireFightEnd(void) {EndFight();}
	void	OnFight(dbc::uLong ulCurTick);

	void	RangeEffect(SFireUnit *pSFireSrc, SubMap *pCMap, dbc::Long *plRangeBParam);
	void	SkillTarEffect(SFireUnit *pSFire);
	void	NotiSkillSrcToEyeshot(dbc::Short sExecTime = 0);
	void	NotiSkillSrcToSelf(dbc::Short sExecTime = 0);
	void	NotiSkillTarToEyeshot(SFireUnit *pSFireSrc);
	void	NotiChangeMainCha(dbc::uLong ulTargetID);
	void	SynAttr(dbc::Short sType);
	void	SynAttrToSelf(dbc::Short sType);
	void	SynAttrToEyeshot(dbc::Short sType);
	void	SynAttrToUnit(CFightAble *pCObj, dbc::Short sType);
	void	SynAttrToUnit(CFightAble *pCObj, dbc::Short sStartAttr, dbc::Short sEndAttr, dbc::Short sType);
	void	SynSkillStateToSelf(void);
	void	SynSkillStateToEyeshot(void);
	void	SynSkillStateToUnit(CFightAble *pCObj);
	void	SynLookEnergy(void);
	// packet organisation
	void	WriteSkillState(WPACKET &pk);
	void	WriteAttr(WPACKET &pk, dbc::Short sSynType);
	void	WriteMonsAttr(WPACKET &pk, dbc::Short sSynType);
	void	WriteAttr(WPACKET &pk, dbc::Short sStartAttr, dbc::Short sEndAttr, dbc::Short sSynType);
	void	WriteLookEnergy(WPACKET &pk);

	bool	IsRightSkill(CSkillRecord *pSkill);
	bool	IsRightSkillSrc(dbc::Char chSkillEffType);
	bool	IsRightSkillTar(CFightAble *pSkillSrc, dbc::Char chSkillObjType, dbc::Char chSkillObjHabitat, dbc::Char chSkillEffType, bool bIncHider = false);
	bool	IsTeammate(CFightAble *pCFighter);
	bool	IsFriend(CFightAble *pCFighter);

	void	ResetFight();
	bool	RectifyAttr();

	dbc::Long	GetLevel(void) {return (long)m_CChaAttr.GetAttr(ATTR_LV);}
	void		AddExp(dbc::uLong);
	bool		AddExpAndNotic(dbc::Long lAddExp, dbc::Short sNotiType = enumATTRSYN_TASK);

	void	CountLevel(void);
	void	CountSailLevel(void);
	void	CountLifeLevel(void);

	// mission event handling interface
	virtual void AfterObjDie(CCharacter *pCAtk, CCharacter *pCDead) {}
	virtual void OnLevelUp( USHORT sLevel ) {};
	virtual void OnSailLvUp( USHORT sLevel ) {};
	virtual void OnLifeLvUp( USHORT sLevel ) {};

	// gather a resource, drop an item
	void	SpawnResource( CCharacter *pCAtk, dbc::Long lSkillLv );
	void	ItemCount(CCharacter *pAtk);
	void	ItemInstance(dbc::Char chType, SItemGrid *pGridContent, BOOL isTradable = 1, LONG expiration = 0);
	bool	GetTrowItemPos(dbc::Long *plPosX, dbc::Long *plPosY);
	bool	SkillExpend(dbc::Short sExecTime = 1);

	dbc::uLong	GetSkillDist(Entity *pTarEnt, CSkillRecord *pRec)
	{
		if (!pRec) return 0;
		if (pTarEnt) return GetRadius() + pTarEnt->GetRadius() + pRec->sApplyDistance;
		else return GetRadius() + pRec->sApplyDistance;
	}
	bool	SkillTarIsEntity(CSkillRecord *pRec)
	{
		if (pRec && (pRec->chApplyType == 1 || pRec->chApplyType == 3)) return true;
		else return false;
	}

	void			BeUseSkill(dbc::Long lPreHp, dbc::Long lNowHp, CCharacter *pCSrcCha, dbc::Char chSkillEffType);
	void			SetMonsterFightObj(dbc::uLong ulObjWorldID, dbc::Long lObjHandle);
	dbc::Long		GetSkillTime(CSkillTempData *pCSkillTData);
	void			EnrichSkillBag(bool bActive = true);
	virtual bool	AddSkillState(dbc::uChar uchFightID, dbc::uLong ulSrcWorldID, dbc::Long lSrcHandle, dbc::Char chObjType, dbc::Char chObjHabitat, dbc::Char chEffType,
					dbc::uChar uchStateID, dbc::uChar uchStateLv, dbc::Long lOnTick, dbc::Char chType = enumSSTATE_ADD_UNDEFINED, bool bNotice = true){return false;}
	virtual bool	DelSkillState(dbc::uChar uchStateID, bool bNotice = true){return false;}
	void			SetItemHostObj(CFightAble *pCObj) {m_pCItemHostObj = pCObj;}

	dbc::Long		setAttr(int nIdx, LONG32 lValue, int nType = 0);
	dbc::Long		getAttr(int nIdx) {return m_CChaAttr.GetAttr(nIdx);}
	// 64-битный доступ к опыту (см. ChaAttr.h, ATTR_IS_EXP64)
	LONG64			getAttr64(int nIdx) {return m_CChaAttr.GetAttr64(nIdx);}
	dbc::Long		setAttr64(int nIdx, LONG64 lValue, int nType = 0);
	virtual void	AfterAttrChange(int nIdx, dbc::Long lOldVal, dbc::Long lNewVal) {};
	void			SetDie(CCharacter *pCSkillSrcCha);
	virtual void	Die(){};

	CCharacter* SkillPopBoat(dbc::Long lPosX, dbc::Long lPosY, dbc::Short sDir = -1);	// launch a boat
	bool SkillPopBoat(CCharacter *pCBoat, dbc::Long lPosX, dbc::Long lPosY, dbc::Short sDir = -1);	// launch a boat
	bool SkillInBoat(CCharacter* pCBoat);	// board a boat
	bool SkillOutBoat(dbc::Long lPosX, dbc::Long lPosY, dbc::Short sDir = -1);	// disembark from a boat
	bool SkillPushBoat(CCharacter* pCBoat, bool bFree = true);	// recall a boat

	dbc::uLong	m_ulPacketID;		// packet ID
	dbc::uChar	m_uchFightID;		// the attack number, used only so the client can match it up

	SFightInit	m_SFightInit;
	SFightProc	m_SFightProc;
	SFightInit	m_SFightInitCache;

	CChaAttr		m_CChaAttr;
	CChaRecord		*m_pCChaRecord;
	CSkillState		m_CSkillState;
	CSkillBag		m_CSkillBag;
	dbc::Short		m_sDefSkillNo;

	//virtual bool IsBoat(void);

protected:
	CFightAble();
	void	Initially();
	void	Finally();

	CFightAble	*	IsFightAble(){return this;}
	void	WritePK(WPACKET& wpk);			// writes the player itself and all attached structures (summons and the like)
	void	ReadPK(RPACKET& rpk);			// rebuilds the player itself and all attached structures (summons and the like)

	bool	GetFightTargetShape(Square *pSTarShape);

	
	void	OnSkillState(DWORD dwCurTick);
	void	RemoveOtherSkillState();
	void	RemoveAllSkillState();

private:
	virtual void BeginFight();
	virtual void EndFight();
	void OnFightBegin(void) {m_bOnFight = true;}
	void OnFightEnd(void) {m_bOnFight = false;}

	virtual void SubsequenceFight(){};

	virtual void BreakAction(RPACKET pk = NULL) {};
	virtual void EndAction(RPACKET pk = NULL) {}

	bool SkillGeneral(dbc::Long lDistance, dbc::Short sExecTime = 1); // ordinary skill

	dbc::uShort	m_usTickInterval;	// combat execution heartbeat (frequency), in milliseconds
	dbc::uLong	m_ulLastTick;		// in milliseconds
	bool		m_bOnFight;

	bool		m_bLookAttrChange;	// appearance attribute changed
	CFightAble*	m_pCItemHostObj;	// owner of the dropped loot

};

class	CTimeSkillMgr
{
public:
	struct SMgrUnit
	{
		SFireUnit	SFireSrc;
		dbc::uLong	ulLeftTick;	// remaining time
		SubMap		*pCMap;
		Point		STargetPos;	// target position
		long		lERangeBParam[defSKILL_RANGE_BASEP_NUM];	// base area parameters (centre coordinates, direction)
		SMgrUnit	*pSNext;
	};

	CTimeSkillMgr(unsigned short usFreq = 1000);
	~CTimeSkillMgr();

	void	Add(SFireUnit *pSFireSrc, dbc::uLong ulLeftTick, SubMap *pCMap, Point *pStarget, dbc::Long *lRangeBParam);
	void	Run(unsigned long ulCurTick);
	void	ExecTimeSkill(SMgrUnit *pFireInfo);

private:
	unsigned long	m_ulTick;
	unsigned short	m_usFreq;	// execution heartbeat (frequency)

	SMgrUnit	*m_pSExecQueue;	// running queue
	SMgrUnit	*m_pSFreeQueue;	// idle queue

};

extern CTimeSkillMgr	g_CTimeSkillMgr;
extern char	g_chItemFall[defCHA_INIT_ITEM_NUM + 1];

#endif // FIGHTABLE_H