#pragma once
#include "UIGlobalVar.h"
#include "NetProtocol.h"
#include "SkillRecord.h"
#include "UIGuiData.h"

class CCharacter;
class CSkillStateRecord;
struct stNetSkillBag;

namespace GUI
{
class CItemCommand;

// skill, item, shortcut, equipment
class CEquipMgr : public CUIInterface
{
public:

	int				GetIMP(){return lIMP;}

	void			SynSkillBag(DWORD dwCharID, stNetSkillBag *pSSkillBag);

    void            UpdataEquip( const stNetChangeChaPart& SPart, CCharacter* pCha );
    void            UpdataEquipSpy( const stNetChangeChaPart& SPart, CCharacter* pCha );
    void            UpdataEquipData( const stNetChangeChaPart& SPart, CCharacter* pCha );

    void            UpdateShortCut( stNetShortCut& stShortCut );		// updates the shortcut bar
	void			DelFastCommand( CCommandObj* pObj );				// deletes the current shortcut bar
    bool            ExecFastKey( int key );
	void			FastChange(int nIndex, short sGridID, char chType,bool update = false);

	void            CloseAllForm();		// closes the window

	// compares the shortcut bar with the item slot and, for shortcuts that differ from the server, sends a protocol to the server to update the shortcut bar; returns the number updated
	// used when items are swapped inside the item slot
	int				RefreshServerShortCut();							

	CGoodsGrid* GetGoodsGrid() { return grdItem; }	// operates on the item slot grid
	CForm* GetItemForm() { return frmInv; }   // operates on the item form

	CForm* stateDrags;

	// unloads an item from the equipment slot into the item slot
	void			UnfixToGrid( CCommandObj* pItem, int nGridID, int nLink );

	CSkillRecord*	FindSkill( int nID );

	CItemCommand*	GetEquipItem( unsigned int nLink );

	bool			IsEquipCom( COneCommand* pCom );

	void			SetIsLock(bool bLock);
	bool			GetIsLock();
	
	void 			UpdateIMP(int imp);

	// gets the total number of an item in the current bag
	int				GetItemCount(int nID);

	static CGuiPic*		GetChargePic( unsigned int n );

	void ShowSellPrompt();
    static void		evtThrowItemEvent(CGuiData *pSender,int id,bool& isThrow);					// throws an item out of the item slot
    static void		evtSwapItemEvent(CGuiData *pSender,int nFirst, int nSecond, bool& isSwap); // swaps items inside the item slot

protected:
	virtual bool Init();
    virtual void End();
	virtual void LoadingCall();
	virtual void FrameMove(DWORD dwTime);
	virtual void SwitchMap();
private:
	enum eDirectType
	{
		LEFT = -1, RIGHT = 1,
	};
	
	static void _UnequipPart(CGuiData *pSender, int x, int y, DWORD key);
	void RotateSpy(eDirectType enumDirect = LEFT);
	void RotateCha(eDirectType enumDirect = LEFT);

    CSkillList*		GetSkillList( char nType );
	void			RefreshUpgrade();
	void			RefreshSkillJob( int nJob );
	void			RenderSpy(int x,int y);
	void			RenderCha(int x,int y);
	void 			RenderModel(int x, int y, CCharacter* original, CCharacter* model, int rotation, bool refresh = false);
	
	bool			_GetThrowPos( int& x, int& y );
	static void ThrowSelectedItems(CGoodsGrid* grid);
	static void LockSelectedItems(CGoodsGrid* grid);
	static void DeleteSelectedItems(CGoodsGrid* grid);
public:
	bool			_GetCommandShortCutType( CCommandObj* pItem, char& chType, short& sGridID );
private:
	static void		_EqSpyRenderEvent(C3DCompent *pSender, int x, int y);
	static void		_ChaRenderEvent(C3DCompent *pSender, int x, int y);
	
	static void		_evtSkillFormShow(CGuiData *pSender);
	static void		_evtEquipFormShow(CGuiData *pSender);
	static void		_evtEquipFormClose( CForm *pForm, bool& IsClose );
	
	static void		_evtSpyFormClose( CForm *pForm, bool& IsClose );
	static void		_evtSpyFormShow(CGuiData *pSender);

	static void		_evtSkillUpgrade(CSkillList *pSender, CSkillCommand* pSkill);
    static void		_evtFastChange(CGuiData *pSender, CCommandObj* pItem, bool& isAccept);		// the shortcut bar changed
	static void		_evtEquipEvent(CGuiData *pSender, CCommandObj* pItem, bool& isAccept);		// equips from the item slot into the equipment slot
	static void		_evtThrowEquipEvent(CGuiData *pSender, CCommandObj* pItem, bool& isThrow);  // throws equipment out of the equipment slot

	static void		_evtButtonClickEvent( CGuiData *pSender, int x, int y, DWORD key);
	static void		_evtRMouseGridEvent(CGuiData *pSender,CCommandObj* pItem,int nGridID);

