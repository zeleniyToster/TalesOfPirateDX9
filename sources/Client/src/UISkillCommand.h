//----------------------------------------------------------------------
// Name: skill
// Author: lh 2004-11-07
// Last modified date:
//----------------------------------------------------------------------
#pragma once
#include "UICommand.h"
#include "SkillRecord.h"

class CAniClock;
class CSkillRecord;
class CCharacter;

namespace GUI
{

class CSkillCommand : public CCommandObj
{
public:
    CSkillCommand( CSkillRecord * pSkill );
    CSkillCommand( const CSkillCommand& rhs );
    CSkillCommand& operator=( const CSkillCommand& rhs );
    ~CSkillCommand();
    ITEM_CLONE(CSkillCommand)

    virtual void    Render( int x, int y );
    virtual bool    UseCommand(bool value = false);
	virtual bool	StartCommand();
    virtual bool    IsAllowUse();

    const char*     GetName();
	const char*		GetSkillName();
    int             GetSkillID()        { return _pSkill->sID;  }
    CSkillRecord*   GetSkillRecord()    { return _pSkill;       }

	static CGuiPic*	GetActiveImage()	{ return &_imgActive;	}

	enum eSpecialType
	{
		enumHighLight  = 1,	// always shown highlighted
		enumNotUpgrade = 2	// manual levelling up is not allowed
	};

	bool			GetIsSpecial(eSpecialType SpecialType);

protected:
    virtual bool    IsAtOnce();
    virtual bool    ReadyUse();
    virtual void    Error();
    virtual void    AddHint( int x, int y );

protected:
    int     _GetSkillTime();
	bool	_WriteNeed( int nType, int nValue, const char* szStr );

protected:
    CGuiPic*	    _pImage;
    CAniClock*      _pAniClock;
    CSkillRecord*   _pSkill;    

	DWORD			_dwPlayTime;
    DWORD           _dwRecordTime;

    static CGuiPic  _imgActive;		// shows the active border

};

// inline function
inline int CSkillCommand::_GetSkillTime()  
{ 
    return _pSkill->GetFireSpeed();
}

}
