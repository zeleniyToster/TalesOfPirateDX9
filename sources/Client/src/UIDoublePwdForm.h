
#pragma once


#include "UIFormMgr.h"
#include "UIForm.h"
#include "UIEdit.h"
#include "UILabel.h"
#include "UITextButton.h"

#include "UIGlobalVar.h"
#include "NetProtocol.h"

#include "LoginScene.h"
#include "SelectChaScene.h"


namespace GUI
{
	class CForm;
	class CEdit;
	class CTextButton;

	// secondary password screen
	class CDoublePwdMgr : public CUIInterface	
	{
	public:

		CDoublePwdMgr();

		// shows the creation form
		void ShowCreateForm();

		// shows the modification form
		void ShowAlterForm();

		// shows the input form
		void ShowDoublePwdForm();

		// closes every form
		void CloseAllForm();

		// gets whether the form is already shown
		bool GetIsShowCreateForm()    { return frmDoublePwdCreate->GetIsShow(); }
		bool GetIsShowAlterForm()     { return frmDoublePwdAlter->GetIsShow();  }
		bool GetIsShowDoublePwdForm() { return frmDoublePwd->GetIsShow();       }

		// sets what runs after the secondary password is entered
		void SetType(int nType) { m_nType = nType; }
		int  GetType()          { return  m_nType; }

		void SetLockGridID(int id) { lockGridID = id; }

		static const int MC_REQUEST	= -1;
		static const int DELETE_CHARACTOR	= 1;	// deletes the current character
		static const int PACKAGE_UNLOCK		= 2;	// bag unlock
		static const int STORE_OPEN_ASK		= 3;	// shop open
		static const int ITEM_UNLOCK		= 4;	// item unlock  ning.yan 2008-11-10
		static const int MULTI_ITEM_UNLOCK = 5;
		static const int SHOW_EXCHANGEFORM	= 5;

	protected:

		virtual bool Init();
		virtual void CloseForm();

		// shows the secondary password soft keyboard
		void ShowDoublePwdKeyboardForm();

		// sets the focused password edit box
		void SetFocusEditBox(CEdit* edt) { edtFocusEditBox = edt; }

		// sends the delete character message
		void SendDeleteCharactor();

		// sends the bag unlock message
		void SendPackageUnlock();

		// sends the item unlock message
		void SendItemUnlock(); //add by ning.yan 2008-11-11

		void SendUnlockSelectedItems();
		// sends the shop open message
		void SendPackageStoreOpen();

		void SendGameRequest();

		// shuffles the soft keyboard buttons randomly
		void RandomInputButton();

	private:

		// create secondary password form
		CForm*      frmDoublePwdCreate;
		CEdit*		edtDoublePwdCreate;
		CEdit*      edtDoublePwdCreateRetry;

		// modify secondary password form
		CForm*      frmDoublePwdAlter;
		CEdit*		edtDoublePwdAlterOld;
		CEdit*		edtDoublePwdAlterNew;
		CEdit*		edtDoublePwdAlterNewRetry;

		// input secondary password form
		CForm*      frmDoublePwd;
		CEdit*      edtDoublePwd;

		// secondary password input soft keyboard form
		CForm*      frmDoublePwdInput;

		// the prompt shown when creating a secondary password
		CForm*		frmDoublePwdInfo;

		// focused password edit box
		CEdit*		edtFocusEditBox;

		// type
		int         m_nType;

		int lockGridID; //TODO: No default value

	private:

		// creation secondary password form button event
		static void _evtCreateFromMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

		// modification secondary password form button event
		static void _evtAlterFromMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

		// input secondary password form button event
		static void _evtDoublePwdFromMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

		// secondary password input soft keyboard form button event
		static void _evtInputFromMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

		// edit box activation event (saves the activated edit box)
		static void _evtEditFocus(CGuiData* pSender);

		// close window event
		static void _evtFormClose(CForm* pForm, bool& IsClose );

		// whether the secondary password is valid
		static bool IsPwdValid(const char* szStr);

	}; // end of class CDoublePwdMgr

} // end of namespace GUI


