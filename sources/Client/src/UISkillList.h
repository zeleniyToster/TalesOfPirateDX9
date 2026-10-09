//----------------------------------------------------------------------
// Name: skill box
// Author: lh 2004-12-13
// Purpose: a double click uses the skill; when levelling up a level up button is shown, with the current level and the mana cost
//      it can be dragged onto the shortcut bar and executed with a double click, so there is a Command family
//      it has a dynamic button for levelling up, so it has to respond to the mouse
//      it has static text showing the skill name
//      it has dynamic text showing the current level and the mana cost
//      it has up and down line changes, paging and jumping to the ends, so it has to respond to the keyboard
//   note: this class could use CList directly, deriving CListItems would be enough, but it has more and more complex functionality,
//      and may change later, so a new class is opened to keep the CList family from becoming too complicated
// Last modified date:
//----------------------------------------------------------------------
#pragma once
#include <vector>
#include "uicompent.h"
#include "uiskillcommand.h"
#include "UITextButton.h"
#include "UIScroll.h"

namespace GUI
{

class CSkillList;
typedef void (*GuiSkillListUpgrade) (CSkillList *pSender, CSkillCommand* pSkill);

class CSkillList : public CCompent
{
public:
    CSkillList(CForm& frmOwn);
    CSkillList( const CSkillList& rhs );
    CSkillList& operator=(const CSkillList& rhs);
    virtual ~CSkillList(void);
    GUI_CLONE(CSkillList)

    virtual void	Init();
    virtual void	Render();
    virtual void	Refresh();
    virtual bool	MouseRun( int x, int y, DWORD key );
    virtual bool	MouseScroll( int nScroll );
    virtual void	SetAlpha( BYTE alpha );
    virtual bool	IsHandleMouse()				{ return true;			}
    virtual bool    OnKeyDown( int key );
    virtual void	DragRender();
    virtual CCompent* GetHintCompent( int x, int y );

    virtual void        SetMargin( int left, int top, int right, int bottom );
    virtual CGuiPic*    GetImage()              { return _pImage;       }
    
    CGuiPic*        GetButtonImage()            { return _pButton;      }
    void            Clear();
    void			SetRowHeight( int v )		{ _nRowHeight = v;	    }
    void            SetUnitSize( int w, int h ) { _nUnitHeight=h; _nUnitWidth=w;        }
    void            SetFontLeft( int x )        { _nFontStart = x;      }
    void            SetFontColor( DWORD color ) { _dwFontColor = color; }
    void            SetIsShowUpgrade( bool v )  { _IsShowUpgrade = v;   }

    CSkillCommand*  GetCommand( unsigned int  v );
    bool            AddCommand( CSkillCommand* p );
    CSkillCommand*  FindSkill( int nID );
    bool            DelSkill( int nID );

	int				GetCount()					{ return (int)_skills.size();			}

    CGuiPic*        GetSelect()                 { return _pSelect;      }
    CScroll*        GetScroll()                 { return _pScroll;      }

    int             FindCommand( CSkillCommand* p );

public:
    GuiSkillListUpgrade     evtUpgrade;

private:
    void    _SetSelf();
    void    _Copy( const CSkillList& rhs );
    void    _SetFirstShowRow( DWORD );
	int		_GetHitSkill( int x, int y );

private:
    static void			_ScrollChange(CGuiData *pSender) {
        ((CSkillList*)(pSender->GetParent()))->_OnScrollChange();
    }
    void				_OnScrollChange();
    void	_ResetPageNum();

private:
    static	void	_DragEnd(CGuiData *pSender, int x, int y, DWORD key)	{
        ((CSkillList*)(pSender))->_DragEnd( x, y, key );
    }
    void			_DragEnd( int x, int y, DWORD key );

protected:
    CGuiPic*		_pImage;		// background image
    CGuiPic*		_pSelect;	    // selection bar
    CGuiPic*        _pButton;       // level up button
    CScroll*		_pScroll;	    

    typedef std::vector<CSkillCommand*> skills;
    skills          _skills;

    // distance to the surrounding border
    int				_nLeftMargin;	
    int				_nTopMargin;	
    int				_nRightMargin;	
    int				_nBottomMargin;	

    int				_nRowHeight;    // row height
    int             _nFontStart;    // where the text starts
    DWORD           _dwFontColor;

    int             _nUnitHeight, _nUnitWidth;   // cell image width and height

    int             _nSelectIndex;  // the selected row number
    bool            _IsShowUpgrade;

private:
    int             _nShowFirst;    // the first row displayed
    int             _nShowLast;     // the last row displayed
    int				_nShowCount;    // how many rows can be displayed in total

    // the range used to display skills
    int             _nSX1, _nSY1, _nSX2, _nSY2;   

    int             _nButtonX1;     // X position where the button is displayed
    int             _nButtonOffY;   // Y offset where the button is displayed

    int             _nRowSpace;     // row spacing
    int             _nFontYOff;     // the height of the text in the middle of a row

private:
    int             _nDragIndex;
    int             _nDragOffX, _nDragOffY;
    int             _nMouseUpgradeID;

};


// inline function
inline 	void CSkillList::_ResetPageNum() 
{ 
    _pScroll->SetPageNum( ( GetHeight() - _nTopMargin - _nBottomMargin ) / _nRowHeight - 1 );
}

inline CSkillCommand* CSkillList::GetCommand( unsigned int  v )
{
    if( v>=_skills.size() ) return NULL;
    return _skills[v];
}

inline int CSkillList::_GetHitSkill( int x, int y )
{
    if( x>=_nSX1 && x<=_nSX2 && y>=_nSY1 && y<=_nSY2 )
    {
        int h = ( _nShowCount * _nRowHeight + _nSY1 );
        if( y >= h ) return false;

        // gets the number of rows
        int row = ( y - _nSY1 ) / _nRowHeight + _nShowFirst;
        if( row>=0 && row<(int)_skills.size() ) 
			return row;
	}
	return -1;
}

}