	bool			_UpdataEquip( SItemGrid& Item, int nLink );
	bool			_UpdataEquipSpy( SItemGrid& Item, int nLink );
    void			_ActiveFast( int num );         // activates which bar

	static void		_evtItemFormMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	// lock added, MSGBOX confirm
	static void		_CheckLockMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	static void _RotateChaLeft(CGuiData *sender, int x, int y, DWORD key );
	static void _RotateChaRight(CGuiData *sender, int x, int y, DWORD key );
	static void _RotateChaLeftContinue(CGuiData *sender );
	static void _RotateChaRightContinue(CGuiData *sender );

	static void _RotateSpyLeft(CGuiData *sender, int x, int y, DWORD key );
	static void _RotateSpyRight(CGuiData *sender, int x, int y, DWORD key );
	static void _RotateSpyLeftContinue(CGuiData *sender );
	static void _RotateSpyRightContinue(CGuiData *sender );
	static void _ClickOpenApparel(CGuiData *pSender, int x, int y, DWORD key );
	static void _ClickTempBag(CGuiData* pSender, int x, int y, DWORD key);
	static void _OnDragStates(CGuiData* pSender, int x, int y, DWORD key);
private:
	static int				lIMP;
	CForm*			frmSkill;

    CLabel*         labPoint;                   // remaining skill points
    CLabel*         labPointLife;               // remaining life skill points
    CSkillList*     lstFightSkill;              // combat skill bar
    CSkillList*     lstLifeSkill;               // life skill bar
    CSkillList*     lstSailSkill;               // navigation skill bar

	CTextButton* btnOpenTempBag;
	
	
	CCheckBox* chkLinkEqForm;
	
	CForm*			frmItemSpy;					// item form
	CGoodsGrid*     grdItem;					// item slot
	COneCommand*    cnmEquip[enumEQUIP_NUM];    // equipment slot
	COneCommand*    cnmEquipSpy[enumEQUIP_NUM];    // equipment slot
    stNetChangeChaPart    stEquip;
    stNetChangeChaPart    stEquipSpy;
    stNetShortCut         _stShortCut;

    int				_nFastCur;					// which slot the shortcut bar is currently on
    CFastCommand**  _pFastCommands;
	CLabel*			_pActiveFastLabel;			// the page number used to activate the shortcut bar

	CLabel*			lblGold;					// money
	CImage*			imgItem4;					// the pictures for the first 4 item cells

	CImage*			imgLock;
	CImage*			imgUnLock;
	CCharacter* 	eqSpyTarget;
	int 					eqSpyTargetRotate;
	int 					chaModelRotate;
	
	CCharacter* 	chaModel;
	CCharacter* 	spyModel;
	
public:
	bool	refreshChaModel;
	bool	refreshSpyModel;


private:
	struct stThrow
	{
		stThrow() : nX(0), nY(0), nGridID(0), pSelf(0) {}

		int				nX, nY;
		int				nGridID;
		CCharacter* pSelf;
	};
	stThrow		_sThrow;
	static void _evtThrowDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtThrowBoatDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	void		_SendThrowData( const stThrow& sthrow, int nThrowNum=1 );

	struct stUnfix
	{
		stUnfix() : nLink(0), nGridID(0), nX(0), nY(0), pItem(NULL) {}
		void	Reset()	{ nGridID=-1; nLink=-1;	pItem=NULL;}

		int		nLink;
		int		nGridID;
		int		nX, nY;
		CCommandObj* pItem;
	};
	stUnfix		_sUnfix;

	static void _evtUnfixDialogEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
	void		_SendUnfixData( const stUnfix& unfix, int nUnfixNum=1 );
	void		_StartUnfix( stUnfix& unfix );


	static void _evtDeleteItemYesNoEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);
	static void _evtLockItemYesNoEvent(CCompent* pSender, int nMsgType, int x, int y, DWORD dwKey);
	int			_nLostGridID;
	int rightClickItemIndex{};
	CMenu* rightClickItemMenu{};

	typedef	std::vector<CSkillRecord*>	vskill;
	vskill		_skills;

	typedef std::vector<int>	ints;
	vskill		_cancels;			// skill states that can be reactivated

	typedef std::vector<CSkillStateRecord*>  states;
	states		_charges;			// charging skill states

	static CGuiPic	_imgCharges[enumEQUIP_NUM];

	struct SSplitItem
	{
		static void		_evtSplitItemEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);	// split item callback
		int			nFirst;
		int			nSecond;
		CCharacter*	pSelf;
	};
	static SSplitItem	SSplit;

public:	// item repair related
	void	ShowRepairMsg( const char* pItemName, long lMoney );
	void    SetIsShow(bool bShow);

private:
	static void		_evtRepairEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
public:
	CForm*			frmInv;	
};

inline CSkillList* CEquipMgr::GetSkillList( char nType )	
{ 
	switch( nType )
	{
	case enumSKILL_SAIL: return lstSailSkill;
	case enumSKILL_FIGHT: return lstFightSkill;
	default: return lstLifeSkill;
	}
}	

}

