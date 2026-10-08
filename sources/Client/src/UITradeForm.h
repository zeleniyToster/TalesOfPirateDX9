#pragma once
#include "UIGlobalVar.h"
#include "gamecommon.h"

struct NET_CHARTRADE_BOATDATA;
namespace GUI
{
// trading with a player
class CTradeMgr : public CUIInterface
{
public:
	void	ShowCharTradeRequest( BYTE byType, DWORD dwRequestID );				// shows an invitation box
	void	ShowCharTrade( BYTE byType, DWORD dwAcceptID,DWORD dwRequestID );	// shows the trade screen
	void	CancelCharTrade(  DWORD dwCharID );						// cancels the trade
	void	ShowCharTradeMoney(DWORD dwCharID, DWORD dwMoney);   // shows the trade amount
	void	ShowCharTradeIMP(DWORD dwCharID, DWORD dwMoney);   // shows the trade amount

	void	ValidateTradeData( DWORD dwCharID );
	void	ValidateTrade(  DWORD dwCharID );
	void	ShowTradeSuccess(); // shows the trade succeeded
	void	ShowTradeFailed();  // shows the trade failed

	void    CloseAllForm();		// close window event

	// dragged into the trade panel
	void	DragTradeToItem(DWORD dwCharID,BYTE byIndex,BYTE byItemIndex );

	//sItem: the index of the traded item in the item table, byIndex : the index in the trade form, byCount: the number traded
	//byItemIndex: the index of the item in the item slot
	void	DragItemToTrade(DWORD dwCharID,USHORT sItemID,BYTE byIndex,BYTE byCount,BYTE byItemIndex, SItemGrid* pGrid, const NET_CHARTRADE_BOATDATA* const pBoat );

	CGoodsGrid*     GetPlayertradeSaleGrid()    { return  grdSale;		} 
	CGoodsGrid*		GetRequestGrid()			{ return  grdRequest;	} //

	void			LocalSaleItem( CGoodsGrid* pSaleGrid, CGoodsGrid* pSelfGrid, int nGridID, CCommandObj* pItem );  // the traded item
	void			LocalCancelItem( CGoodsGrid* pSaleGrid, CGoodsGrid* pSelfGrid, int nGridID, CCommandObj* pItem );// the cancelled item

	// whether a trade is in progress
	bool			IsTrading();

	CForm*			GetForm()	{ return frmPlayertrade; } //

protected:
	bool Init();
    void End();
    void FrameMove(DWORD dwTime);

	void Clear();

protected:
	static void _evtSelectYesNoEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
    static void _evtThrowItemEvent(CGuiData *pSender,int id,bool& isThrow);     // throws an item out of the item slot

	static void _evtIMPFormEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtGoldFormEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _MainMousePlayerTradeEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtSelfRMouseGridEvent(CGuiData *pSender,CCommandObj* pItem,int nGridID);		// clicking to view your own items on the trade screen
	static void _evtOtherRMouseGridEvent(CGuiData *pSender,CCommandObj* pItem,int nGridID);		// clicking to view the other side's items on the trade screen

private:
	CGoodsGrid* GetPlayertradeBuyGrid()     { return  grdBuy;	}
	static void _evtLocalSaleEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

private:
	CForm*		frmPlayertrade;
	CForm*		frmRequest;				//the item form on the left of the trade (or the cargo hold)
	CGoodsGrid* grdRequest;
	CGoodsGrid*	grdBuy;
	CGoodsGrid*	grdSale;
	CLabelEx*	labOtherGold;
	CLabelEx*	labSelfGold;

	CLabelEx*	labOtherIMP;
	CLabelEx*	labSelfIMP;
	
	CTextButton*	btnTrade;
	CTextButton*	btnYes;
	CTextButton*	btnGold;
	CTextButton*	btnIMP;

	CCheckBox*		chkTrade;
	CCheckBox*		chkYes;

	DWORD			m_dwAcceptID;         // the accepting side of the trade
	DWORD			m_dwRequestID;        // the requesting side of the trade
	BYTE			m_bTradeType;		  // the trade type; see TRADE_CHAR_TYPE in RoleCommand.h

	DWORD			m_dwMainID;

private:
	struct stSale
	{
		stSale() : dwSaleID(0), nGridID(0), nDragID(0) {}

		DWORD   dwSaleID;
		int		nGridID;
		int		nDragID;
	};
	stSale		_sSale;

};

}

