//=============================================================================
// FileName: SkillStateType.h
// Creater: ZhangXuedong
// Date: 2005.02.04
// Comment: Skill State Type
//=============================================================================

#ifndef SKILLSTATETYPE_H
#define SKILLSTATETYPE_H

const unsigned char	SSTATE_NONE			= 0;	// empty state

// the first eight are also used for ground states
const unsigned char	SSTATE_CUT			= 1;	// flame slash
const unsigned char	SSTATE_FERVOR_ARROR	= 2;	// searing arrow
const unsigned char	SSTATE_FROST_ARROR	= 3;	// freezing arrow
const unsigned char	SSTATE_SKYROCKET	= 4;	// fireworks show
const unsigned char	SSTATE_MURRAIN		= 5;	// plague
const unsigned char	SSTATE_GIDDY		= 6;	// stun
const unsigned char	SSTATE_FREEZE		= 7;	// paralysis
const unsigned char	SSTATE_SLEEP		= 8;	// sleep
const unsigned char	SSTATE_BIND			= 9;	// bind
const unsigned char	SSTATE_FROST		= 10;	// frozen
const unsigned char	SSTATE_BEAT_BACK	= 11;	// knock back
const unsigned char	SSTATE_UNBEATABLE	= 12;	// invulnerable
const unsigned char	SSTATE_TOXIN		= 13;	// voodoo
const unsigned char	SSTATE_REBOUND		= 14;	// reflect
const unsigned char	SSTATE_AVATAR		= 15;	// transform
const unsigned char	SSTATE_TITAN		= 16;	// giant
const unsigned char	SSTATE_BLINDNESS	= 17;	// blind
const unsigned char	SSTATE_HAIR			= 18;	// haircut
const unsigned char	SSTATE_FLOAT		= 19;	// levitate
const unsigned char	SSTATE_CALL			= 20;	// summon
const unsigned char	SSTATE_SHIELD		= 21;	// shield proficiency
const unsigned char	SSTATE_TIGER		= 22;	// tiger roar
const unsigned char	SSTATE_HIDE			= 43;	// invisible
const unsigned char	SSTATE_TRADE		= 85;	// trade
const unsigned char	SSTATE_STALL		= 99;	// stall
const unsigned char	SSTATE_REPAIR		= 100;	// repair
const unsigned char	SSTATE_FORGE		= 101;	// refine


const unsigned char	AREA_STATE_MAXID	= 255;

// Modify by lark.li 20080828 begin
//const unsigned char	SKILL_STATE_MAXID	= 240;
const unsigned char	SKILL_STATE_MAXID	= 254;
// End

const unsigned char	SKILL_STATE_LEVEL	= 20;

const long	SSTATE_SIGN_BYTE_NUM	= (SKILL_STATE_MAXID + sizeof(char) * 8 - 1) / (sizeof(char) * 8);

const unsigned char	AREA_STATE_NUM		= AREA_STATE_MAXID;
const unsigned char	CHA_STATE_NUM		= 16;

#endif // SKILLSTATETYPE_H
