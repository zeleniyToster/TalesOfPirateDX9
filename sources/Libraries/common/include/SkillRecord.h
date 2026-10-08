//======================================================================================================================
// FileName: SkillRecord.h
// Creater: ZhangXuedong
// Date: 2004.09.01
// Comment: CSkillRecordSet class
//======================================================================================================================

#ifndef	SKILLRECORD_H
#define	SKILLRECORD_H

#include <tchar.h>
#include "util.h"
#include "TableData.h"
#include "SkillBag.h"

const char cchSkillRecordKeyValue = (char)0xfe;				// -1 means "either hand" and 0 means "bare handed", so it was changed to -2

#define	defSKILL_NAME_LEN	17
#define defSKILL_JOB_SELECT_NUM	9
#define defSKILL_ITEM_NEED_NUM	8
#define defSKILL_PRE_SKILL_NUM	3
#define defSKILL_ICON_NAME_LEN	17
#define defEFFECT_SELF_ATTR_NUM	2
#define defEFFECT_TAR_ATTR_NUM	2
#define defSELF_EFFECT_NUM		2
#define defEXPEND_ITEM_NUM	2
#define defSKILL_OPERATE_NUM	3
#define defSKILL_POSE_NUM       10
#define defSKILL_EFFECT_SCRIPT_LEN	33
#define defSKILL_RANGE_SET_SCRIPT	33
#define defSKILL_ACTION_EFFECT  3
#define defSKILL_ITEM_EFFECT    2

enum ESkillItemNeed
{
	enumSKILL_ITEM_NEED_TYPE	= 1,
	enumSKILL_ITEM_NEED_ID		= 2,
};

enum ESkillObjType
{
	// target selection for this skill: 1 self, 2 party members, 3 scene, 4 enemies, 5 everyone

	enumSKILL_TYPE_SELF		= 1,	// self
	enumSKILL_TYPE_TEAM		= 2,	// party members
	enumSKILL_TYPE_SCENE	= 3,	// scene effect
	enumSKILL_TYPE_ENEMY	= 4,	// enemies
	enumSKILL_TYPE_ALL		= 5,	// everyone
	enumSKILL_TYPE_PLAYER_DIE	= 6,	// player corpse
	enumSKILL_TYPE_EXCEPT_SELF  = 7,    // every character and monster except self

	// special skill type
	enumSKILL_TYPE_REPAIR	= 17,	// repair
	enumSKILL_TYPE_TREE		= 18,	// chop trees
	enumSKILL_TYPE_MINE		= 19,	// mine ore
	enumSKILL_TYPE_TRADE	= 22,	// run a stall
	enumSKILL_TYPE_FISH		= 28,	// fish
	enumSKILL_TYPE_SALVAGE	= 29,	// salvage wrecks

};

enum eSelectCha		// client side only
{
	enumSC_NONE = 0,		// cannot be used on everyone
	enumSC_ALL,				// usable on everyone
	enumSC_PLAYER,			// other players, excluding self; used for menus

	enumSC_ENEMY,			// enemies: monsters plus players not on the same party when PK is allowed, otherwise only monsters
	enumSC_PLAYER_ASHES,	// player corpse (already dead)

	enumSC_MONS,			// ordinary monster
	enumSC_MONS_REPAIRABLE, // repairable monster
	enumSC_MONS_TREE,       // tree monster
	enumSC_MONS_MINE,		// ore monster
	enumSC_MONS_FISH,		// fish monster
	enumSC_MONS_DBOAT,		// wreck monster

	enumSC_SELF,			// the player itself
	enumSC_TEAM,			// party member
};

enum ESkillEffType
{
	enumSKILL_EFF_BANEFUL	= 0,	// harmful
	enumSKILL_EFF_HELPFUL	= 1,	// beneficial
};

enum ESkillUpgrade
{
	enumSKILL_UPGRADE_NONE,			// cannot be upgraded
	enumSKILL_UPGRADE_CAN,			// can be upgraded
	enumSKILL_UPGRADE_MAX,			// already at maximum level
};

