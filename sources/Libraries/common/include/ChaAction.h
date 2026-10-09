//=============================================================================
// FileName: ChaAction.h
// Creater: ZhangXuedong
// Date: 2005.09.15
// Comment: Character Action Control Type
//=============================================================================

#ifndef CHAACTION_H
#define CHAACTION_H

const long	ACTCONTROL_MOVE         = 0; // movement bit
const long	ACTCONTROL_USE_GSKILL   = 1; // normal skill use bit
const long	ACTCONTROL_USE_MSKILL   = 2; // magic skill use bit
const long	ACTCONTROL_BEUSE_SKILL  = 3; // being targeted by a skill bit
const long	ACTCONTROL_TRADE        = 4; // trade bit
const long	ACTCONTROL_USE_ITEM     = 5; // item use bit
const long	ACTCONTROL_BEUSE_ITEM   = 6; // being targeted by an item bit
const long	ACTCONTROL_INVINCIBLE   = 7; // invulnerable bit
const long	ACTCONTROL_EYESHOT      = 8; // vision bit (sees whatever is visible in range)
const long	ACTCONTROL_NOHIDE       = 9; // not invisible, i.e. can be seen
const long	ACTCONTROL_NOSHOW       = 10; // not force-revealed (force reveal suppresses invisibility)
const long	ACTCONTROL_ITEM_OPT     = 11; // item interaction bit
const long	ACTCONTROL_TALKTO_NPC   = 12; // talk to NPC bit

const long	ACTCONTROL_MAX          = 13;

#endif // CHAACTION_H