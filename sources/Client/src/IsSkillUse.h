//----------------------------------------------------------------------
// Name: game event
// Author: lh 2004-05-26
// Purpose: used to respond to events
// Implementation: an event must exist attached to a character or an item
//----------------------------------------------------------------------
#pragma once

class CSkillRecord;
class CCharacter;

// a small helper that decides whether a single target skill can be used
class CIsSkillUse
{
public:
	CIsSkillUse();

	// only decides whether the skill is available
	bool		IsValid( CSkillRecord* pSkill, CCharacter* pSelf );

	// used for the combined availability decision
	bool		IsUse( CSkillRecord* pSkill, CCharacter* pSelf, CCharacter* pTarget );

	// decides whether the skill can be used on a given player
	bool		IsAttack( CSkillRecord* pSkill, CCharacter* pSelf, CCharacter* pTarget );		

	// if it cannot be used, gives the reason
	const char*	GetError();	

private:
	enum eError
	{
		enumNone,

		enumSelf,
		enumFish,
		enumDieBoat,
		enumOnlyTeam,
		enumTree,
		enumMine,
		enumDie,
		enumRepair,

		enumAttackMain,
		enumAttackTeam,
		enumHelpMons,
		enumAttackDie,
		enumTargetError,
		enumAttackPlayer,

		enumInValid,
		enumNotEnergy,
		enumNotAttack,
		enumNotUse,
		enumNotMP,
		enumNotTarget,

	};
	eError			_eError;
	CSkillRecord*	_pSkill;

};

extern CIsSkillUse	g_SkillUse;