enum ESkillFightType
{
	enumSKILL_LAND_LIVE		= 0, // land life skill
	enumSKILL_FIGHT			= 1, // combat skill
	enumSKILL_SAIL			= 2, // sailing skill
	enumSKILL_SEE_LIVE		= 3, // sea life skill
};

enum ESkillSrcType
{
	enumSKILL_SRC_HUMAN	= 1,	// character
	enumSKILL_SRC_BOAT	= 2,	// ship
};

enum ESkillTarHabitatType
{
	enumSKILL_TAR_LAND	= 1,	// land
	enumSKILL_TAR_SEA	= 2,	// sea
	enumSKILL_TAR_LORS	= 3,	// amphibious
};

enum ESkillType
{
	enumSKILL_INBORN	= 0,	// innate skill
	enumSKILL_ACTIVE	= 1,	// active skill
	enumSKILL_PASSIVE	= 2,	// passive skill
};

enum ESkillPhaseType
{
	enumSKILL_NOT_MANUAL_ADD   = 6,	// players may not upgrade this manually
};


class CSkillRecord : public CRawDataInfo
{
public:
	CSkillRecord();

	short	sID;												// id
	_TCHAR	szName[defSKILL_NAME_LEN];							// name
	char    chFightType;										// skill type: life skill, combat skill
	char	chJobSelect[defSKILL_JOB_SELECT_NUM][2];			// jobs that may learn this skill, and the highest level each can reach
																// 0 monster, 1 swordsman, 2 sniper, 3 sailor, 4 navigator, 5 merchant, 6 doctor, 7 technician, 8 all
	short	sItemNeed[3][defSKILL_ITEM_NEED_NUM][2];			// left (0) / right (1) hand, body (2); the matching item must be equipped before the skill can be used
	short	sConchNeed[defSKILL_ITEM_NEED_NUM][3];				// shell requirements (slot, type, id; slot, type, id; ...)
	char	chPhase;											// skill stage: 1 innate, 2 basic, 3 advanced, 4 ultimate, 5 sea egg; 6 whether skill levels may be added manually
	char	chType;												// skill type: 1 active (cast manually), 2 passive (fires automatically)
	short	sLevelDemand;										// level required to learn this skill
	short	sPremissSkill[defSKILL_PRE_SKILL_NUM][2];			// the three prerequisite skills (skill id, level)
	char	chPointExpend;										// skill points consumed
	char	chSrcType;											// caster form (character, ship)
	char	chTarType;											// target type (land creature, sea creature)
	short	sApplyDistance;										// use distance, in metres
	char	chApplyTarget;										// the target used
	char	chApplyType;										// effect type: 1 single target, 2 area, 3 splash
	char	chHelpful;											// whether the skill is beneficial or harmful
	short	sAngle;												// angle (0-360), used by the "area" attack type
	short	sRadii;												// radius, used by the "area" attack type
	char	chRange;											// skill area shape id
	char	szPrepare[defSKILL_RANGE_SET_SCRIPT];				// prepare the skill
	char	szUseSP[defSKILL_EFFECT_SCRIPT_LEN];				// SP cost formula
	char	szUseEndure[defSKILL_EFFECT_SCRIPT_LEN];			// durability cost formula
	char	szUseEnergy[defSKILL_EFFECT_SCRIPT_LEN];			// energy cost formula
	char	szSetRange[defSKILL_EFFECT_SCRIPT_LEN];				// skill area settings
	char	szRangeState[defSKILL_EFFECT_SCRIPT_LEN];			// terrain state settings
	char	szUse[defSKILL_EFFECT_SCRIPT_LEN];					// cast stage formula
	char	szEffect[defSKILL_EFFECT_SCRIPT_LEN];				// effect stage formula
	char	szActive[defSKILL_EFFECT_SCRIPT_LEN];				// positive effect formula
	char	szInactive[defSKILL_EFFECT_SCRIPT_LEN];				// negative effect formula
	int		nStateID;											// ids of the bound states that can be removed manually
	short	sSelfAttr[defEFFECT_SELF_ATTR_NUM];					// change to the caster's own attribute values; formula id
	short	sSelfEffect[defSELF_EFFECT_NUM];					// effect on self
	short	sItemExpend[defEXPEND_ITEM_NUM][2];					// items consumed (id, count)
	short	sBeingTime;											// duration
	short	sTargetAttr[defEFFECT_TAR_ATTR_NUM];				// change to the target's attribute values; formula id
	short	sSplashPara;										// splash parameters
	short	sTargetEffect;										// lasting effect on the target
	short	sSplashEffect;										// lasting effect from the splash
	short	sVariation;											// transformation id
	short	sSummon;											// summon id
	short	sPreTime;											// cast time
	char	szFireSpeed[defSKILL_EFFECT_SCRIPT_LEN];			// recast speed formula
	char	chOperate[defSKILL_OPERATE_NUM];					// special action: 0 none, 1 board ship, 2 disembark

public:		// for client side presentation
	short	sActionHarm;										// how damage is presented when the action effect plays
	char	chActionPlayType;									// action playback: 0 sequential, 1 random
	short	sActionPose[defSKILL_POSE_NUM];						// the caster's action for this skill
	short	sActionKeyFrme;										// key frame of the character casting animation
	short	sWhop;												// attack sound
	short	sActionDummyLink[defSKILL_ACTION_EFFECT];			// link point on the caster character
	short	sActionEffect[defSKILL_ACTION_EFFECT];				// the caster's effect for this skill
	short	sActionEffectType[defSKILL_ACTION_EFFECT];			// how the skill is released: 0 no scaling, 1 scales automatically
	short	sItemDummyLink;										// link point on the caster's item
	short	sItemEffect1[defSKILL_ITEM_EFFECT];					// caster item effect; bit 0 = item effect, bit 1 = execution timing
	short	sItemEffect2[defSKILL_ITEM_EFFECT];					// 
	short	sSkyEffectActionKeyFrame;							// key frame of the outgoing effect animation
	short   sSkyEffectActionDummyLink;							// dummy at the flight origin on the character
	short   sSkyEffectItemDummyLink;							// 飞行出发点道具dummy;
	short   sSkyEffect;											// the flight effect for this skill
	short	sSkySpd;											// flight speed
	short	sWhoped;											// hit sound
	short   sTargetDummyLink;									// link point on the target being hit
	short	sTargetEffectID;									// hit effect
	char	chTargetEffectTime;									// hit effect timing: 0 action start, 1 key frame, 2 action end (no effect for flying damage)
    short   sAgroundEffectID;                                   // for area attacks, the effect that appears on the ground (including water)
	short	sWaterEffectID;										// for area attacks, the effect that appears on the water surface
	char	szICON[defSKILL_ICON_NAME_LEN];						// the icon this skill shows in the game UI
	char	chPlayTime;											// number of times the animation plays
	char	szDescribeHint[128];								// description, shown in the tooltip
	char	szEffectHint[128];									// effect, shown in the tooltip
	char	szExpendHint[128];									// cost, shown in the tooltip

public:
    bool    IsPlayCyc()         { return chPlayTime==1;     }   // whether playback loops
    bool    IsAttackArea()		{ return chApplyType==2;    }   // whether it attacks the ground
	bool	IsHarmRange()		{ return chApplyType!=1;	}	// whether it deals area damage
    bool    IsActKeyHarm()      { return sActionHarm==1;    }   // whether damage lands on the action key frame
    bool    IsEffectHarm()      { return sActionHarm==2;    }   // whether damage comes from the effect
    bool    IsNoHarm()          { return sActionHarm==0;    }   // skill deals no damage
	bool	IsShow()			{ return szICON[1]!='\0';	}	// whether it is displayed
	bool	IsPlayRand()		{ return chActionPlayType==1;	}
	int		GetPoseNum()		{ return _nPoseNum;			}

public:
    void    SetSkillGrid( SSkillGridEx& v )   { _Skill = v;   }
    SSkillGridEx&     GetSkillGrid()  { return _Skill;        }

