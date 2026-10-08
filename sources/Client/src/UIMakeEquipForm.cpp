#include "StdAfx.h"

#include "UIMakeEquipForm.h"
#include "UIFormMgr.h"
#include "uiform.h"
#include "UIFastCommand.h"
#include "UIEquipForm.h"
#include "UILabel.h"
#include "UIItemCommand.h"
#include "UIGoodsGrid.h"
#include "UIMemo.h"
#include "PacketCmd.h"
#include "GameApp.h"
#include "UIProgressBar.h"
#include "WorldScene.h"
#include "Character.h"
#include "UIBoxForm.h"
#include "StringLib.h"
#include "ItemRecord.h"

using namespace std;


namespace GUI
{

	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::Init()
	{
		CFormMgr &mgr = CFormMgr::s_Mgr;

		frmMakeEquip = mgr.Find("frmMakeEquip");
		if ( !frmMakeEquip)
		{
			LG("gui", g_oLangRec.GetString(685));
			return false;
		}
		frmMakeEquip->evtEntrustMouseEvent = _MainMouseEvent;
		frmMakeEquip->evtClose = _OnClose;

		cmdRouleau = dynamic_cast<COneCommand*>(frmMakeEquip->Find("cmdRouLeau"));
		if (!cmdRouleau)
		{
			return Error(g_oLangRec.GetString(561),
						 frmMakeEquip->GetName(), 
						 "cmdRouleau");
		}
		cmdRouleau->evtBeforeAccept = _DragEvtRouleau;

		cmdLastEquip = dynamic_cast<COneCommand*>(frmMakeEquip->Find("cmdLastEquip"));
		if (!cmdLastEquip)
		{
			return Error(g_oLangRec.GetString(561),
						 frmMakeEquip->GetName(), 
						 "cmdForgeItem");
		}

		char szBuf[32];
		for (int i(0); i<ITEM_NUM; i++)
		{
			sprintf(szBuf, "cmdItem%d", i);
			cmdItem[i] = dynamic_cast<COneCommand*>(frmMakeEquip->Find(szBuf));
			if (!cmdItem[i]) 
				return Error(g_oLangRec.GetString(561),
							 frmMakeEquip->GetName(), 
							 szBuf);
		}
		cmdItem[0]->evtBeforeAccept = _DragEvtEquipItem0;
		cmdItem[1]->evtBeforeAccept = _DragEvtEquipItem1;
		cmdItem[2]->evtBeforeAccept = _DragEvtEquipItem2;
		cmdItem[3]->evtBeforeAccept = _DragEvtEquipItem3;

		labForgeGold = dynamic_cast<CLabel*>(frmMakeEquip->Find("labForgeGold"));
		if (!labForgeGold)
		{
			return Error(g_oLangRec.GetString(561),
						 frmMakeEquip->GetName(), 
						 "labForgeGold");
		}


		memForgeItemState = dynamic_cast<CMemo*> (frmMakeEquip->Find("memForgeItemState")) ;
		if( !memForgeItemState ) 
			return Error(g_oLangRec.GetString(561), frmMakeEquip->GetName(), "memForgeItemState");	

		btnYes = dynamic_cast<CTextButton*>(frmMakeEquip->Find("btnForgeYes"));
		if( !btnYes ) 
			return Error(g_oLangRec.GetString(561), frmMakeEquip->GetName(), "btnForgeYes");

		return true;
	}

	//-----------------------------------------------------------------------------
	void  CMakeEquipMgr::CloseForm()
	{
	}

