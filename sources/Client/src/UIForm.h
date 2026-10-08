//--------------------------------------------------------------
// Name: form class
// Author: lh 2004-07-08
// Design: a container for controls
// Purpose: hot key show and hide, dragging
// Last modified date: 2004-10-09
//--------------------------------------------------------------
#pragma once
#include "uiguidata.h"

namespace GUI
{

typedef void (*GuiFormCloseEvent)( CForm* pForm, bool& IsClose );
typedef void (*GuiFormBeforeShowEvent)( CForm* pForm, bool& IsShow );
typedef void (*GuiFormEscCloseEvent)( CForm* pForm );

// callback for button control events inside the form;
// pSender is the button, nMsgType is eModalResult
typedef void (*FormMouseEvent) (CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);


class CTextButton;
class CMenu;
class CForm : public CGuiData
{
	friend class CCompent;
	friend class CContainer;
	friend class CFormMgr;
	
public:
	enum eModalResult 
	{
		mrNone,
		mrClose,
		mrYes,
		mrNo,
		mrOk,
		mrCancel,
        mrEnd,
	};
	enum eFormStyle  //form style: ordinary, fully centred, centred left and right, centred top and bottom, left, right, top, bottom
	{
		fsNone,
		fsAllCenter,
		fsXCenter,
		fsYCenter,
		fsLeft,
		fsRight,
		fsTop,
		fsBottom,
		fsLeftTop,
		fsRightTop,
		fsLeftBottom,
		fsRightBottom,

	} ;

	CForm();
	CForm( const CForm& rhs );
	CForm& operator=( const CForm& rhs );
	~CForm();
	GUI_CLONE(CForm)

	virtual void		Init();
	virtual void		Render();
	virtual void		Refresh();

	virtual bool		MouseRun( int x, int y, DWORD key );
    virtual void		SetIsShow( bool v )			{ if( v ) Show(); else Close();  }
	virtual void		SetParent( CGuiData* p );
	virtual void		Reset();

	bool				MenuMouseRun( int x, int y, DWORD key );

public:
	void				ForEach( CompentFun pFun );		// every Compent in the Form runs pFun once
    void                FrameMove(int nCount);

public:
	void				Show();
    void                ShowModal();
	void				Hide();
	void				Close();
	CCompent*			GetHitCommand( int x, int y );
    CGuiData*           GetHintGui( int x, int y );
	void				SetStyle(eFormStyle index, int offWidth=0, int offHeight=0, bool bRedraw=false);
	int                 GetFormStyle()        { return  _formStyle;			}

	void				SetIsEscClose( bool v )	{ _IsEscClose = v;			}
	bool				GetIsEscClose()			{ return _IsEscClose;		}

	virtual CCompent*	Find( const char* );
	virtual void		SetAlpha( BYTE alpha );
	virtual void		SetIsDrag( bool v );

	bool				GetIsModal()		{ return _isModal;		}
	eModalResult		GetModalResult()	{ return _modalResult;	}
    void                SetModalResult( eModalResult v ) { _modalResult = v;        }

	static CForm*		GetActive()				{ return _pActive;					}
	CFramePic*			GetFrameImage()		{ return _pImage;		}

	void				SetHotKey(char key) { _cHotKey=key;			}
	char				GetHotKey()			{ return _cHotKey;		}
	void				SetHotKeyHandler(GuiHotKeyEvent handler)	{ evtHotkeyHandler=handler; }

    bool                SetNextActiveCompent( bool isNext=true );       // activates the next control in the form; isNext=false gives the previous control

	void				SetActiveCompent( CCompent* pActive );

	// gets the current menu
	void				PopMenu( CMenu* pMenu, int x=0, int y=0 );
	
    void                SetPointer( void* v )   { _pPointer = v;    }
    void*               GetPointer()        { return _pPointer;     }

	void				SetEnterButton( CTextButton* v )	{ _pEnterButton = v;	}

public:
	bool				OnChar( char key );
	bool				OnKeyDown( int key );
	void                OnSetScreen();	

    CCompent*           FindActiveCompent();

	int					ClearChild();	// breaks all relations with child windows; returns the number broken

public:		// events
	GuiEvent			evtShow;		// on show
	GuiEvent			evtHide;		// on hide
	GuiEvent			evtActive;		// on gaining focus
	GuiEvent			evtLost;		// on losing focus
	GuiMouseEvent		evtMouseDown;	// when a mouse button is pressed over the window
	GuiKeyDownEvent		evtKeyDown;
	GuiKeyCharEvent		evtKeyChar;
    GuiEvent            evtOnSetScreen;
	GuiMouseEvent		evtMouseDragEnd;			// dragging ends
	GuiHotKeyEvent		evtHotkeyHandler;	//hot key handling; returning true means it was handled, false means it was not

	GuiFormEscCloseEvent	evtEscClose;
	GuiFormBeforeShowEvent	evtBeforeShow;
	GuiFormCloseEvent		evtClose;		// called when closing

public:		// used for control callback events
	FormMouseEvent			evtEntrustMouseEvent;

protected:
	void				_OnActive();
	bool				_AddCompent(CCompent* c, bool isCheck);
	bool				_AddFrameMove(CCompent* c, bool isCheck);

	void				_TempleteInit();	// initialisation before entering the template
	void				_TempleteClear();	// cleanup after leaving the template

protected:
    void*               _pPointer;

	typedef std::vector<CCompent*> vcs;	
	vcs					_compents;			// the controls inside the form, also the order they are displayed in
	vcs					_mouse;				// handles the control's mouse
	vcs					_actives;			// the activatable list
	vcs					_allCompents;		// every control
	vcs					_frames;

	typedef std::list<CForm*> vfrm;
	vfrm				_childs;			// child forms

	int					_nActiveCompentID;	// index of the currently active Compent

	bool			    _isModal;			// whether it is shown modally
	
	static	CForm*		_pActive;

    eModalResult		_modalResult;		// window modal
	eFormStyle          _formStyle;

	bool				_IsEscClose;		// if true, pressing esc closes the form

	CFramePic*			_pImage;			// background image, two frames, one normal and one active
	CTextButton*		_pEnterButton;

protected:
	char				_cHotKey;			// the active hot key
	CMenu*				_pPopMenu;			// pops up the menu

private:
	void				_Copy( const CForm* rhs );
	void				_CopyCompent( const CForm* rhs );
    void                _ActiveForm( DWORD key );

private:
	static void			_DragMouseEvent(CGuiData *pSender, int x, int y, DWORD key);
	static void			_DragMouseEventBegin(CGuiData *pSender, int x, int y, DWORD key);
	static int			_nDragOffX,  _nDragOffY;
	int					_nOffWidth;			//-Added by Arcol
	int					_nOffHeight;		//-Added by Arcol
};

inline void CForm::_ActiveForm(DWORD key)
{
    if( (key & Mouse_Down) && !GetIsModal() && _pActive!=this ) 
    {
        _OnActive();
    }
}

inline void	CForm::Close()				
{ 
	_modalResult = mrClose;
    Hide();				
}

}
