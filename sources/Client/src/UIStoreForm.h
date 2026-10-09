
#pragma once

#include "uiglobalvar.h"
#include "uiguidata.h"
#include "uiform.h"
#include "uilabel.h"
#include "uimemo.h"
#include "uiboxform.h"
#include "uifastcommand.h"
#include "uitreeview.h"
#include "uiprogressbar.h"
#include "UI3DCompent.h"
#include "NetProtocol.h"

#include <map>


namespace GUI
{

	// information about a single item inside a single good
	struct stItemInfo
	{
		short       itemID;         //	item number
		int         itemNum;        //	item count
		int         itemFlute;      //	slot count
		short       itemAttrID[5];  //	five attribute numbers
		short       itemAttrVal[5]; //	five attribute values

		void Clear() { memset(this, 0, sizeof(stItemInfo)); }
	};

	// information about a single good
	struct stStoreInfo
	{
		long        comID;          //	goods number
		char        comName[21];    //	goods name
		long        comClass;       //	goods category
		long        comTime;        //	goods listing time
		long        comPrice;       //	price
		char        comRemark[129]; //	goods remark
		int         comNumber;      //  remaining number of the goods. -1 unlimited, 0 delisted
		time_t      comExpire;      //  goods expiry time.
		int         itemNum;        //  item count
		int         isHot;          //  whether it is on hot sale
		stItemInfo  comItemInfo[6]; //  at most six items

		stStoreInfo() { memset(this, 0, sizeof(stStoreInfo)); }
		void Clear()  { memset(this, 0, sizeof(stStoreInfo)); }
	};

	//  the GUI for a single good
	struct stStoreGui
	{
		CLabelEx*		labName;			// goods name
		CLabelEx*		labPrice;			// goods price
		CLabelEx*		labLeftTime;		// goods remaining time
		CLabelEx*		labLeftNum;			// goods remaining count
		CLabelEx*		labRemark;			// goods description
		CLabelEx*		labRightClickView;	// right click to view the details
		CImage*			imgCutLine;			// divider line
		CImage*			imgHot;				// hot sale icon
		CImage*			imgNew;				// new arrival icon
		CImage*			imgBlue;			// highlight of the selected good
		CImage*			imgSquare;			// item cell
		COneCommand*	cmdStore;			// goods grid
		CTextButton*	btnBlue;			// highlight button
		
		CImage*			imgWhite;

		void SetIsShow(bool v)
		{
			labName->SetIsShow(v);
			labPrice->SetIsShow(v);
			labRemark->SetIsShow(v);
			labLeftTime->SetIsShow(v);
			labLeftNum->SetIsShow(v);
			labRightClickView->SetIsShow(v);
			imgCutLine->SetIsShow(v);
			imgHot->SetIsShow(v);
			imgNew->SetIsShow(v);
			imgBlue->SetIsShow(v);
			imgSquare->SetIsShow(v);
			cmdStore->SetIsShow(v);
			btnBlue->SetIsShow(v);

			if(!v) cmdStore->DelCommand();
		}
	};


	class CStoreMgr : public CUIInterface
	{
	public:
		CStoreMgr();

		void ShowTempKitbag(bool bShow = true);
		void ShowStoreForm(bool bShow = true);
		void ShowStoreLoad(bool bShow = true);
		void ShowViewAllForm(bool bShow = true);
		void ShowTryonForm(bool bShow = true);
		void ShowAlphaMatteForm(bool bShow = true);
		void ShowStoreWebPage();

		CGoodsGrid* GetTempKitbagGrid()		{  return grdTempBag;	}
		CForm*		GetStoreForm()			{  return frmStore;		}
		static int	GetPageSize()			{  return STORE_PAGE_SIZE;  }

		// the scene turns black
		void DarkScene(bool bDark);

		// takes it out of the temporary bag
		bool PopFromTempKitbag(CGoodsGrid& rkDrag, CGoodsGrid& rkSelf, int nGridID, CCommandObj& rkItem);
		
		// the query for opening the shop
		void OpenStoreAsk();

		// adds a shop tree node (a parent node of zero means the root node)
		void AddStoreTreeNode(long nParentID, long nID, const char* szCaption);

		// adds shop goods information
		void AddStoreItemInfo(long nSeq, long nID, const char* szName, long nPrice, const char* szRemark, bool isHot, long nTime, long nRemainNum, long nRemainTime);

		// adds shop goods detail information
		void AddStoreItemDetail(long nSeq, long nSubSeq, short sItemID, short sItemNum, short sFlute, short pItemAttrID[], short pItemAttrVal[]);

		// adds a user settings node
		void AddStoreUserTreeNode(void);

		// sets the page of the shop list
		void SetStoreItemPage(long nCurPage, long nMaxPage);

		// sets the MoDou count
		void SetStoreMoney(long nMoBean, long nRplMoney);

		// sets the VIP value
		void SetStoreVip(long nVip);

		// adds a shop announcement
		//void AddStoreCommunique(long nID, const char* szTitle, 

		// refreshes the shop tree
		void StoreTreeRefresh();

		// clears every shop tree node
		void ClearStoreTreeNode();

		// clears every shop item list
		void ClearStoreItemList();

		// sets whether the shop buttons are usable
		void SetStoreBuyButtonEnable(bool b);

		// gets the currently selected ID
		int GetCurSelItemID();

		// resets the time of the last operation; handled at 10 second intervals
		bool ResetLastOperate(bool bSilent = false);

