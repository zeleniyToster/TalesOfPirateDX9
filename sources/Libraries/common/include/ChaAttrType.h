//=============================================================================
// FileName: ChaAttrType.h
// Creater: ZhangXuedong
// Date: 2004.12.13
// Comment: Character Attribute type
//=============================================================================

#ifndef ATTRTYPE_H
#define ATTRTYPE_H

const long	ATTR_COUNT_BASE0    = 0;
const long	ATTR_LV             = ATTR_COUNT_BASE0 + 0; // character level
const long	ATTR_HP             = ATTR_COUNT_BASE0 + 1; // current HP (durability, for a ship)
const long	ATTR_SP             = ATTR_COUNT_BASE0 + 2; // current SP (supplies, for a ship)
const long	ATTR_TITLE          = ATTR_COUNT_BASE0 + 3; // character title
const long	ATTR_JOB            = ATTR_COUNT_BASE0 + 4; // character job
const long	ATTR_FAME           = ATTR_COUNT_BASE0 + 5; // reputation
const long	ATTR_AP             = ATTR_COUNT_BASE0 + 6; // attribute points
const long	ATTR_TP             = ATTR_COUNT_BASE0 + 7; // skill points
const long	ATTR_GD             = ATTR_COUNT_BASE0 + 8; // money
const long	ATTR_SPRI           = ATTR_COUNT_BASE0 + 9; // shell speed
const long	ATTR_CHATYPE        = ATTR_COUNT_BASE0 + 10; // who controls the character (player, NPC, monster, ...)
const long	ATTR_SAILLV         = ATTR_COUNT_BASE0 + 11; // sailing level
const long	ATTR_LIFELV         = ATTR_COUNT_BASE0 + 12; // life skill level
const long	ATTR_LIFETP         = ATTR_COUNT_BASE0 + 13; // life skill points
const long	ATTR_BOAT_BERTH     = ATTR_COUNT_BASE0 + 14; // port the ship is moored at
//			// the order of the attributes in the range below must not change
const long	ATTR_CEXP           = ATTR_COUNT_BASE0 + 15; // current experience (proficiency, for a ship)
const long	ATTR_NLEXP          = ATTR_COUNT_BASE0 + 16; // experience needed for the next level
const long	ATTR_CLEXP          = ATTR_COUNT_BASE0 + 17; // experience gained at the current level
const long	ATTR_CLEFT_SAILEXP  = ATTR_COUNT_BASE0 + 18; // sailing experience still to gain at the current level
const long	ATTR_CSAILEXP       = ATTR_COUNT_BASE0 + 19; // accumulated sailing experience (the current sailing experience)
const long	ATTR_CLV_SAILEXP    = ATTR_COUNT_BASE0 + 20; // sailing experience earned at the current level
const long	ATTR_NLV_SAILEXP    = ATTR_COUNT_BASE0 + 21; // sailing experience needed for the next level
const long	ATTR_CLIFEEXP       = ATTR_COUNT_BASE0 + 22; // current life skill experience
const long	ATTR_CLV_LIFEEXP    = ATTR_COUNT_BASE0 + 23; // life skill experience at the current level
const long	ATTR_NLV_LIFEEXP    = ATTR_COUNT_BASE0 + 24; // life skill experience needed for the next level
//			//

