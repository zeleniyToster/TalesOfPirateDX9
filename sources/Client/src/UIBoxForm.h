#pragma once
#include "UIGlobalVar.h"
#include "uicloneform.h"
#include "uiform.h"

namespace GUI
{
struct stTradeBox
{
	stTradeBox();
	void Refresh();
	int GetTradeNum();

	CForm*  frmDialog;
	CLabel*	labPrice;
	CEdit*	edtNumber;
	CLabel* labName;

	// user data
	float	fUnitMoney;
	int		nTotalNum;

};

struct stSelectBox
{
	stSelectBox();

	CForm*  frmDialog;
	CLabel* labInfo;

	// user data
	DWORD	dwTag;
	DWORD	dwParam;
	void*	pointer;
};

struct stNumBox
{
	stNumBox();
	void Refresh();
	int GetNumber();
	void GetString(const char*& out);

	CForm*  frmDialog;
	CEdit*	edtNumber;
	CLabel* labInfo;

	// user data
	int		nTotalNum;
	void*	pointer;
};

struct stPasswordBox
{
	stPasswordBox():frmDialog(NULL),edtPassword(NULL),labInfo(NULL){}
	CForm*  frmDialog;
	CEdit*	edtPassword;
	CLabel* labInfo;
};

struct stMsgBox
{
	CForm*  frmDialog;
	CLabel* labInfo;
	
	// user data
};

struct stMsgTimeBox
{
	CForm*  frmDialog;
	CLabel* labInfo;

	// user data
	short teamID;
};

// some common dialog boxes
class CBoxMgr : public CUIInterface
{
public:
	// the dialog box used for trading
	static stTradeBox*		ShowTradeBox( FormMouseEvent evtForm=NULL, float fUnitMoney=0, int nTotalNum=-1, const char* szName=NULL );

	// a dialog box choosing confirm or cancel
	static stSelectBox*		ShowSelectBox( FormMouseEvent evtForm=NULL, const char* szTitle=NULL, bool bModal=false );

	// quantity input dialog
	static stNumBox*		ShowNumberBox( FormMouseEvent evtForm=NULL, int nTotalNum=-1, const char* szTitle=NULL, bool IsMax=true, bool bModal=true );

	// password input dialog
	static stPasswordBox*	ShowPasswordBox( FormMouseEvent evtForm=NULL, const char* szTitle=NULL, const char* szPassword=NULL, bool bModal=true );

	// shows a dialog
	static stMsgBox*		ShowMsgBox( FormMouseEvent evtForm=NULL, const char* szTitle=NULL, bool bModal=true );

	static stMsgTimeBox*	ShowMsgTime(  FormMouseEvent evtForm=NULL, const char* szTitle=NULL, int iSeconds = 60 );

	static void				CloseAllBox();

protected:
	virtual bool Init();
    virtual void End();
	virtual void FrameMove(DWORD dwTime);

private:
	static void _ClearBox( CHideForm& list );

	CForm*		_FindForm(const char * frmName);

private:
	// trade quantity dialog: quantity, total price
	static CHideForm	_cTrade;	

	// choose yes, or cancel
	static CHideForm	_cSelect;	

	// quantity input dialog
	static CHideForm	_cNumber;

	// password input dialog
	static CHideForm	_cPassword;

	// shows a dialog
	static CHideForm	_cMsg;

	// tick used for timing
	static CForm*	_cfrmError;
	static DWORD    _cTick;
	static int		_cSeconds;
	static bool		_cStart;
	
};

}