		// user information node
		static const long USER_NODEID = 0x7FFFFFFE;
		static const long HELP_NODEID = 0x7FFFFFFF;

	protected:
		virtual bool Init();
		virtual void CloseForm();
		virtual void SwitchMap();
		virtual void FrameMove(DWORD dwTime);

	private:

		// temporary bag
		CForm*			frmTempBag;			// temporary bag screen
		CGoodsGrid*		grdTempBag;			// temporary bag grid
		
		
		stNumBox*		m_pkNumberBox;		// asks for a count
		stNetTempKitbag m_NetTempKitbag;	// temporary bag network protocol data

		static void _evtDragItemsEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void _MoveItemsEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	private:
		// shop
		CForm*			frmStore;			// shop screen
		CTreeView*		trvStore;			// tree list
		CLabelEx*		labMoneyLeft;		// money remaining
		CLabelEx*		labBeanLeft;		// tokens remaining
		CLabelEx*		labMemberStyle;		// membership type

		CLabelEx*		labNameTitle;
		CLabelEx*		labPriceTitle;
		CLabelEx*		labLeftTimeTitle;
		CLabelEx*		labLeftNumTitle;

		CLabelEx*		labListPage;		// page tab
		CTextButton*	btnLeftPage;		// left button
		CTextButton*	btnRightPage;		// right button
		CTextButton*	btnTrade;			// trade button
		CTextButton*	btnTryon;			// try on button
		CTextButton*	btnViewAll;			// view all goods button
		CLabelEx*		labTrade;			// trade button text
		CLabelEx*		labTryon;			// try on button text
		CLabelEx*		labViewAll;			// view all goods button text
		CMemo*			memStoreHelp;		// personal information help
		CImage*			imgBackGround10;	// white panel (shown while buying goods, hidden when custom information is shown)
		stNumBox*		m_pkExchangeNum;	// token exchange count input box

		static const int STORE_PAGE_SIZE = 9;		  // a goods list page holds at most 9 entries
		stStoreGui		m_stStoreGui [STORE_PAGE_SIZE];
		stStoreInfo		m_stStoreInfo[STORE_PAGE_SIZE];

		long			m_nCurClass;		// current type
		long			m_nCurSel;			// current selection
		long			m_nCurPage;			// current page
		long			m_nMaxPage;			// maximum page
		long			m_nExchangeNum;		// exchange count
		long			m_nVip;				// whether it is a VIP


		typedef std::map<long, std::string> MapNode;
		typedef std::map<long, std::string>::iterator MapNodeIter;
		MapNode			m_mapNode;		// tree node

		void _SetIsShowUserInfo(bool bShow);
		void _SetIsShowHelpInfo(bool bShow);
		void _SetIsShowCozeForm(bool bShow);
		void _RefreshStoreListHighLight();
		void _ShowTradeSelectBox();
		void _LoadStoreHelpInfo();
		void _ChangeChaPart(stNetLookInfo& SLookInfo);
		bool _IsCurSelVipNode(void);

		static void	_evtStoreTreeMouseClick(CGuiData *pSender, int x, int y, DWORD key);
		static void	_evtStoreOpenCheckEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void	_evtStoreToVipEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void _evtExchangeEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void	_evtExchangeCheckEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void	_evtTradeCheckEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void _evtStoreFormClose(CForm* pForm, bool& IsClose);

		static void _evtStoreFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void	_evtStoreListMouseRClick(CGuiData *pSender, int x, int y, DWORD key);
		static void	_evtStoreListMouseDBClick(CGuiData *pSender, int x, int y, DWORD key);

	private:

		// shop reading screen
		CForm*			frmStoreLoad;
		CProgressBar*	proStoreLoad;
		static void _evtStoreLoadFormClose(CForm* pForm, bool& IsClose);
		static void _evtProTimeArriveEvt(CGuiData *pSender);
		DWORD			_dwDarkScene;

		static const int STORE_OPEN_TIMEOUT = 15;	// shop open timeout (unit: seconds)

	private:

		// view the details
		static const int STORE_ITEM_COUNT = 6;	// a good holds at most six items
		CForm*			frmViewAll;
		CMemo*			memViewAll;
		CImage*			imgSquareViewAll[STORE_ITEM_COUNT];
		COneCommand*	cmdSquareIcon[STORE_ITEM_COUNT];

		static void _evtStoreViewAllMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void _evtStoreViewAllLostEvent(CGuiData *pSender);

	private:

		// try on (Avata)
		CForm*			frmTryon;		// try on screen
		C3DCompent*		ui3dplayer;		// 3D try on

		int				m_nChaRotate;	// character rotation
		CCharacter*		m_pCurrMainCha;
		bool			m_isFight;
		stNetLookInfo	m_sLookInfo;
		stNetLookInfo	m_sCurLookInfo;

		void			ChaEquipClearAll();
		void			ChaLeftRotate();
		void			ChaRightRotate();
		void			RenderChaTryon(int x, int y);	// draws the 3D character

		static void	_evtChaTryonRenderEvent(C3DCompent *pSender, int x, int y);
		static void _evtChaEquipClearAll(CGuiData *sender, int x, int y, DWORD key);
		static void _evtChaLeftRotate(CGuiData *sender, int x, int y, DWORD key);
		static void _evtChaRightRotate(CGuiData *sender, int x, int y, DWORD key);
		static void _evtChaLeftContinueRotate(CGuiData *sender);
		static void _evtChaRightContinueRotate(CGuiData *sender);
		static void _evtTryonFormClose(CForm* pForm, bool& IsClose);

	};

}