const long	ATTR_COUNT_BASE1    = 25;
const long	ATTR_STR            = ATTR_COUNT_BASE1 + 0; // strength
const long	ATTR_DEX            = ATTR_COUNT_BASE1 + 1; // focus
const long	ATTR_AGI            = ATTR_COUNT_BASE1 + 2; // agility
const long	ATTR_CON            = ATTR_COUNT_BASE1 + 3; // constitution
const long	ATTR_STA            = ATTR_COUNT_BASE1 + 4; // vitality
const long	ATTR_LUK            = ATTR_COUNT_BASE1 + 5; // luck
const long	ATTR_MXHP           = ATTR_COUNT_BASE1 + 6; // maximum HP
const long	ATTR_MXSP           = ATTR_COUNT_BASE1 + 7; // maximum SP
const long	ATTR_MNATK          = ATTR_COUNT_BASE1 + 8; // minimum attack
const long	ATTR_MXATK          = ATTR_COUNT_BASE1 + 9; // maximum attack
const long	ATTR_DEF            = ATTR_COUNT_BASE1 + 10; // defence
const long	ATTR_HIT            = ATTR_COUNT_BASE1 + 11; // hit rate
const long	ATTR_FLEE           = ATTR_COUNT_BASE1 + 12; // evasion rate
const long	ATTR_MF             = ATTR_COUNT_BASE1 + 13; // treasure find rate
const long	ATTR_CRT            = ATTR_COUNT_BASE1 + 14; // critical rate
const long	ATTR_HREC           = ATTR_COUNT_BASE1 + 15; // HP regeneration rate
const long	ATTR_SREC           = ATTR_COUNT_BASE1 + 16; // SP regeneration rate
const long	ATTR_ASPD           = ATTR_COUNT_BASE1 + 17; // attack interval
const long	ATTR_ADIS           = ATTR_COUNT_BASE1 + 18; // attack range
const long	ATTR_MSPD           = ATTR_COUNT_BASE1 + 19; // movement speed
const long	ATTR_COL            = ATTR_COUNT_BASE1 + 20; // resource gathering speed
const long	ATTR_PDEF           = ATTR_COUNT_BASE1 + 21; // physical resistance
const long  ATTR_BOAT_CRANGE    = ATTR_COUNT_BASE1 + 22; // ship cannon blast radius
const long  ATTR_BOAT_CSPD      = ATTR_COUNT_BASE1 + 23; // ship cannon shell speed
const long  ATTR_BOAT_PRICE     = ATTR_COUNT_BASE1 + 24; // ship value

const long	ATTR_COUNT_BASE2    = 50;
const long	ATTR_BSTR           = ATTR_COUNT_BASE2 + 0; // base strength
const long	ATTR_BDEX           = ATTR_COUNT_BASE2 + 1; // base focus
const long	ATTR_BAGI           = ATTR_COUNT_BASE2 + 2; // base agility
const long	ATTR_BCON           = ATTR_COUNT_BASE2 + 3; // base constitution
const long	ATTR_BSTA           = ATTR_COUNT_BASE2 + 4; // base vitality
const long	ATTR_BLUK           = ATTR_COUNT_BASE2 + 5; // base luck
const long	ATTR_BMXHP          = ATTR_COUNT_BASE2 + 6; // base maximum HP
const long	ATTR_BMXSP          = ATTR_COUNT_BASE2 + 7; // base maximum SP
const long	ATTR_BMNATK         = ATTR_COUNT_BASE2 + 8; // base minimum attack
const long	ATTR_BMXATK         = ATTR_COUNT_BASE2 + 9; // base maximum attack
const long	ATTR_BDEF           = ATTR_COUNT_BASE2 + 10; // base defence
const long	ATTR_BHIT           = ATTR_COUNT_BASE2 + 11; // base hit rate
const long	ATTR_BFLEE          = ATTR_COUNT_BASE2 + 12; // base evasion rate
const long	ATTR_BMF            = ATTR_COUNT_BASE2 + 13; // base treasure find rate
const long	ATTR_BCRT           = ATTR_COUNT_BASE2 + 14; // base critical rate
const long	ATTR_BHREC          = ATTR_COUNT_BASE2 + 15; // base HP regeneration rate
const long	ATTR_BSREC          = ATTR_COUNT_BASE2 + 16; // base SP regeneration rate (consumption rate for a ship)
const long	ATTR_BASPD          = ATTR_COUNT_BASE2 + 17; // base attack interval
const long	ATTR_BADIS          = ATTR_COUNT_BASE2 + 18; // base attack range
const long	ATTR_BMSPD          = ATTR_COUNT_BASE2 + 19; // base movement speed
const long	ATTR_BCOL           = ATTR_COUNT_BASE2 + 20; // base resource gathering speed
const long	ATTR_BPDEF          = ATTR_COUNT_BASE2 + 21; // base physical resistance
const long  ATTR_BOAT_BCRANGE   = ATTR_COUNT_BASE2 + 22; // base ship shell blast radius
const long  ATTR_BOAT_BCSPD     = ATTR_COUNT_BASE2 + 23; // base ship shell speed

