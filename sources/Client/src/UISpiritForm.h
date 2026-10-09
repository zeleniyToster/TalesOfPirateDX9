#pragma once


#include "uiglobalvar.h"
#include "uiform.h"
#include "uiformmgr.h"
#include "uilabel.h"
#include "uitextbutton.h"
#include "uicheckbox.h"
#include "uiimage.h"
#include "uicommand.h"
#include "uifastcommand.h"


namespace GUI
{

	class CSpiritMgr : public CUIInterface
	{
	public:
		CSpiritMgr(void);
		~CSpiritMgr(void);

		static const int NO_USE = -1;
		static const int SPIRIT_MARRY_TYPE = 6;			// elf fusion type (used to talk to the server)

		static const int SPIRIT_MARRY_CELL_COUNT = 3;	// number of cells
		static const int SPIRIT_MARRY_ITEM = 0;			// demon fruit
		static const int SPIRIT_MARRY_ONE  = 1;			// elf 1
		static const int SPIRIT_MARRY_TWO  = 2;			// elf 2

		static const int ERNIE_IMAGE_COUNT = 5;			// number of slot machine images
		static const int ERNIE_SPEED       = 50;		// flip speed (milliseconds)
		static const int ERNIE_COIN_COUNT  = 5;			// 5 elf coins
		static const int ERNIE_EMPTY_COUNT = 5;			// 5 free bag slots

		void ClearAllCommand();
		void ShowMarryForm(bool bShow = true);
		void ShowErnieForm(bool bShow = true);

		void UpdateErnieNumber(short nNum, short nID1, short nID2, short nID3);	// updates the slot machine
		void UpdateErnieString(const char* szText);		// updates the displayed content
		void ShowErnieHighLight();

		int GetType() {  return SPIRIT_MARRY_TYPE;	}	// type

	protected:
		virtual bool Init();
		virtual void CloseForm();
		virtual void FrameMove(DWORD dwTime);


	private:

		//
		//  elf fusion
		//
		CForm*		    frmSpiritMarry;
		CLabel*			labMoneyShow;
		CTextButton*	btnForgeYes;
		COneCommand*    cmdSpiritMarry[SPIRIT_MARRY_CELL_COUNT];

		int             m_iSpiritItemPos[SPIRIT_MARRY_CELL_COUNT];

		void		PushItem(int iIndex, CItemCommand& rItem);
		void		PopItem(int iIndex);
		void		SetSpiritUI();

		bool		IsValidSpiritItem(CItemCommand& rItem);	// whether it is a valid demon fruit
		bool		IsValidSpirit(CItemCommand& rItem);		// whether it is a valid elf (LV > 20)

		void		SendSpiritMarryProtocol();

		static void _evtDragMarryItem(CGuiData *pSender,CCommandObj* pItem,bool& isAccept);	// the demon fruit is dragged in
		static void _evtDragMarryOne(CGuiData *pSender,CCommandObj* pItem,bool& isAccept);	// elf 1 is dragged in
		static void _evtDragMarryTwo(CGuiData *pSender,CCommandObj* pItem,bool& isAccept);	// elf 2 is dragged in

		static void _evtMainMouseButton(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);// mouse button click event
		static void _evtCloseMarryForm(CForm* pForm, bool& IsClose);	// close window event

	private:

		//
		//  elf slot machine
		//
		CForm*			frmSpiritErnie;
		CImage*			imgLine[9][ERNIE_IMAGE_COUNT];
		CCheckBox*		chkSetmoney[3];
		CTextButton*	btnStop[3];
		COneCommand*	cmdItem[9];
		CTextButton*	btnStart;

		CLabelEx*		labUsemoney[3];
		CLabelEx*		labLastshow1;
		CLabelEx*		labLastshow2;

		bool			m_bIsRunning[3];

		DWORD			m_dwLastTickCount;
		int				m_nCurrImage;

		void			AddTigerItem(short nNum, short sItemID);
		void			ClearTigerItem(void);
		void			ErnieHightLight(int nNum, bool b = true);

		static void _evtErnieMouseButton(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);// mouse button click event
		static void _evtCloseErnieForm(CForm* pForm, bool& IsClose);	// close window event

	};

}

