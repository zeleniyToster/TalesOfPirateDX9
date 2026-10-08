//--------------------------------------------------------------
// Name: user interface bank management class
// Design: manages the interface bank
//--------------------------------------------------------------

#ifndef UI_BANK_FORM_H
#define UI_BANK_FORM_H

#include "UIGlobalVar.h"
#include "NetProtocol.h"

namespace GUI
{
	class CForm;
	class CGoodsGrid;
	class CLabel;

	struct stNumBox;
     
	class CBankMgr : public CUIInterface
	{
	public:
		void		ShowBank(); // shows the item slot items
		CGoodsGrid* GetBankGoodsGrid() { return grdBank; } // gets an item slot cell

		bool		PushToBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem); // puts an item into the bank
		bool		PopFromBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem); //drags an item out of the bank
		bool		BankToBank(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem);  //moves an item from one bank to another

	protected:
		virtual bool Init(); // user interface bank information initialisation
		virtual void CloseForm(); //closes the form

	private:
		static void _MoveItemsEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);  // moves several items
		static void _MoveAItemEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);  //moves a single item
		static void _evtBankToBank(CGuiData *pSender,int nFirst, int nSecond, bool& isSwap);  //used to swap items inside the user bank form
		static void _evtOnClose( CForm* pForm, bool& IsClose );//closes the bank form

	private:
		stNumBox*		m_pkNumberBox; // asks for a count
		stNetBank		m_kNetBank;

		// interface
		CForm*			frmBank; // bank form
		CGoodsGrid*		grdBank; // cell form
		CLabel*			labCharName;

	}; // end of class CBankMgr

} // end of namespace GUI

#endif // end of UI_BANK_FORM_H