const long	ATTR_COUNT_BASE3    = 74;
const long	ATTR_ITEMC_STR      = ATTR_COUNT_BASE3 + 0; // strength item coefficient bonus
const long	ATTR_ITEMC_AGI      = ATTR_COUNT_BASE3 + 1; // agility item coefficient bonus
const long	ATTR_ITEMC_DEX      = ATTR_COUNT_BASE3 + 2; // focus item coefficient bonus
const long	ATTR_ITEMC_CON      = ATTR_COUNT_BASE3 + 3; // constitution item coefficient bonus
const long	ATTR_ITEMC_STA      = ATTR_COUNT_BASE3 + 4; // vitality item coefficient bonus
const long	ATTR_ITEMC_LUK      = ATTR_COUNT_BASE3 + 5; // luck item coefficient bonus
const long	ATTR_ITEMC_ASPD     = ATTR_COUNT_BASE3 + 6; // attack interval item coefficient bonus
const long	ATTR_ITEMC_ADIS     = ATTR_COUNT_BASE3 + 7; // attack range item coefficient bonus
const long	ATTR_ITEMC_MNATK    = ATTR_COUNT_BASE3 + 8; // minimum attack item coefficient bonus
const long	ATTR_ITEMC_MXATK    = ATTR_COUNT_BASE3 + 9; // maximum attack item coefficient bonus
const long	ATTR_ITEMC_DEF      = ATTR_COUNT_BASE3 + 10; // defence item coefficient bonus
const long	ATTR_ITEMC_MXHP     = ATTR_COUNT_BASE3 + 11; // maximum HP item coefficient bonus
const long	ATTR_ITEMC_MXSP     = ATTR_COUNT_BASE3 + 12; // maximum SP item coefficient bonus
const long	ATTR_ITEMC_FLEE     = ATTR_COUNT_BASE3 + 13; // evasion rate item coefficient bonus
const long	ATTR_ITEMC_HIT      = ATTR_COUNT_BASE3 + 14; // hit rate item coefficient bonus
const long	ATTR_ITEMC_CRT      = ATTR_COUNT_BASE3 + 15; // critical rate item coefficient bonus
const long	ATTR_ITEMC_MF       = ATTR_COUNT_BASE3 + 16; // treasure find rate item coefficient bonus
const long	ATTR_ITEMC_HREC     = ATTR_COUNT_BASE3 + 17; // HP regeneration rate item coefficient bonus
const long	ATTR_ITEMC_SREC     = ATTR_COUNT_BASE3 + 18; // SP regeneration rate item coefficient bonus
const long	ATTR_ITEMC_MSPD     = ATTR_COUNT_BASE3 + 19; // movement speed item coefficient bonus
const long	ATTR_ITEMC_COL      = ATTR_COUNT_BASE3 + 20; // resource gathering speed item coefficient bonus
const long	ATTR_ITEMC_PDEF     = ATTR_COUNT_BASE3 + 21; // physical resistance item coefficient bonus