	int     GetSPExpend()       { return _Skill.sUseSP;     }   // SP cost
    short   GetRange()          { return _Skill.sRange[1];  }   // attack range
    short*  GetParam()          { return &_Skill.sRange[1]; }
    int     GetShape()          { return _Skill.sRange[0];  }   // skill shape
    int     GetDistance()       { return sApplyDistance;    }   // use distance
    int     GetLevel()          { return _Skill.chLv;       }
    int     GetFireSpeed()      { return _Skill.lResumeTime;}   // attack speed
    bool    GetIsValid()        { return _Skill.chState!=0; }   // whether it is usable

	int     GetUpgrade()		{ return _nUpgrade;			}
	bool    GetIsUpgrade()		{ return _nUpgrade==enumSKILL_UPGRADE_CAN;	}

	bool	GetIsUse()			{ return chType!=2;			}
	bool	GetIsHelpful()		{ return chHelpful==1;		}	// the skill is beneficial, so it may be used in a safe zone

	void	Refresh( int nJob );

	void	SetIsActive( bool v )	{ _IsActive = v;		}
	bool	GetIsActive()			{ return _IsActive;		}

	void	SetAttackTime( DWORD v ){ _dwAttackTime=v;			}
	bool	IsAttackTime( DWORD v )	{ return v>=_dwAttackTime;	}

