//----------------------------------------------------------------------
// Name: used to run shortcut commands
// Author: lh 2004-10-28
//----------------------------------------------------------------------
#pragma once

#include "uicompent.h"

namespace GUI
{
#define defCommandDefaultIndex	-1

class CCommandObj : public CItemObj
{
public:
	CCommandObj() : _nFast(0), nTag(0), _pParent(NULL) ,_bTrade(false), _nIndex(defCommandDefaultIndex) {}
	virtual ~CCommandObj();
    ITEM_CLONE(CCommandObj)

    bool	        Exec();         // executed when the user selects it
	bool	        ExecRightClick();         // executed when the user selects it

	virtual bool	MouseDown(){ return false;	} //mouse button pressed
	bool	GetCanDrag();
	virtual	void	SaleRender( int x, int y, int nWidth, int nHeight )	{}
	virtual void	OwnDefRender( int x, int y, int nWidth, int nHeight ) {}
	virtual void	RenderEnergy(int x, int y)	{}

    void            SetParent( CCompent* p )    { _pParent=p;   }
    CCompent*       GetParent()     { return _pParent;          }
    
    static  bool    UserExec();     // after the user selects it, wait for the user to type what to run
    static  void    Cancel()        { _pCommand=NULL;           }
	static  CCommandObj* GetReadyCommand()	{ return  _pCommand;}

    virtual bool    UseCommand(bool value = false);   // use
	virtual bool	StartCommand()	{ return false;	}

    virtual int     GetTotalNum()   { return 1;     }
    virtual void    SetTotalNum( int num ){}

    virtual bool    IsDragFast()    { return true;  }   // whether it can be dragged into a shortcut

    virtual bool    IsAllowThrow()  { return false; }   // whether dropping is allowed
    virtual bool    IsAllowEquip()  { return false; }   // equippable

	virtual void    SetIsSolid( bool v ) {}
	virtual bool    GetIsSolid()    { return true;  }

	virtual void    SetIsValid( bool v ){}
	virtual bool    GetIsValid()	{ return false; }

    virtual bool    GetIsPile()     { return false; }
    virtual int     GetPrice()      { return 0;     }

    virtual const char* GetName()   { return "unknown";    }
    virtual bool    IsAllowUse()    { return true;  }   // can be used

    // hint
    virtual bool    HasHint()       { return true;  }
	virtual void    ReadyForHint( int x, int y, CCompent* pCompent );
    virtual void    RenderHint( int x, int y );

	// sets a custom string
	void			SetOwnDefText(const char* pszText) { _OwnDefText = pszText; }
	const char*		GetOwnDefText()	{ return _OwnDefText.c_str(); }

	static CTextHint& GetHints()	{ return  _hints;}

public:
    void		SetIsFast( bool v );                    // whether it is a shortcut
    bool        GetIsFast()             { return _nFast>0;  }

    void        SetIndex( int nIndex )  { _nIndex = nIndex; }
    int         GetIndex()              { return _nIndex;   }

	int			nTag;       // used to record the ScriptID of an Item or Skill

protected:
    virtual bool    IsAtOnce()      { return true;  }   // can be used directly
    virtual bool    ReadyUse()      { return false; }   // about to be used; corresponds to the case where it cannot be used at once
    virtual void    Error(){}

    virtual void    AddHint( int x, int y ) {}

protected:
    static CCommandObj*     _pCommand;      // the skill the user is currently operating

private:
    bool    _Exec();

private:
	int	        _nFast;	    // the count on the shortcut bar; if it has already been put on a shortcut bar, a notification is sent on exit
    CCompent*   _pParent;   // the control it belongs to
	bool        _bTrade;
    int         _nIndex;

protected:
    void    PushHint( const char* str, DWORD color=COLOR_WHITE, int height=5, int font=0, int index = -1, bool shadow = false, DWORD scolor = 0); // height: the height with the character height deducted
    void    AddHintHeight( int height=6 );
	void    SetHintIsCenter( bool v );

	static CTextHint _hints;

	std::string	_OwnDefText;

};

// inline function
inline void CCommandObj::PushHint( const char* str, DWORD color, int height, int font, int index, bool shadow, DWORD scolor )
{ 
	_hints.PushHint( str, color, height, font, index, shadow, scolor );
}

inline void CCommandObj::AddHintHeight( int height )
{
	_hints.AddHintHeight( height );
}

inline void CCommandObj::SetHintIsCenter( bool v )   
{ 
	_hints.SetHintIsCenter( v );    
}

}