const long	ATTR_COUNT_BASE4    = 96;
const long	ATTR_ITEMV_STR      = ATTR_COUNT_BASE4 + 0; // strength item value bonus
const long	ATTR_ITEMV_AGI      = ATTR_COUNT_BASE4 + 1; // agility item value bonus
const long	ATTR_ITEMV_DEX      = ATTR_COUNT_BASE4 + 2; // focus item value bonus
const long	ATTR_ITEMV_CON      = ATTR_COUNT_BASE4 + 3; // constitution item value bonus
const long	ATTR_ITEMV_STA      = ATTR_COUNT_BASE4 + 4; // vitality item value bonus
const long	ATTR_ITEMV_LUK      = ATTR_COUNT_BASE4 + 5; // luck item value bonus
const long	ATTR_ITEMV_ASPD     = ATTR_COUNT_BASE4 + 6; // attack interval item value bonus
const long	ATTR_ITEMV_ADIS     = ATTR_COUNT_BASE4 + 7; // attack range item value bonus
const long	ATTR_ITEMV_MNATK    = ATTR_COUNT_BASE4 + 8; // minimum attack item value bonus
const long	ATTR_ITEMV_MXATK    = ATTR_COUNT_BASE4 + 9; // maximum attack item value bonus
const long	ATTR_ITEMV_DEF      = ATTR_COUNT_BASE4 + 10; // defence item value bonus
const long	ATTR_ITEMV_MXHP     = ATTR_COUNT_BASE4 + 11; // maximum HP item value bonus
const long	ATTR_ITEMV_MXSP     = ATTR_COUNT_BASE4 + 12; // maximum SP item value bonus
const long	ATTR_ITEMV_FLEE     = ATTR_COUNT_BASE4 + 13; // evasion rate item value bonus
const long	ATTR_ITEMV_HIT      = ATTR_COUNT_BASE4 + 14; // hit rate item value bonus
const long	ATTR_ITEMV_CRT      = ATTR_COUNT_BASE4 + 15; // critical rate item value bonus
const long	ATTR_ITEMV_MF       = ATTR_COUNT_BASE4 + 16; // treasure find rate item value bonus
const long	ATTR_ITEMV_HREC     = ATTR_COUNT_BASE4 + 17; // HP regeneration rate item value bonus
const long	ATTR_ITEMV_SREC     = ATTR_COUNT_BASE4 + 18; // SP regeneration rate item value bonus
const long	ATTR_ITEMV_MSPD     = ATTR_COUNT_BASE4 + 19; // movement speed item value bonus
const long	ATTR_ITEMV_COL      = ATTR_COUNT_BASE4 + 20; // resource gathering speed item value bonus
const long	ATTR_ITEMV_PDEF     = ATTR_COUNT_BASE4 + 21; // physical resistance item value bonus

const long	ATTR_COUNT_BASE5    = 118;
const long	ATTR_STATEC_STR     = ATTR_COUNT_BASE5 + 0; // strength state coefficient bonus
const long	ATTR_STATEC_AGI     = ATTR_COUNT_BASE5 + 1; // agility state coefficient bonus
const long	ATTR_STATEC_DEX     = ATTR_COUNT_BASE5 + 2; // focus state coefficient bonus
const long	ATTR_STATEC_CON     = ATTR_COUNT_BASE5 + 3; // constitution state coefficient bonus
const long	ATTR_STATEC_STA     = ATTR_COUNT_BASE5 + 4; // vitality state coefficient bonus
const long	ATTR_STATEC_LUK     = ATTR_COUNT_BASE5 + 5; // luck state coefficient bonus
const long	ATTR_STATEC_ASPD    = ATTR_COUNT_BASE5 + 6; // attack interval state coefficient bonus
const long	ATTR_STATEC_ADIS    = ATTR_COUNT_BASE5 + 7; // attack range state coefficient bonus
const long	ATTR_STATEC_MNATK   = ATTR_COUNT_BASE5 + 8; // minimum attack state coefficient bonus
const long	ATTR_STATEC_MXATK   = ATTR_COUNT_BASE5 + 9; // maximum attack state coefficient bonus
const long	ATTR_STATEC_DEF     = ATTR_COUNT_BASE5 + 10; // defence state coefficient bonus
const long	ATTR_STATEC_MXHP    = ATTR_COUNT_BASE5 + 11; // maximum HP state coefficient bonus
const long	ATTR_STATEC_MXSP    = ATTR_COUNT_BASE5 + 12; // maximum SP state coefficient bonus
const long	ATTR_STATEC_FLEE    = ATTR_COUNT_BASE5 + 13; // evasion rate state coefficient bonus
const long	ATTR_STATEC_HIT     = ATTR_COUNT_BASE5 + 14; // hit rate state coefficient bonus
const long	ATTR_STATEC_CRT     = ATTR_COUNT_BASE5 + 15; // critical rate state coefficient bonus
const long	ATTR_STATEC_MF      = ATTR_COUNT_BASE5 + 16; // treasure find rate state coefficient bonus
const long	ATTR_STATEC_HREC    = ATTR_COUNT_BASE5 + 17; // HP regeneration rate state coefficient bonus
const long	ATTR_STATEC_SREC    = ATTR_COUNT_BASE5 + 18; // SP regeneration rate state coefficient bonus
const long	ATTR_STATEC_MSPD    = ATTR_COUNT_BASE5 + 19; // movement speed state coefficient bonus
const long	ATTR_STATEC_COL     = ATTR_COUNT_BASE5 + 20; // resource gathering speed state coefficient bonus
const long	ATTR_STATEC_PDEF    = ATTR_COUNT_BASE5 + 21; // physical resistance state coefficient bonus