	int		GetSelectCha()		{ return _eSelectCha;		}

	bool	IsAutoAttack()		{ return !IsPlayCyc() && GetDistance()>0;	}

	void	RefreshPrivateData();				// refresh the internal data
	
	bool	IsJobAllow( int nJob );
	int		GetJobMax( int nJob );

private:
    SSkillGridEx		_Skill;
	int					_nUpgrade;				// 0 cannot be upgraded, 1 can be upgraded, 2 already at maximum level

	bool				_IsActive;				// for the animation: draw an animated border around the active skill
	DWORD				_dwAttackTime;			// time of the next possible attack

	eSelectCha			_eSelectCha;			// used by scene logic to decide which players may be attacked

	int					_nPoseNum;				// total number of poses

	// added by clp
public:
	bool IsReadingSkill()
	{
		return sID == 512;
	}
};

class CSkillRecordSet : public CRawDataSet
{
public:

	static CSkillRecordSet* I() { return _Instance; }

	CSkillRecordSet(int nIDStart, int nIDCnt, int nCol = 128)
		:CRawDataSet(nIDStart, nIDCnt, nCol)
	{
		_Instance = this;
		_Init();
	}

protected:

	static CSkillRecordSet* _Instance; // acts like a singleton: keeps a reference to itself

	virtual CRawDataInfo* _CreateRawDataArray(int nCnt)
	{
		return new CSkillRecord[nCnt];
	}

	virtual void _DeleteRawDataArray()
	{
		delete[] (CSkillRecord*)_RawDataArray;
	}

	virtual int _GetRawDataInfoSize()
	{
		return sizeof(CSkillRecord);
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

inline CSkillRecord* GetSkillRecordInfo( int nTypeID )
{
	return (CSkillRecord*)CSkillRecordSet::I()->GetRawDataInfo(nTypeID);
}

inline CSkillRecord* GetSkillRecordInfo( const char* szName )
{
	return (CSkillRecord*)CSkillRecordSet::I()->GetRawDataInfo(szName);
}

inline bool CSkillRecord::IsJobAllow( int nJob )
{
	if( chJobSelect[0][0]==-1 ) return true;

	for (char i = 0; i<defSKILL_JOB_SELECT_NUM; i++ )
	{
		if (chJobSelect[i][0] == cchSkillRecordKeyValue)
			return false;
		else if( chJobSelect[i][0]==nJob )
			return true;
	}
	return false;
}

inline int CSkillRecord::GetJobMax( int nJob )
{
	if( chJobSelect[0][0]==-1 ) return chJobSelect[0][1];

	for (int i = 0; i<defSKILL_JOB_SELECT_NUM; i++ )
	{
		if (chJobSelect[i][0] == cchSkillRecordKeyValue)
			return -1;
		else if( chJobSelect[i][0]==nJob )
			return chJobSelect[i][1];
	}

	return -1;
}
#endif //SKILLRECORD_H