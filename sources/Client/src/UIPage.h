//----------------------------------------------------------------------
// Name: paged class
// Author: lh 2004-08-02
// Design: CPageItem is a container for controls and CPage is a container for CPageItem, implementing the switching of CPageItem
// Last modified date: 2004-10-09
//----------------------------------------------------------------------
#pragma once
#include "uicompent.h"
#include "uitextbutton.h"
#include "uiimage.h"

namespace GUI
{
class CPage;

// the page class of the paged class CPage
class CPageItem : public CContainer
{
	friend class CPage;
public:
	enum eTitleState
	{
		tsNormal,
		tsActive,
		tsEnd,
	};

public:
	CPageItem(CForm& frmOwn);
	CPageItem( const CPageItem& rhs );
	CPageItem& operator=(const CPageItem& rhs);
	virtual ~CPageItem(void);
	GUI_CLONE(CPageItem)	

	virtual void	Render();
	virtual void	Refresh();
	virtual bool	MouseRun( int x, int y, DWORD key );
	virtual void	SetAlpha( BYTE alpha );

    virtual CCompent*   GetHintCompent( int x, int y );

public:
	void			RenderTitle()				{ _pTitle->Render();}

	void			SetIndex( unsigned int n )	{ _nIndex = n;		}
	unsigned int	GetIndex()					{ return _nIndex;	}

	CImage*			GetTitle()					{ return _pTitle;	}
	CImage*			GetBkgImage()				{ return _pImage;	}

protected:
	void			_InActive( bool isActive );

private:
	void			_SetSelf();
	static void		_OnTitleDown(CGuiData *pSender, int x, int y, DWORD key);
	void			_OnTitleDown();

protected:
	CImage			*_pTitle;		// operates the title bar of this page; its position is decided by CPage
	CImage			*_pImage;		// the page shows a background image that fills every region
	unsigned int	_nIndex;		// the index inside the Page class

};

// the paged class
class CPage : public CCompent
{
public:
	enum eButtonPos		// all arranged in sequence
	{
		bpLeftUp=0,		// top left
		bpUpFull,		// fills the top
        bpCustom,       // custom
		bpEnd,
	};

public:
	CPage(CForm& frmOwn);
	CPage( const CPage& rhs );
	CPage& operator=(const CPage& rhs);
	virtual ~CPage(void);
	GUI_CLONE(CPage)

	virtual void	Init();
	virtual void	Render();
	virtual void	Refresh();
	virtual bool	MouseRun( int x, int y, DWORD key );
	virtual bool	IsHandleMouse()								{ return true;					}
	virtual void	SetAlpha( BYTE alpha );

    virtual CCompent*       GetHintCompent( int x, int y );
	virtual CGuiPic*		GetImage()							{ return _pImage;				}
    virtual CCompent*       GetHitCommand( int x, int y );

public:
	void			SetIndex( unsigned int n );		// activates a page
	int				GetIndex()									{ return _nIndex;				}
	CPageItem*		GetPage( unsigned int n );

	CPageItem*		NewPage();				// produces a new page

	void			SetButtonSize( int w, int h );
	void			SetButtonPutStyle( eButtonPos style )		{ _eButtonPos=style;			}
	void			SetButtonMargin( int n )					{ if(n>0) _nButtonMargin=n;		}

public:
	GuiEvent		evtSelectPage;			// a new page was selected

private:
	void			_RefreshButtonPos();

protected:
	unsigned int	_nIndex;				// current page
	unsigned int	_nMaxPage;				// current maximum page

	CGuiPic*		_pImage;

	typedef std::vector<CPageItem*>	items;
	items			_pages;

	eButtonPos		_eButtonPos;		// arrangement of the page title buttons
	int				_nButtonWidth, _nButtonHeight;		// page title button width and height
	int				_nButtonMargin;		// page title button margin

};

// inline function
inline void CPageItem::_OnTitleDown(CGuiData *pSender, int x, int y, DWORD key)
{
	((CPageItem*)(pSender->GetParent()))->_OnTitleDown();
}

inline CPageItem* CPage::GetPage( unsigned int n )
{ 
	if( n<_nMaxPage ) return _pages[n];
	return NULL;
}

inline void	CPage::SetButtonSize( int w, int h ) 
{ 
	if( (w > 0) && (h > 0 ) ) _nButtonWidth=w, _nButtonHeight=h; 
}

}