const long	ATTR_COUNT_BASE6    = 140;
const long	ATTR_STATEV_STR     = ATTR_COUNT_BASE6 + 0; // strength state value bonus
const long	ATTR_STATEV_AGI     = ATTR_COUNT_BASE6 + 1; // agility state value bonus
const long	ATTR_STATEV_DEX     = ATTR_COUNT_BASE6 + 2; // focus state value bonus
const long	ATTR_STATEV_CON     = ATTR_COUNT_BASE6 + 3; // constitution state value bonus
const long	ATTR_STATEV_STA     = ATTR_COUNT_BASE6 + 4; // vitality state value bonus
const long	ATTR_STATEV_LUK     = ATTR_COUNT_BASE6 + 5; // luck state value bonus
const long	ATTR_STATEV_ASPD    = ATTR_COUNT_BASE6 + 6; // attack interval state value bonus
const long	ATTR_STATEV_ADIS    = ATTR_COUNT_BASE6 + 7; // attack range state value bonus
const long	ATTR_STATEV_MNATK   = ATTR_COUNT_BASE6 + 8; // minimum attack state value bonus
const long	ATTR_STATEV_MXATK   = ATTR_COUNT_BASE6 + 9; // maximum attack state value bonus
const long	ATTR_STATEV_DEF     = ATTR_COUNT_BASE6 + 10; // defence state value bonus
const long	ATTR_STATEV_MXHP    = ATTR_COUNT_BASE6 + 11; // maximum HP state value bonus
const long	ATTR_STATEV_MXSP    = ATTR_COUNT_BASE6 + 12; // maximum SP state value bonus
const long	ATTR_STATEV_FLEE    = ATTR_COUNT_BASE6 + 13; // evasion rate state value bonus
const long	ATTR_STATEV_HIT     = ATTR_COUNT_BASE6 + 14; // hit rate state value bonus
const long	ATTR_STATEV_CRT     = ATTR_COUNT_BASE6 + 15; // critical rate state value bonus
const long	ATTR_STATEV_MF      = ATTR_COUNT_BASE6 + 16; // treasure find rate state value bonus
const long	ATTR_STATEV_HREC    = ATTR_COUNT_BASE6 + 17; // HP regeneration rate state value bonus
const long	ATTR_STATEV_SREC    = ATTR_COUNT_BASE6 + 18; // SP regeneration rate state value bonus
const long	ATTR_STATEV_MSPD    = ATTR_COUNT_BASE6 + 19; // movement speed state value bonus
const long	ATTR_STATEV_COL     = ATTR_COUNT_BASE6 + 20; // resource gathering speed state value bonus
const long	ATTR_STATEV_PDEF    = ATTR_COUNT_BASE6 + 21; // physical resistance state value bonus

