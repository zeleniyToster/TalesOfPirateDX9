//=============================================================================
// FileName: SkillTemp.h
// Creater: ZhangXuedong
// Date: 2005.03.03
// Comment: Skill Temp Data
//=============================================================================

#ifndef SKILLTEMP_H
#define SKILLTEMP_H

#include "PreAlloc.h"

#define defSKILL_RANGE_BASEP_NUM	3	// number of base area parameters (range centre coordinates, direction)
#define defSKILL_RANGE_EXTEP_NUM	4	// number of area extension parameters (range type, type parameters)
#define defSKILL_STATE_PARAM_NUM	3	// number of area state parameters (number, level, duration)

class CSkillTempData : public dbc::PreAllocStru
{
public:
	CSkillTempData(unsigned long ulSize = 1) : PreAllocStru(1)
	{
		sRange[0] = enumRANGE_TYPE_NONE;
		sStateParam[0] = SSTATE_NONE;
	}

	short	sUseEndure{};	// cost of 'durability'
	short	sUseEnergy{};	// cost of 'energy'
	short	sUseSP{};		// cost of 'SP'

	long	lResumeTime{};// time before it can be used again (milliseconds)
	short	sRange[defSKILL_RANGE_EXTEP_NUM];	// area description (type + parameters)
	short	sStateParam[defSKILL_STATE_PARAM_NUM];	// state number, level, duration

protected:

private:
	void	Initially();
	void	Finally();

};

inline void CSkillTempData::Initially()
{
	sUseEndure = 0;
	sUseEnergy = 0;
	sUseSP = 0;
	lResumeTime = 0;
	sRange[0] = enumRANGE_TYPE_NONE;
	sStateParam[0] = SSTATE_NONE;
}

inline void CSkillTempData::Finally()
{
}

#endif // SKILLTEMP_H
