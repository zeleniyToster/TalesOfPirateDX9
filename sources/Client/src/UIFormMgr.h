//--------------------------------------------------------------
// Name: form management class
// Author: lh 2004-07-08
// Design: manages every form
// Purpose: show, hide, find forms, switch the form template, provide a global hook callback interface
// Last modified date: 2004-10-09
//--------------------------------------------------------------
#pragma once
#include "uicompent.h"
#include "uiform.h"

namespace GUI
{
class CFormMgr;
typedef bool (*FormMgrEvent) ( CFormMgr* pSender );

typedef bool (*KeyDownEvent) (int& key);
typedef bool (*KeyCharEvent) (char& key);
typedef bool (*MouseEvent) (int& x, int& y, DWORD& mouse);
typedef bool (*MouseScrollEvent) (int& nScroll);
typedef bool (*HotKeyEvent) ( char& key, int& control );	// returning true interrupts


class CFormMgr
{
	friend class CForm;
public:	
	CFormMgr();
	~CFormMgr();

	static void		SetDebugMode(bool v) { _IsDebugMode=v;}		//sets whether to start debug mode (called by a console command) -added by Arcol
	void			ShowDebugInfo();							//shows debug information -added by Arcol
	static void		SetDrawFrameInDebugMode(bool v)	{_IsDrawFrameInDebugMode=v;}	//sets whether window borders are shown in debug mode -added by Arcol
	static void		SetDrawBackGroundInDebugMode(bool v) {_IsDrawBackgroundInDebugMode=v;}	//sets whether the background colour is shown in debug mode -added by Arcol

	bool			AddForm( CForm* form, int templete=0 );

	bool			SetFormTempleteMax(int n);	
	unsigned int	GetFormTempleteMax()			{ return (unsigned int)_showforms.size();	}
	int				GetFormTempletetNum()			{ return _nTempleteNo;						}

	bool			SwitchTemplete( int n );		// switches the current template; if it is -1, switch to the default template

public:
	bool			Init(HWND hWnd);
	void			Clear();

    void            FrameMove( int x, int y, DWORD dwMouseKey, DWORD dwTime );
	void			Render();
	void            RenderHint( int x, int y );
	bool			HandleWindowMsg(DWORD dwMsg, DWORD dwParam1, DWORD dwParam2);

	bool			OnKeyDown(int key);
	bool			OnKeyChar(char key);
	bool			OnHotKey( char key, int control );

	bool			MouseScroll( int nScroll );
	void			Refresh();

    void            MouseReset();

	CForm*			GetHitForm( int x, int y );

	CForm*			Find( const char * );			// looks up a Form in the current template
	CForm*			Find( const char *str, int no );// looks up a Form in the 'no' template
	CForm*			FindAll( const char* );			// looks up a Form among every form

	CForm*			FindESCForm();					// looks up a Form that can be closed with ESC
	int				ModalFormNum()					{ return (int)_modal.size();		}

	typedef void (*FormFun)(CForm* pSender);
	void			ForEach( FormFun pFun );		// every Form runs pFun once

public:
	void			SetEnabled( bool v )			{ _bEnabled = v;	}
	bool			GetEnabled()					{ return _bEnabled; }
    void            ResetAllForm();
    void            SetScreen();

	void			SetEnableHotKey(int flag, bool v);		// revised per the Ximen document
	bool			GetEnableHotKey()				{ return _nEnableHotKey == 0xFFFFFFFF; }// revised per the Ximen document

	static bool		IsMouseInGui()					{ return _eMouseAction==enumMA_Gui;	}
	static eMouseAction	GetMouseAction()			{ return _eMouseAction;	}

public:	// global hook
	bool			AddFormInit( FormMgrEvent pInitFun );	// the event run during initialisation after the script is read

	bool			AddKeyDownEvent( KeyDownEvent event );
	bool			DelKeyDownEvent( KeyDownEvent event );

	bool			AddKeyCharEvent( KeyCharEvent event );
	bool			DelKeyCharEvent( KeyCharEvent event );

	bool			AddMouseEvent( MouseEvent event );
	bool			DelMouseEvent( MouseEvent event );

	bool			AddMouseScrollEvent( MouseScrollEvent event );
	bool			DelMouseScrollEvent( MouseScrollEvent event );

	bool			AddHotKeyEvent( HotKeyEvent event );
	bool			DelHotKeyEvent( HotKeyEvent event );

public:
	static CFormMgr	s_Mgr;

private:
	bool			_AddMemory( CForm* form );
	bool			_DelMemory( CForm* form );

	bool			_MouseRun( int x, int y, DWORD mouse );

	void			_ShowModal( CForm* form );
    void            _ModalClose( CForm* form );

	void			_SetNewActiveForm();		// when the currently active form is lost, a new form has to be found and activated

	void			_InitFormID();

    void            _DelShowForm( CForm* frm );
    void            _AddShowForm( CForm* frm );
    void            _UpdataShowForm( CForm* frm );

    void            _ActiveCompent();

private:
	typedef std::list<CForm*> vfrm;
	vfrm			_allForms;
	vfrm*			_forms;						// the resources for the modeless form display, the first being the currently active form
	vfrm			_modal;						// the modal box being displayed
    vfrm            _show;                      // the modeless box being displayed

	typedef std::vector<vfrm*> frmtemplete;
	frmtemplete		_showforms;
	vfrm			_defaulttemplete;

	bool			_bEnabled;		
	bool			_bInit;
	int				_nEnableHotKey;		// revised per the Ximen document

	typedef std::vector<KeyDownEvent>	vkeydowns;
	vkeydowns		_OnKeyDown;

	typedef std::vector<KeyCharEvent>	vkeychars;
	vkeychars		_OnKeyChar;

	typedef std::vector<MouseEvent>		vmouses;
	vmouses			_OnMouseRun;
	
	typedef std::vector<MouseScrollEvent> vscrolls;
	vscrolls		_OnMouseScroll;

	typedef std::vector<FormMgrEvent>	vinits;
	vinits			_vinits;

	typedef std::vector<HotKeyEvent>		vhotkey;
	vhotkey			_vhotkey;

	int				_nTempleteNo;				// the current template number

    int             _nMouseHover;               // how long the mouse has been idle
    
    CGuiData*       _pHintGui;

	static eMouseAction		_eMouseAction;
	static bool		_IsDebugMode;				// whether it is in debug mode -added by Arcol
	static bool		_IsDrawFrameInDebugMode;	// debug mode only -added by Arcol
	static bool		_IsDrawBackgroundInDebugMode;	// debug mode only -added by Arcol

private:
	void			_DelForm( vfrm& list, CForm* frm );

};

inline void CFormMgr::MouseReset()                    
{ 
    _nMouseHover=0;     
}

}