const long	ATTR_LHAND_ITEMV    = ATTR_COUNT_BASE6 + 22; // left hand item bonus

const long	ATTR_COUNT_BASE7        = 163;
const long	ATTR_BOAT_SHIP          = ATTR_COUNT_BASE7 + 0; // ship id
const long	ATTR_BOAT_HEADER        = ATTR_COUNT_BASE7 + 1; // ship bow type
const long  ATTR_BOAT_BODY          = ATTR_COUNT_BASE7 + 2; // ship hull type
const long  ATTR_BOAT_ENGINE        = ATTR_COUNT_BASE7 + 3; // ship engine type
const long  ATTR_BOAT_CANNON        = ATTR_COUNT_BASE7 + 4; // ship cannon type
const long	ATTR_BOAT_PART          = ATTR_COUNT_BASE7 + 5; // ship accessory type
const long  ATTR_BOAT_DBID          = ATTR_COUNT_BASE7 + 6; // database id of the ship
const long  ATTR_BOAT_DIECOUNT      = ATTR_COUNT_BASE7 + 7; // death count for this ship
const long  ATTR_BOAT_ISDEAD	    = ATTR_COUNT_BASE7 + 8; // whether the ship is currently dead

const long	ATTR_COUNT_BASE8        = 172;
const long	ATTR_BOAT_SKILLC_MNATK  = ATTR_COUNT_BASE8 + 0; // ship MNATK skill coefficient effect
const long	ATTR_BOAT_SKILLC_MXATK  = ATTR_COUNT_BASE8 + 1; // ship MXATK skill coefficient effect
const long	ATTR_BOAT_SKILLC_ADIS   = ATTR_COUNT_BASE8 + 2; // ship attack range (atkrange) skill coefficient effect
const long	ATTR_BOAT_SKILLC_MSPD   = ATTR_COUNT_BASE8 + 3; // ship movement speed skill coefficient effect
const long	ATTR_BOAT_SKILLC_CSPD   = ATTR_COUNT_BASE8 + 4; // shell speed skill coefficient effect
const long	ATTR_BOAT_SKILLC_ASPD   = ATTR_COUNT_BASE8 + 5; // ship ASPD skill coefficient effect
const long	ATTR_BOAT_SKILLC_CRANGE = ATTR_COUNT_BASE8 + 6; // shell blast radius skill coefficient effect
const long	ATTR_BOAT_SKILLC_DEF    = ATTR_COUNT_BASE8 + 7; // ship DEF skill coefficient effect
const long	ATTR_BOAT_SKILLC_RESIST = ATTR_COUNT_BASE8 + 8; // ship RESIST skill coefficient effect
const long	ATTR_BOAT_SKILLC_MXUSE  = ATTR_COUNT_BASE8 + 9; // ship maximum durability skill coefficient effect
const long	ATTR_BOAT_SKILLC_USEREC = ATTR_COUNT_BASE8 + 10; // ship durability regeneration rate skill coefficient effect
const long	ATTR_BOAT_SKILLC_EXP    = ATTR_COUNT_BASE8 + 11; // ship consumption rate skill coefficient effect
const long	ATTR_BOAT_SKILLC_CPT    = ATTR_COUNT_BASE8 + 12; // ship cargo capacity skill coefficient effect
const long	ATTR_BOAT_SKILLC_SPD    = ATTR_COUNT_BASE8 + 13; // sailing speed skill coefficient effect
const long	ATTR_BOAT_SKILLC_MXSPLY = ATTR_COUNT_BASE8 + 14; // maximum supply skill coefficient effect