	//-----------------------------------------------------------------------------
	void  CMakeEquipMgr::SwitchMap()
	{
		this->Clear();
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::ShowMakeEquipForm(bool bShow)
	{
		this->Clear();

		if (bShow)
		{

			frmMakeEquip->SetPos(150, 150);
			frmMakeEquip->Reset();
			frmMakeEquip->Refresh();
			frmMakeEquip->Show();

			//opens the player's equipment slot as well
			int x = frmMakeEquip->GetX() + frmMakeEquip->GetWidth();
			int y = frmMakeEquip->GetY();
			g_stUIEquip.GetItemForm()->SetPos(x, y);
			g_stUIEquip.GetItemForm()->Refresh();

			if (!(m_isOldEquipFormShow = g_stUIEquip.GetItemForm()->GetIsShow()))
			{
				g_stUIEquip.GetItemForm()->Show();
			}
		}
		else
		{
			frmMakeEquip->Close();
			g_stUIEquip.GetItemForm()->SetIsShow(m_isOldEquipFormShow);
		}

		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::ShowConfirmDialog(long lMoney)
	{
		char szBuf[255] = { 0 };
		sprintf( szBuf, g_oLangRec.GetString(568), lMoney );
		GUI::stSelectBox * pBox = g_stUIBox.ShowSelectBox( _evtConfirmEvent, szBuf, true );
		pBox->frmDialog->evtEscClose = _evtConfirmCancelEvent;
	}

	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsRouleauCommand(COneCommand* oneCommand)
	{
		return (oneCommand == cmdRouleau);
	}

	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsAllCommand(COneCommand* oneCommand)
	{
		if (oneCommand == cmdRouleau)
			return true;
		else if (oneCommand == cmdRouleau)
			return true;
		else
			for (int i(0); i<ITEM_NUM; i++)
				if (cmdItem[i] == oneCommand)
					return true;

		return false;
	}

	int CMakeEquipMgr::GetItemComIndex(COneCommand* oneCommand)
	{
		for (int i(0); i<ITEM_NUM; i++)
			if (cmdItem[i] == oneCommand)
				return i;
		return -1;
	}

	void CMakeEquipMgr::PopGemItem(int iIndex)
	{
		PopItem(iIndex);
	}


	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::DragRouleauToEquipGrid()
	{
		this->PopRouleau();
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::DragItemToEquipGrid(int iIndex)
	{
		switch(m_iType) 
		{
		case MAKE_EQUIP_TYPE:
			if (this->IsMakeGem())
			{
				PopGemItem(iIndex);
			}
			else
			{
				PopEquipItem(iIndex);
			}
			break;
		case EQUIP_FUSION_TYPE:
			if (iIndex == 0)
			{	// removes the appearance and the equipment as well
				PopItem(iIndex);

				CItemCommand* pEquipItemCommand = 
					dynamic_cast<CItemCommand*>(cmdItem[1]->GetCommand());
				if (pEquipItemCommand)
				{
					PopItem(1);
				}
			}
			else if (iIndex == 1)
			{	// removes the equipment
				PopItem(iIndex);
			}
			else if (iIndex == 2)
			{	// removes the catalyst
				PopItem(iIndex);
			}
			break;
		case EQUIP_UPGRADE_TYPE:
			PopItem(iIndex);
			break;
		}
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::MakeEquipSuccess(long lChaID)
	{
		if( !CGameApp::GetCurScene() ) return;

		CCharacter* pCha = CGameApp::GetCurScene()->SearchByID( lChaID );
		if( !pCha ) return;

		pCha->SelfEffect(FORGE_SUCCESS_EFF_ID);

		if( pCha->IsMainCha() )
			this->ShowMakeEquipForm(false);

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::MakeEquipFailed(long lChaID)
	{
		if( !CGameApp::GetCurScene() ) return;

		CCharacter* pCha = CGameApp::GetCurScene()->SearchByID( lChaID );
		if( !pCha ) return;

		pCha->SelfEffect(FORGE_FAILED_EFF_ID);

		if( pCha->IsMainCha() )
			this->ShowMakeEquipForm(false);

	}


	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::MakeEquipOther(long lChaID)
	{
			this->ShowMakeEquipForm(false);
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushEquipItem(int iIndex, CItemCommand& rItem)
	{
		CItemCommand* pOldItemCommand = 
			dynamic_cast<CItemCommand*>(cmdItem[iIndex]->GetCommand());
		if (!pOldItemCommand)
			return;

		int iNum(0), iPos(-1);
		// updates the data of the corresponding COneCommand
		if (pOldItemCommand->GetIsPile())
		{
		}
		else
		{
			iNum = 1;
		}

		//int iPos = g_stUIEquip.GetGoodsGrid()->GetDragIndex();
		////// updates the corresponding EquipList
		//EquipInfo* pEquipInfo = new EquipInfo();
		//pEquipInfo->iPos = iPos;
		//pEquipInfo->iNum = iNum;
		//equipItems[iIndex].push_back(pEquipInfo);

		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopEquipItem(int iIndex)
	{
		// clears the data of the corresponding COneCommand

		// clears the corresponding EquipList
		ClearEquipList(iIndex);
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushEquipFusionItem(int iIndex, CItemCommand& rItem)
	{
		if (iIndex == 0)
		{
			if (IsAppearanceEquip(rItem))
			{
				PushItem(iIndex, rItem, 1);
			}
			else 
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(686));
				return;
			}
		}
		else if (iIndex == 1)
		{
			CItemCommand* pItemCommand =  
				dynamic_cast<CItemCommand*>(cmdItem[0]->GetCommand());
			if (!pItemCommand)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(687));
				return;
			}

			// modify by Philip.Wu  2006-06-11
			// while fusing, when the appearance equipment type is 27 and the attribute equipment type is 22, no matching check is performed
			if ((pItemCommand->GetItemInfo()->sType == 27 && rItem.GetItemInfo()->sType == 22) ||
				(IsSameAppearEquip(rItem, *pItemCommand)))
			{
				PushItem(iIndex, rItem, 1);
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(688));
				return;
			}
		}
		else if (iIndex == 2)
		{
			if (IsEquipFusionCatalyzer(rItem))
			{
				PushItem(iIndex, rItem, 1);
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(689));
				return;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopEquipFusionItem(int iIndex)
	{
		PopItem(iIndex);
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushEquipUpgradeItem(int iIndex, CItemCommand& rItem)
	{
		if (iIndex == 0)
		{
			if (IsFusionEquip(rItem))
			{
				PushItem(iIndex, rItem, 1);
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(690));
				return;
			}
		}
		else if (iIndex == 1)
		{
			if (IsEquipUpgradeSpar(rItem))
			{
				PushItem(iIndex, rItem, 1);
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(691));
				return;
			}
		}
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopEquipUpgradeItem(int iIndex)
	{
		PopItem(iIndex);
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushRouleau(CItemCommand& rItem)
	{
		// checks whether a scroll is already there and removes it if so
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdRouleau->GetCommand());
		if (pItemCommand)
		{
			if (pItemCommand->GetItemInfo()->lID == rItem.GetItemInfo()->lID)
			{
				return;
			}
			else
			{
				PopRouleau();
			}
		}

		// records the position of the scroll in the item slot
		m_iRouleauPos = g_stUIEquip.GetGoodsGrid()->GetDragIndex();

		// greys out the item slot of the scroll
		rItem.SetIsValid(false);
		// puts the created Command in
		CItemCommand* pItemCmd = new CItemCommand(rItem);
		cmdRouleau->AddCommand(pItemCmd);
		pItemCmd->SetIsValid(true);
		
		// updates the data of the corresponding recipe COneCommand; this is split into two flows, item refining and gem synthesis
		if (rItem.GetItemInfo()->sType == GEM_ROULEAU_TYPE)
		{	//gem synthesis
			PushNewGems();
		}
		else
		{	//item refining
			PushNewEquips(*(rItem.GetItemInfo()));
		}

		this->SetMakeEquipUI();

		return;
	}

	void CMakeEquipMgr::PushNewGems()
	{
	}


	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopRouleau()
	{
		// clears the data of the corresponding COneCommand
		if (m_iRouleauPos == -1)
			return;
		
		CCommandObj* pCmdObj = g_stUIEquip.GetGoodsGrid()->GetItem(m_iRouleauPos);
		if (pCmdObj)
			pCmdObj->SetIsValid(true);

		// 删除其中的Command (DelCommand()中调用delete删除)
		cmdRouleau->DelCommand();

		// clears the corresponding data of the three recipe COneCommands
		for (int i(0); i<ITEM_NUM; ++i)
			PopItem(i);

		// clears the last synthesised item
		PopLastEquip();

		this->SetMakeEquipUI();

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushLastEquip(CItemCommand& rItem)
	{
		
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopLastEquip()
	{
		CCommandObj* pCom = cmdLastEquip->GetCommand();
		if (pCom)
		{
			cmdLastEquip->DelCommand();
		}

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::ClearEquipList(int iIndex)
	{
		EquipListIter iter = equipItems[iIndex].begin();
		EquipListIter end  = equipItems[iIndex].end(); 
		for (; iter != end; ++iter)
		{
			//delete (*iter);
			SAFE_DELETE(*iter); // UI freeze handling
		}
		equipItems[iIndex].clear();

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::Clear()
	{
		// clears the UI form data
		labForgeGold->SetCaption("");
		btnYes->SetIsEnabled(false);


		// clears the scroll (this also clears the other Items)
		PopRouleau();
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::ClearEquips()
	{
		// clears the equipment slot item corresponding to the refining
		EquipListIter iter, end;
		for (int i(0); i<ITEM_NUM; ++i)
		{
			ClearEquipList(i);
		}
		
		return;
	}

	bool CMakeEquipMgr::CanPushEquip(int iIndex, CItemCommand& rItem)
	{
		return true;
	}

	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::CanPushStone(int iIndex, CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();
		if (!pItemRecord)
			return false;

		//  returns false straight away if it is neither a gem nor a refining stone
		if (pItemRecord->sType != GEN_STONE_TYPE && pItemRecord->sType != FORGE_STONE_TYPE)
		{
			g_pGameApp->MsgBox(g_oLangRec.GetString(692));
			return false;
		}

		// checks whether the other slot holds a gem
		int iOtherIndex = iIndex == 0 ? 1 : 0;
		CItemCommand* pOtherItem = dynamic_cast<CItemCommand* >(cmdItem[iOtherIndex]->GetCommand());
		if (pOtherItem)
		{	//the other slot has a gem, so it returns true only when the IDs match
			CItemRecord* pOtherItemRecord = pOtherItem->GetItemInfo();
			if (pItemRecord->lID == pOtherItemRecord->lID)
			{
					return true;
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(693));
				return false;
			}
		}
		else //the other slot has no gem, so it returns true directly
		{
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushItem(int iIndex, CItemCommand& rItem, int iItemNum)
	{
		// checks whether the original Cmd already has an Item and removes it if so
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdItem[iIndex]->GetCommand());
		if (pItemCommand)
		{
			PopItem(iIndex);
		}

		if (iItemNum == 1)
		{
			// records the position of the Item in the item slot
			EquipInfo* pEquipInfo = new EquipInfo();
			pEquipInfo->iPos = g_stUIEquip.GetGoodsGrid()->GetDragIndex();
			pEquipInfo->iNum = iItemNum;
			equipItems[iIndex].resize(1);
			equipItems[iIndex][0] = pEquipInfo;

			// greys out the item slot of the Item
			rItem.SetIsValid(false);

			// 将创建的Item放入Cmd中，这里用new将会在PopItem()中删除
			CItemCommand* pItemCmd = new CItemCommand(rItem);
			pItemCmd->SetIsValid(true);
			cmdItem[iIndex]->AddCommand(pItemCmd);
		}

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushNewEquips(CItemRecord& rRouleauRecord)
	{
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PopItem(int iIndex)
	{
		// 删除Cmd中的Item，该Item会在PushItem()中由new生成
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdItem[iIndex]->GetCommand());
		if (pItemCommand)
			cmdItem[iIndex]->DelCommand();	// this function deletes the Item

		// greys out the item slot of the Item
		CCommandObj* pItem(0);
		EquipListIter iter = equipItems[iIndex].begin();
		EquipListIter end  = equipItems[iIndex].end();
		for (; iter != end; ++iter)
		{
			pItem = g_stUIEquip.GetGoodsGrid()->GetItem((*iter)->iPos);
			if (pItem)
			{
				pItem->SetIsValid(true);
			}

		}

		ClearEquipList(iIndex);

		this->SetMakeEquipUI();

		return;
	}


	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::PushGemItem(int iIndex, CItemCommand& rItem)
	{
		if (iIndex < STONE_ITEM_NUM)
		{
			if (this->CanPushStone(iIndex, rItem))
			{
				// checks whether the original Cmd already has an Item and removes it if so
				CItemCommand* pItemCommand =  
					dynamic_cast<CItemCommand*>(cmdItem[iIndex]->GetCommand());
				if (pItemCommand)
				{
					PopItem(iIndex);
				}

				// records the position of the Item in the item slot
				EquipInfo* pEquipInfo = new EquipInfo();
				pEquipInfo->iPos = g_stUIEquip.GetGoodsGrid()->GetDragIndex();
				pEquipInfo->iNum = 1;
				equipItems[iIndex].push_back(pEquipInfo);

				// greys out the item slot of the Item
				rItem.SetIsValid(false);

				// 将创建的Item放入Cmd中，这里用new将会在PopItem()中删除
				CItemCommand* pItemCmd = new CItemCommand(rItem);
				pItemCmd->SetIsValid(true);
				cmdItem[iIndex]->AddCommand(pItemCmd);
			}
		}
		this->SetMakeEquipUI();
	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsEquipMakeRouleau(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord && pItemRecord->sType == GEM_ROULEAU_TYPE || pItemRecord->sType == EQUIP_ROULEAU_TYPE)
		{
			return true;
		}
		return false;
	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsEquipFusionRouleau(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord && pItemRecord->sType == EQUIP_FUSION_ROULEAU_TYPE)
		{
			return true;
		}
		return false;
	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsEquipUpgradeRouleau(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord && pItemRecord->sType == EQUIP_UPGRADE_ROULEAU_TYPE)
		{
			return true;
		}
		return false;
	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsAppearanceEquip(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		return CItemRecord::IsVaildFusionID(pItemRecord);
		
		//if (pItemRecord && pItemRecord->lID > APPEAR_EQUIP_BASE_ID)
		//if (pItemRecord && 
		//	(pItemRecord->lID >= CItemRecord::enumItemFusionStart && pItemRecord->lID < CItemRecord::enumItemFusionEnd) /*&&
		//	rItem.GetData().GetFusionItemID() > 0*/ )
		//{
		//	return true;
		//}
		//return false;

	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsEquipFusionCatalyzer(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord && pItemRecord->sType == EQUIP_FUSION_CATALYZER_TYPE)
		{
			return true;
		}
		return false;
	}
	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsSameAppearEquip(CItemCommand& rEquipItem, CItemCommand& rAppearItem)
	{
		CItemRecord* pEquipRecord = rEquipItem.GetItemInfo();
		CItemRecord* pAppearRecord = rAppearItem.GetItemInfo();

		// comment by Philip.Wu  2006-08-15  allows appearance equipment to be fused equipment too
		//if (IsAppearanceEquip(rEquipItem))
		//{
		//	return false;
		//}

		if (pEquipRecord && pAppearRecord&& pEquipRecord->sType == pAppearRecord->sType)
		{
			return true;
		}

		return false;
	}
	//---------------------------------------------------------------------
	bool CMakeEquipMgr::IsEquipUpgradeSpar(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord && pItemRecord->sType == EQUIP_UPGRADE_SPAR)
		{
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------
	bool CMakeEquipMgr::IsFusionEquip(CItemCommand& rItem)
	{
		//CItemRecord* pItemRecord = rItem.GetItemInfo();
		//SItemGrid& rItemData =rItem.GetData();
		//if ((pItemRecord->lID >= CItemRecord::enumItemFusionStart && pItemRecord->lID < CItemRecord::enumItemFusionEnd)
		//	&& rItemData.GetFusionItemID() > 0)
		//{
		//	return true;
		//}
		//return false;
		CItemRecord* pItemRecord = rItem.GetItemInfo();

		if (pItemRecord) {
			short sType = pItemRecord->sType;
			//	Close by alfred.shi 20080912  hats can be polished too
			
			switch(sType){
				case enumItemTypeSword		:
				case enumItemTypeGlave		:
				case enumItemTypeBow		:	
				case enumItemTypeHarquebus	:
				case enumItemTypeStylet		:
				case enumItemTypeCosh		:
				case enumItemTypeShield		:
				case enumItemTypeHair		:
				case enumItemTypeClothing	:
				case enumItemTypeGlove		:
				case enumItemTypeBoot		:
				case 25:
				case 26:
				case 27:
				case 81:
				case 82:
				case 83:
					return true;
				default:
					return false;
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------------
	// callback function
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_MainMouseEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
	{
		string name = pSender->GetName();
		if( name=="btnClose"  || name == "btnForgeNo" )  
		{ ////closes the form

			g_stUIMakeEquip.ShowMakeEquipForm(false);
			return;
		}
		else if( name == "btnForgeYes") 
		{
			if (g_stUIMakeEquip.m_iType == EQUIP_FUSION_TYPE)
			{
				if (!g_stUIMakeEquip.cmdItem[2]->GetCommand())
				{
					g_stUIBox.ShowSelectBox( _evtFusionNoCatalyzerConfirmEvent, 
						g_oLangRec.GetString(694), 
						true );
				}
				else
				{
					g_stUIMakeEquip.SendMakeEquipProtocol();
				}
			}
			else
			{
				g_stUIMakeEquip.SendMakeEquipProtocol();
			}
		}

		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_DragEvtRouleau(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		isAccept = false;

		CGoodsGrid* pGood = dynamic_cast<CGoodsGrid*>(CDrag::GetParent());
		if( pGood!=g_stUIEquip.GetGoodsGrid() ) return;

		CItemCommand* pItemCommand =  dynamic_cast<CItemCommand*>(pItem);
		if( !pItemCommand ) return;
		if (!(pItemCommand->GetIsValid())) return;


		switch(g_stUIMakeEquip.m_iType)
		{
		case MAKE_EQUIP_TYPE:
			if (g_stUIMakeEquip.IsEquipMakeRouleau(*pItemCommand))	
			{
				g_stUIMakeEquip.PushRouleau(*pItemCommand);
				g_stUIMakeEquip.SetMakeEquipUI();
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(695));
			}
			break;
		case EQUIP_FUSION_TYPE:
			if (g_stUIMakeEquip.IsEquipFusionRouleau(*pItemCommand))	
			{
				g_stUIMakeEquip.PushRouleau(*pItemCommand);
				g_stUIMakeEquip.SetMakeEquipUI();
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(696));
			}
			break;
		case EQUIP_UPGRADE_TYPE:
			if (g_stUIMakeEquip.IsEquipUpgradeRouleau(*pItemCommand))	
			{
				g_stUIMakeEquip.PushRouleau(*pItemCommand);
				g_stUIMakeEquip.SetMakeEquipUI();
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(697));
			}
			break;
		case ELF_SHIFT_TYPE:	// elf second rebirth
			if (g_stUIMakeEquip.IsElfShiftStone(*pItemCommand))
			{
				g_stUIMakeEquip.PushRouleau(*pItemCommand);
				g_stUIMakeEquip.SetMakeEquipUI();
			}
			else
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(698));
			}
		}

		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_DragEvtEquipItem0(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		g_stUIMakeEquip.DragEvtEquipItem(0, pSender, pItem, isAccept);
		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_DragEvtEquipItem1(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		g_stUIMakeEquip.DragEvtEquipItem(1, pSender, pItem, isAccept);
		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_DragEvtEquipItem2(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		g_stUIMakeEquip.DragEvtEquipItem(2, pSender, pItem, isAccept);
		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_DragEvtEquipItem3(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		g_stUIMakeEquip.DragEvtEquipItem(3, pSender, pItem, isAccept);
		return;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_OnClose(CForm* pForm, bool& IsClose)
	{
		g_stUIMakeEquip.Clear();
		CS_ItemForgeAsk(false);

	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_evtConfirmEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
	{
		CS_ItemForgeAnswer( nMsgType==CForm::mrYes );
	}
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::_evtConfirmCancelEvent(CForm* pForm)
	{
		CS_ItemForgeAnswer( false );
		pForm->SetIsShow(false);
	}
	//---------------------------------------------------------------------
	void CMakeEquipMgr::_evtFusionNoCatalyzerConfirmEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
	{
		if (nMsgType==CForm::mrYes)
		{
			g_stUIMakeEquip.SendMakeEquipProtocol();
		}
	}



	//-----------------------------------------------------------------------------
	// private function
	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::DragEvtEquipItem(int index, CGuiData *pSender, CCommandObj* pItem, bool& isAccept)
	{
		isAccept = false;

		CGoodsGrid* pGood = dynamic_cast<CGoodsGrid*>(CDrag::GetParent());
		if( pGood!=g_stUIEquip.GetGoodsGrid() ) return;

		CItemCommand* pItemCommand =  dynamic_cast<CItemCommand*>(pItem);
		if( !pItemCommand ) return;
		
		//if (!(pItemCommand->GetIsValid())) return; //removed this line, so you can drag 2x of the same gem in.
		//howver this could lead to issue with apparel, so make sure to check type.
		if (!pItemCommand->GetIsValid()){
			SItemGrid item = pItemCommand->GetData() ;
			int sType = pItemCommand->GetItemInfo()->sType;
			if ((sType != GEN_STONE_TYPE &&  sType !=FORGE_STONE_TYPE ) || item.sNum == 1){
				return; 
			}
		}

		if (!cmdRouleau->GetCommand())
		{
			// by Philip.Wu  changed the original decision logic: when the top cell key item is not placed, the item below may not be dragged in
			switch(this->m_iType)
			{
			case MAKE_EQUIP_TYPE:
			case EQUIP_FUSION_TYPE:
			case EQUIP_UPGRADE_TYPE:
				g_pGameApp->MsgBox(g_oLangRec.GetString(699));
				break;

			case ELF_SHIFT_TYPE:
				g_pGameApp->MsgBox(g_oLangRec.GetString(700));
				break;

			default:
				g_pGameApp->MsgBox(g_oLangRec.GetString(701));
				break;
			}

			return;

			// the following is the original handling, kept for now
			//if (IsEquipMakeRouleau(*pItemCommand) || 
			//	IsEquipFusionRouleau(*pItemCommand) || 
			//	IsEquipUpgradeRouleau(*pItemCommand))
			//{
			//	g_pGameApp->MsgBox("请将卷轴放入顶格");
			//	return;
			//}
			//else
			//{
			//	g_pGameApp->MsgBox("请先放入卷轴");
			//	return;
			//}
		}

		switch(g_stUIMakeEquip.m_iType)
		{
		case MAKE_EQUIP_TYPE:
			if (this->IsMakeGem())
			{
				PushGemItem(index, *pItemCommand);
			}
			else
			{
				PushEquipItem(index, *pItemCommand);
			}
			break;

		case EQUIP_FUSION_TYPE:
			PushEquipFusionItem(index, *pItemCommand);
			break;

		case EQUIP_UPGRADE_TYPE:
			PushEquipUpgradeItem(index, *pItemCommand);
			break;

		case ELF_SHIFT_TYPE:
			PushElfShiftItem(index, *pItemCommand);
			break;
		}

		this->SetMakeEquipUI();

		return;
	}

	//-----------------------------------------------------------------------------
	bool CMakeEquipMgr::IsMakeGem()
	{
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdRouleau->GetCommand());
		if (pItemCommand && pItemCommand->GetItemInfo()->sType == GEM_ROULEAU_TYPE)
		{
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::SetMakeEquipUI()
	{
		//memForgeItemState->SetCaption("这里写具体的内容");
		//memForgeItemState->ProcessCaption();
		switch(m_iType) 
		{
		case MAKE_EQUIP_TYPE:
			if (IsMakeGem() && cmdItem[0]->GetCommand() && cmdItem[1]->GetCommand())
			{
				labForgeGold->SetCaption(StringSplitNum(CalMakeEquipMoney()));
				btnYes->SetIsEnabled(true);
			}
			else
			{
				labForgeGold->SetCaption("");
				btnYes->SetIsEnabled(false);
			}
			break;
		case EQUIP_FUSION_TYPE:
			if (cmdRouleau->GetCommand() && cmdItem[0]->GetCommand() && cmdItem[1]->GetCommand())
			{
				labForgeGold->SetCaption(StringSplitNum(CalMakeEquipMoney()));
				btnYes->SetIsEnabled(true);
			}
			else
			{
				labForgeGold->SetCaption("");
				btnYes->SetIsEnabled(false);
			}
			break;
		case EQUIP_UPGRADE_TYPE:
			if (cmdRouleau->GetCommand() && cmdItem[0]->GetCommand() && cmdItem[1]->GetCommand())
			{
				labForgeGold->SetCaption(StringSplitNum(CalMakeEquipMoney()));
				btnYes->SetIsEnabled(true);
			}
			else
			{
				labForgeGold->SetCaption("");
				btnYes->SetIsEnabled(false);
			}
		case ELF_SHIFT_TYPE:
			if (cmdRouleau->GetCommand() && cmdItem[0]->GetCommand() && cmdItem[1]->GetCommand())
			{
				labForgeGold->SetCaption(StringSplitNum(CalMakeEquipMoney()));
				btnYes->SetIsEnabled(true);
			}
			else
			{
				labForgeGold->SetCaption("0");
				btnYes->SetIsEnabled(false);
			}

			break;
		}
	}

	//-----------------------------------------------------------------------------
	void CMakeEquipMgr::SendMakeEquipProtocol()
	{
		stNetItemForgeAsk kNetItemForgeAsk;
		kNetItemForgeAsk.chType = char(m_iType);	// synthesis

		if (m_iType == MAKE_EQUIP_TYPE)
		{
			if (IsMakeGem())
			{
				// scroll 0
				kNetItemForgeAsk.SGroup[0].sCellNum = 1;
				kNetItemForgeAsk.SGroup[0].pCell = new SForgeCell::SCell[1];
				kNetItemForgeAsk.SGroup[0].pCell[0].sNum = 1;
				kNetItemForgeAsk.SGroup[0].pCell[0].sPosID = m_iRouleauPos;

				// gem 1~2
				for (int i(1); i<=STONE_ITEM_NUM; ++i)
				{
					kNetItemForgeAsk.SGroup[i].sCellNum = 1;		// synthesis is always 1
					kNetItemForgeAsk.SGroup[i].pCell = new SForgeCell::SCell[1];
					kNetItemForgeAsk.SGroup[i].pCell[0].sNum = equipItems[i-1][0]->iNum;
					kNetItemForgeAsk.SGroup[i].pCell[0].sPosID = equipItems[i-1][0]->iPos;
				}
			}
			else
			{
				for (int i(0); i<ITEM_NUM; ++i)
				{
					kNetItemForgeAsk.SGroup[i].sCellNum = 1;		// synthesis is always 1
					kNetItemForgeAsk.SGroup[i].pCell = new SForgeCell::SCell[1];
					kNetItemForgeAsk.SGroup[i].pCell[1].sNum = 1;
					//kNetItemForgeAsk.SGroup[i].pCell[1].sPosID = m_iForgeItemPos[i];
				}

			}
		}
		else
		{	// fusing and upgrading
			// scroll 0
			kNetItemForgeAsk.SGroup[0].sCellNum = 1;
			kNetItemForgeAsk.SGroup[0].pCell = new SForgeCell::SCell[1];
			kNetItemForgeAsk.SGroup[0].pCell[0].sNum = 1;
			kNetItemForgeAsk.SGroup[0].pCell[0].sPosID = m_iRouleauPos;

			int iNum(0);
			if (m_iType == EQUIP_FUSION_TYPE)
			{
				if (!cmdItem[2]->GetCommand())
				{
					iNum = FUSION_NUM-1;
				}
				else
				{
					iNum = FUSION_NUM;
				}
			}
			else if (m_iType == EQUIP_UPGRADE_TYPE)
			{
				iNum = UPGRADE_NUM;
			}
			else if (m_iType == ELF_SHIFT_TYPE)
			{
				iNum = SHIFT_NUM;
			}

			for (int i(1); i<=iNum; ++i)
			{
				kNetItemForgeAsk.SGroup[i].sCellNum = 1;		// synthesis is always 1
				kNetItemForgeAsk.SGroup[i].pCell = new SForgeCell::SCell[1];
				kNetItemForgeAsk.SGroup[i].pCell[0].sNum = equipItems[i-1][0]->iNum;
				kNetItemForgeAsk.SGroup[i].pCell[0].sPosID = equipItems[i-1][0]->iPos;
			}

		}

		CS_ItemForgeAsk(true, &kNetItemForgeAsk);

		ShowMakeEquipForm(false);

		return;
	}

	//-----------------------------------------------------------------------------
	long CMakeEquipMgr::CalMakeEquipMoney()
	{
		CItemCommand* pItemCommand(0);
		long iLevelPlusOne = 0, nLevel1 = 0, nLevel2 = 0;		
		switch(m_iType) 
		{
		case MAKE_EQUIP_TYPE:
			return MAKE_EQUIP_MONEY;
			break;
		case EQUIP_FUSION_TYPE:
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[1]->GetCommand());	
			return EQUIP_FUSION_MONEY * pItemCommand->GetItemInfo()->sNeedLv;
			break;
		case EQUIP_UPGRADE_TYPE:
			// (equipment level + 1) squared * 1W (design document)
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[0]->GetCommand());	
			iLevelPlusOne = pItemCommand->GetData().GetItemLevel() + 1;
			return EQUIP_UPGRADE_MONEY * iLevelPlusOne * iLevelPlusOne;
			break;
		case ELF_SHIFT_TYPE:
			// elf second rebirth (design document)
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[0]->GetCommand());
			nLevel1  = pItemCommand->GetData().GetItemLevel();
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[1]->GetCommand());
			nLevel2  = pItemCommand->GetData().GetItemLevel();
			return (nLevel1 >= 60 || nLevel2 >= 60) ? 0 : (60 - nLevel1) * (60 - nLevel2) * 10000; 
			break;
		}
		return 0;
	}


	// whether it is the demon fruit needed for the elf second rebirth
	bool CMakeEquipMgr::IsElfShiftStone(CItemCommand& rItem)
	{
		CItemRecord* pItem = rItem.GetItemInfo();
		if( pItem      != NULL && pItem->lID == 3918 || pItem->lID == 3919 || pItem->lID == 3920 || 
			pItem->lID == 3921 || pItem->lID == 3922 || pItem->lID == 3924 || pItem->lID == 3925)
		{
			return true;
		}

		return false;
	}


	// whether it is an elf
	bool CMakeEquipMgr::IsElfShiftItem(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();
		if(pItemRecord && pItemRecord->sType == 59)
		{
			return true;
		}

		return false;
	}


	// puts in an elf
	void CMakeEquipMgr::PushElfShiftItem(int iIndex, CItemCommand& rItem)
	{
		CItemCommand* pItemCommand = NULL;
		SItemHint sItemHint;
		memset(&sItemHint, 0, sizeof(SItemHint));
		sItemHint.Convert(rItem.GetData(), rItem.GetItemInfo());

		// computes the elf level
		int nLevel = sItemHint.sInstAttr[ITEMATTR_VAL_STR] +
					 sItemHint.sInstAttr[ITEMATTR_VAL_AGI] +
					 sItemHint.sInstAttr[ITEMATTR_VAL_DEX] +
					 sItemHint.sInstAttr[ITEMATTR_VAL_CON] +
					 sItemHint.sInstAttr[ITEMATTR_VAL_STA];

		if(20 > nLevel)
		{
			g_pGameApp->MsgBox(g_oLangRec.GetString(702));
			return;
		}

		if (iIndex == 0)
		{
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[1]->GetCommand());

			// decides whether it is an elf and its ID differs from the elf in the second cell
			if (IsElfShiftItem(rItem) && 
				(NULL == pItemCommand || (rItem.GetItemInfo()->lID != pItemCommand->GetItemInfo()->lID)))
			{
				PushItem(iIndex, rItem, 1);
			}
			else 
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(703));
				return;
			}
		}
		else if (iIndex == 1)
		{
			pItemCommand = dynamic_cast<CItemCommand*>(cmdItem[0]->GetCommand());

			// decides whether it is an elf and its ID differs from the elf in the first cell
			if (IsElfShiftItem(rItem) && 
				(NULL == pItemCommand || (rItem.GetItemInfo()->lID != pItemCommand->GetItemInfo()->lID)))
			{
				PushItem(iIndex, rItem, 1);
			}
			else 
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(703));
				return;
			}
		}
	}

	void CMakeEquipMgr::PopElfShiftItem(int iIndex)
	{
		PopItem(iIndex);
	}

} // end of namespace GUI
