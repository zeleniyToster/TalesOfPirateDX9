#pragma once

namespace GUI
{
class CCursor 
{
public:
	enum eState
	{
		stNormal	= 0,	// normal state
		stActive	= 1,	// state with an action
		stDrag		= 2,	// while dragging
		stSize		= 3,	// resizing
		stVertical	= 4,	// vertical
		stLevel		= 5,	// horizontal
		stWait		= 6,	// waiting
		stAttack	= 7,	// the attack cursor shown while the mouse moves
		stSkillAttack = 8,	// single skill attack
		stUpBank	= 9,	// going ashore
		stUpBoat	= 10,	// boarding a boat
		stStop		= 11,	// not operable
		stHover		= 12,	// hovering an item
		stPick		= 13,	// picking up an item
        stCamera	= 14,   // rotating the camera
        stChat		= 15,   // dialog
        stMouse		= 16,   // a mouse
        stButtonClick = 17, // button pressed
        stHide		= 18,   // cursor not shown
		stSearch	= 19,	// query
		stBlock		= 20,	// not walkable
		stRepair	= 21,	// repair
		stFeed		= 22,	// feeding
		stEnd,				// terminator, used to find the maximum value of the array
	};

public:
	CCursor();
	~CCursor();

	void	Init();
	void	SetCursor( eState v );			// sets the cursor state - state function
	eState  GetCursor()			{ return _eActive;				}

	bool	SetFrame( eState v );			// sets the cursor state - per frame call function
    void    Restore()           { _IsShowFrame=false;           }

	int		GetMax()			{ return stEnd;					}
	int		GetIndex()			{ return _eState;				}

    void    Render();

public:
	static	CCursor* I()		{ return &s_Cursor;				}

private:
	void	_ShowCursor();

private:
	static CCursor	s_Cursor;

	bool		_IsInit;
    bool        _IsShowFrame;

	eState		_eFrame;					// the cursor state of the frame function
	eState		_eState;					// the cursor state of the state function

	eState		_eActive;					// the state currently in use
	HCURSOR		_hCursor[stEnd];

};

class CWaitCursor
{
public:
	CWaitCursor()					{ CCursor::I()->SetCursor( CCursor::stWait );	}
	CWaitCursor(CCursor::eState c)	{ CCursor::I()->SetCursor( c );					}
	~CWaitCursor()					{ CCursor::I()->SetCursor( CCursor::stNormal );	}
};

// inline function
inline void CCursor::SetCursor( eState v )
{
    if( v>=stNormal && v<stEnd )
    {
        _eState = v;
    }
}

inline bool CCursor::SetFrame( eState v )
{
    if( v>=stNormal && v<stEnd )
    {
        _IsShowFrame = true;
        _eFrame = v;
        return true;
    }
    return false;
}

inline void CCursor::Render()
{
    if( _IsShowFrame )
    {
        if( _eActive!=_eFrame )  
        {
            _eActive = _eFrame;
            _ShowCursor();
        }
    }
    else if( _eActive!=_eState )  
    {
        _eActive = _eState;
        _ShowCursor();
    }
}

}