const long	ATTR_COUNT_BASE9        = 187;
const long	ATTR_BOAT_SKILLV_MNATK  = ATTR_COUNT_BASE9 + 0; // ship MNATK skill constant effect
const long	ATTR_BOAT_SKILLV_MXATK  = ATTR_COUNT_BASE9 + 1; // ship MXATK skill constant effect
const long	ATTR_BOAT_SKILLV_ADIS   = ATTR_COUNT_BASE9 + 2; // ship attack range (atkrange) skill constant effect
const long	ATTR_BOAT_SKILLV_MSPD   = ATTR_COUNT_BASE9 + 3; // ship movement speed skill constant effect
const long	ATTR_BOAT_SKILLV_CSPD   = ATTR_COUNT_BASE9 + 4; // shell speed skill constant effect
const long	ATTR_BOAT_SKILLV_ASPD   = ATTR_COUNT_BASE9 + 5; // ship ASPD skill constant effect
const long	ATTR_BOAT_SKILLV_CRANGE = ATTR_COUNT_BASE9 + 6; // shell blast radius skill constant effect
const long	ATTR_BOAT_SKILLV_DEF    = ATTR_COUNT_BASE9 + 7; // ship DEF skill constant effect
const long	ATTR_BOAT_SKILLV_RESIST = ATTR_COUNT_BASE9 + 8; // ship RESIST skill constant effect
const long	ATTR_BOAT_SKILLV_MXUSE  = ATTR_COUNT_BASE9 + 9; // ship maximum durability skill constant effect
const long	ATTR_BOAT_SKILLV_USEREC = ATTR_COUNT_BASE9 + 10; // ship durability regeneration rate skill constant effect
const long	ATTR_BOAT_SKILLV_EXP    = ATTR_COUNT_BASE9 + 11; // ship consumption rate skill constant effect
const long	ATTR_BOAT_SKILLV_CPT    = ATTR_COUNT_BASE9 + 12; // ship cargo capacity skill constant effect
const long	ATTR_BOAT_SKILLV_SPD    = ATTR_COUNT_BASE9 + 13; // sailing speed skill constant effect
const long	ATTR_BOAT_SKILLV_MXSPLY = ATTR_COUNT_BASE9 + 14; // maximum supply skill constant effect

// Modify by lark.li 20080723
// extended attributes
const long	ATTR_COUNT_BASE10        = 202;
const long	ATTR_EXTEND0  = ATTR_COUNT_BASE10 + 0;
const long	ATTR_EXTEND1  = ATTR_COUNT_BASE10 + 1;
const long	ATTR_EXTEND2  = ATTR_COUNT_BASE10 + 2;
const long	ATTR_EXTEND3  = ATTR_COUNT_BASE10 + 3;
const long	ATTR_EXTEND4  = ATTR_COUNT_BASE10 + 4;
const long	ATTR_EXTEND5  = ATTR_COUNT_BASE10 + 5;
const long	ATTR_EXTEND6  = ATTR_COUNT_BASE10 + 6;
const long	ATTR_EXTEND7  = ATTR_COUNT_BASE10 + 7;
const long	ATTR_EXTEND8  = ATTR_COUNT_BASE10 + 8;
const long	ATTR_EXTEND9  = ATTR_COUNT_BASE10 + 9;

const long	ATTR_MAX_NUM                = 202;
//const long	ATTR_MAX_NUM                = 213;

// End

const long	ATTR_CLIENT_MAX             = ATTR_ITEMC_STR;	
const long  MAX_ATTR_CLIENT				= ATTR_ITEMC_STR;				// highest usable attribute value, client side only
const long	ATTR_CLIENT_SIGN_BYTE_NUM   = (ATTR_CLIENT_MAX + sizeof(char) * 8 - 1) / (sizeof(char) * 8);
const long	ATTR_SIGN_BYTE_NUM          = (ATTR_MAX_NUM + sizeof(char) * 8 - 1) / (sizeof(char) * 8);

inline bool	g_IsNosignChaAttr(long lAttrNo)
{
	if (lAttrNo >= ATTR_CEXP && lAttrNo <= ATTR_NLV_LIFEEXP)
		return true;

	return false;
}

#endif // ATTRTYPE_H