
#include "stdafx.h"
#include "UIPurifyForm.h"
#include "GameApp.h"
#include "UIEquipForm.h"
#include "uigoodsgrid.h"
#include "packetcmd.h"
#include "character.h"
#include "UIBoxform.h"
#include "packetCmd.h"
#include "uiitemcommand.h"

using namespace std;


namespace GUI
{

	CPurifyMgr::CPurifyMgr(void)
	{
	}

	CPurifyMgr::~CPurifyMgr(void)
	{
	}


	bool CPurifyMgr::Init()
	{
		frmEquipPurify = CFormMgr::s_Mgr.Find("frmEquipPurify");
		if(! frmEquipPurify)
		{
			LG("gui", "main.clu   frmEquipPurify not found.\n");
			return false;
		}

		labMoneyShow = dynamic_cast<CLabelEx*>(frmEquipPurify->Find("labMoneyShow"));
		if(! labMoneyShow)
		{
			LG("gui", "main.clu   frmEquipPurify:labMoneyShow not found.\n");
			return false;
		}

		btnForgeYes = dynamic_cast<CTextButton*>(frmEquipPurify->Find("btnForgeYes"));
		if(! btnForgeYes)
		{
			LG("gui", "main.clu   frmEquipPurify:btnForgeYes not found.\n");
			return false;
		}

		// the hint under the Command
		char szName[32] = {0};
		for(int i = 0; i < TYPE_COUNT; ++i)
		{
			sprintf(szName, "labHintleft%d", i + 1);
			labHintleft[i] = dynamic_cast<CLabelEx*>(frmEquipPurify->Find(szName));
			if(! labHintleft[i])
			{
				LG("gui", "main.clu   frmEquipPurify:%s not found.\n", szName);
				return false;
			}

			sprintf(szName, "labHintright%d", i + 1);
			labHintright[i] = dynamic_cast<CLabelEx*>(frmEquipPurify->Find(szName));
			if(! labHintright[i])
			{
				LG("gui", "main.clu   frmEquipPurify:%s not found.\n", szName);
				return false;
			}

			sprintf(szName, "labTitle%d", i + 1);
			labTitle[i] = dynamic_cast<CLabelEx*>(frmEquipPurify->Find(szName));
			if(! labTitle[i])
			{
				LG("gui", "main.clu   frmEquipPurify:%s not found.\n", szName);
				return false;
			}
		}

		cmdEquipPurify[PURIFY_ONE] = dynamic_cast<COneCommand*>(frmEquipPurify->Find("cmdEquipOne"));
		if(! cmdEquipPurify[PURIFY_ONE])
		{
			LG("gui", "main.clu   frmEquipPurify:cmdEquipOne not found.\n");
			return false;
		}

		cmdEquipPurify[PURIFY_TWO] = dynamic_cast<COneCommand*>(frmEquipPurify->Find("cmdEquipTwo"));
		if(! cmdEquipPurify[PURIFY_TWO])
		{
			LG("gui", "main.clu   frmEquipPurify:cmdEquipTwo not found.\n");
			return false;
		}

		frmEquipPurify->evtClose             = _evtClosePurifyForm;
		frmEquipPurify->evtEntrustMouseEvent = _evtMainMouseButton;

		cmdEquipPurify[PURIFY_ONE]->evtBeforeAccept = _evtDragPurifyOne;
		cmdEquipPurify[PURIFY_TWO]->evtBeforeAccept = _evtDragPurifyTwo;

		return true;
	}


	void CPurifyMgr::CloseForm()
	{
		// does nothing
	}


	void CPurifyMgr::ShowForm(int nType)
	{
		if(frmEquipPurify)
		{
			ClearAllCommand();

			if(PURIFY_TYPE <= nType && nType < PURIFY_TYPE + TYPE_COUNT)
			{
				m_nType = nType;  // saves the type

				for(int i = 0; i < TYPE_COUNT; ++i)
				{
					labHintleft[i]->SetIsShow (i + PURIFY_TYPE == m_nType);
					labHintright[i]->SetIsShow(i + PURIFY_TYPE == m_nType);
					labTitle[i]->SetIsShow    (i + PURIFY_TYPE == m_nType);
				}

				SetPurifyUI();

				frmEquipPurify->SetPos(100, 100);
				frmEquipPurify->Refresh();
				frmEquipPurify->SetIsShow(true);

				int nLeft, nTop;
				nLeft = frmEquipPurify->GetX2();
				nTop  = frmEquipPurify->GetY();

				g_stUIEquip.GetItemForm()->SetPos(nLeft, nTop);
				g_stUIEquip.GetItemForm()->Refresh();
				g_stUIEquip.GetItemForm()->Show();
			}
			else
			{
				frmEquipPurify->SetIsShow(false);
			}
		}
	}


	void CPurifyMgr::CloseAllForm()
	{
		if(frmEquipPurify)
		{
			frmEquipPurify->SetIsShow(false);
		}
	}


	void CPurifyMgr::ClearAllCommand()
	{
		PopItem(PURIFY_ONE);
		PopItem(PURIFY_TWO);
	}


	int CPurifyMgr::GetItemComIndex(COneCommand* pCom)
	{
		for(int i = 0; i < PURIFY_CELL_COUNT; ++i)
		{
			if(cmdEquipPurify[i] == pCom)
				return i;
		}

		return NO_USE;
	}


	void CPurifyMgr::DragItemToEquipGrid(int nIndex)
	{
		if(PURIFY_ONE == nIndex)
		{
			ClearAllCommand();
		}
		else if(PURIFY_TWO == nIndex)
		{
			PopItem(PURIFY_TWO);
		}

		SetPurifyUI();
	}


	// whether it is an item that can be equipped
	bool CPurifyMgr::IsEquipItem(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();
		if(pItemRecord)
		{
			short sType = pItemRecord->sType;

			switch(sType)
			{
			case 1:		//sword
			case 2:		//greatsword
			case 3:		//bow
			case 4:		//musket
			case 7:		//dagger
			case 9:		//staff
			case 11:	//shield
			case 20:	//hat
			case 22:	//clothing
			case 23:	//gloves
			case 24:	//boots
			case 25:	//rings
			case 26:	//necks
			case 27:	//tattoo
			case 81:	//brac
			case 82:	//brac
			case 83:	//brac
				return true;

			default:
				return false;
			}
		}

		return false;
	}


	bool CPurifyMgr::IsMainLifeItem(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();
		if(pItemRecord)
		{
			long nType = pItemRecord->lID;

			switch(nType)
			{
			case 1067://	crystal pot
			case 1068://	black hole crystal
			case 1069://	antimatter crystal
			case 1070://	particle flow crystal
				return true;

			default:
				return false;
			}
		}

		return false;
	}


	bool CPurifyMgr::IsRepairLifeItem(CItemCommand& rItem)
	{
		CItemRecord* pItemRecord = rItem.GetItemInfo();
		if(pItemRecord)
		{
			long nType = pItemRecord->lID;

			switch(nType)
			{
			case 2236://	repair tool
				return true;

			default:
				return false;
			}
		}

		return false;
	}


	void CPurifyMgr::PushItem(int iIndex, CItemCommand& rItem)
	{
		// decides whether the item can be dragged
		if(! rItem.GetIsValid())
		{
			return;
		}

		// checks whether the original Cmd already has an Item and removes it if so
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdEquipPurify[iIndex]->GetCommand());
		if (pItemCommand)
		{
			PopItem(iIndex);
		}

		// records the position of the Item in the item slot
		m_iPurifyItemPos[iIndex] = g_stUIEquip.GetGoodsGrid()->GetDragIndex();
		// greys out the item slot of the Item
		rItem.SetIsValid(false);

		// 将创建的Item放入Cmd中，这里用new将会在PopItem()中删除
		CItemCommand* pItemCmd = new CItemCommand(rItem);
		pItemCmd->SetIsValid(true);
		cmdEquipPurify[iIndex]->AddCommand(pItemCmd);

		SetPurifyUI();
	}


	void CPurifyMgr::PopItem(int iIndex)
	{
		// 删除Cmd中的Item，该Item会在PushItem()中由new生成
		CItemCommand* pItemCommand =  
			dynamic_cast<CItemCommand*>(cmdEquipPurify[iIndex]->GetCommand());
		if (! pItemCommand) return;

		cmdEquipPurify[iIndex]->DelCommand();	// this function deletes the Item

		// puts the item slot of the Item back to usable
		CCommandObj* pItem = 
					g_stUIEquip.GetGoodsGrid()->GetItem(m_iPurifyItemPos[iIndex]);
		if (pItem)
		{
			pItem->SetIsValid(true);
		}

		// records the position of the Item in the item slot
		m_iPurifyItemPos[iIndex] = NO_USE;
	}


	// sets up the screen and computes the price
	void CPurifyMgr::SetPurifyUI()
	{
		CItemCommand* pItem1 = dynamic_cast<CItemCommand*>(cmdEquipPurify[PURIFY_ONE]->GetCommand());
		CItemCommand* pItem2 = dynamic_cast<CItemCommand*>(cmdEquipPurify[PURIFY_TWO]->GetCommand());

		if (NULL == pItem1 || NULL == pItem2)
		{
			btnForgeYes->SetIsEnabled(false);
			labMoneyShow->SetCaption("");
			return;
		}

		pItem1 = dynamic_cast<CItemCommand*>(g_stUIEquip.GetGoodsGrid()->GetItem(m_iPurifyItemPos[PURIFY_ONE]));
		pItem2 = dynamic_cast<CItemCommand*>(g_stUIEquip.GetGoodsGrid()->GetItem(m_iPurifyItemPos[PURIFY_TWO]));

		if (NULL == pItem1 || NULL == pItem2)
		{
			btnForgeYes->SetIsEnabled(false);
			labMoneyShow->SetCaption("");
			return;
		}

		// both slots are filled, so the 'confirm' button may be clicked
		btnForgeYes->SetIsEnabled(true);

		int nMoney = 0;

		// item purifying
		if(GetType() == CPurifyMgr::PURIFY_TYPE)
		{
			//int nMainLevel = pItem1->GetItemInfo()->sNeedLv;
			//nMoney = nMainLevel * nMainLevel * 1000;

			nMoney = 1000000;
		}
		// shell
		else if(GetType() == CPurifyMgr::ENERGY_TYPE)
		{
			int nBatteryID = pItem2->GetItemInfo()->lID;
			if(nBatteryID == 1022)
			{
				nMoney = 300;
			}
			else if(nBatteryID == 1024)
			{
				nMoney = 1000;
			}
		}
		// gem
		else if(GetType() == CPurifyMgr::GETSTONE_TYPE)
		{
			int nStoneLevel = pItem1->GetForgeInfo().nStoneLevel[0] + 
							  pItem1->GetForgeInfo().nStoneLevel[1] + 
							  pItem1->GetForgeInfo().nStoneLevel[2];
			nMoney = nStoneLevel * 10000;
		}
		// repairing the broken pot
		else if(g_stUIPurify.GetType() == CPurifyMgr::REPAIR_OVEN_TYPE)
		{
			//int nOvenLevel = 0;
			//for(int i = 0; i < 5; ++i)
			//{
			//	if(pItem1->GetData().sInstAttr[i][0] == ITEMATTR_VAL_STR)
			//	{
			//		nOvenLevel = pItem1->GetData().sInstAttr[i][1];
			//		break;
			//	}
			//}

			//nMoney = nOvenLevel * nOvenLevel * 100;

			nMoney = 200;
		}

		char szBuffer[256] = {0};
		sprintf(szBuffer, "%d", nMoney);
		labMoneyShow->SetCaption(szBuffer);
	}


	// sends the purify protocol
	void CPurifyMgr::SendNetProtocol()
	{
		CS_ItemForgeAsk(true, GetType(), m_iPurifyItemPos, PURIFY_CELL_COUNT);
	}


///////////////////////////////////////////////////////////////////////////
//
//	screen message handling
//

	// the main item is dragged in
	void CPurifyMgr::_evtDragPurifyOne(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		CItemCommand* pItemCommand =  dynamic_cast<CItemCommand*>(pItem);
		if( !pItemCommand || !pItemCommand->GetIsValid()) return;

		CGoodsGrid* pGood = dynamic_cast<CGoodsGrid*>(CDrag::GetParent());
		if( pGood != g_stUIEquip.GetGoodsGrid() ) return;

		// item purifying
		if(g_stUIPurify.GetType() == CPurifyMgr::PURIFY_TYPE)
		{
			//if(! g_stUIPurify.IsEquipItem(*pItemCommand))
			//{
			//	g_pGameApp->MsgBox(g_oLangRec.GetString(831)); // 主要装备不合法，请重新选择
			//	return;
			//}
		}
		// shell
		else if(g_stUIPurify.GetType() == CPurifyMgr::ENERGY_TYPE)
		{
			if(pItemCommand->GetItemInfo()->sType != 29)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(852)); // what you put in is not a shell; please choose again
				return;
			}
		}
		// gem
		else if(g_stUIPurify.GetType() == CPurifyMgr::GETSTONE_TYPE)
		{
			if(! g_stUIPurify.IsEquipItem(*pItemCommand))
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(831)); // the main equipment is not valid; please choose again
				return;
			}
		}
		// repairing the broken pot
		else if(g_stUIPurify.GetType() == CPurifyMgr::REPAIR_OVEN_TYPE)
		{
			if(! g_stUIPurify.IsMainLifeItem(*pItemCommand))
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(891)); // what you put in is not a life skill tool; please choose again
				return;
			}
		}
		else	// the type is not valid
		{
			return;
		}

		g_stUIPurify.PushItem(PURIFY_ONE, *pItemCommand);
	}


	// the item to purify is dragged in
	void CPurifyMgr::_evtDragPurifyTwo(CGuiData *pSender,CCommandObj* pItem,bool& isAccept)
	{
		CItemCommand* pItemCommand =  dynamic_cast<CItemCommand*>(pItem);
		if( !pItemCommand || !pItemCommand->GetIsValid()) return;

		CGoodsGrid* pGood = dynamic_cast<CGoodsGrid*>(CDrag::GetParent());
		if( pGood != g_stUIEquip.GetGoodsGrid() ) return;

		CItemCommand* pMainItem = dynamic_cast<CItemCommand*>(g_stUIPurify.cmdEquipPurify[PURIFY_ONE]->GetCommand());

		// item purifying
		if(g_stUIPurify.GetType() == CPurifyMgr::PURIFY_TYPE)
		{
			if(NULL == pMainItem)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(828)); // please put in the main equipment first
				return;
			}

			//if(pMainItem->GetItemInfo()->sType != pItemCommand->GetItemInfo()->sType)
			//{
			//	g_pGameApp->MsgBox(g_oLangRec.GetString(829)); // 提纯装备和主要装备不是同一类型，请重新选择
			//	return;
			//}

			//if(pMainItem->GetData().GetItemLevel() > pItemCommand->GetData().GetItemLevel() + 5)
			//{
			//	g_pGameApp->MsgBox(g_oLangRec.GetString(830)); // 提纯装备等级不能小于主要装备 5 级，请重新选择
			//	return;
			//}
		}
		// shell
		else if(g_stUIPurify.GetType() == CPurifyMgr::ENERGY_TYPE)
		{
			if(NULL == pMainItem)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(853)); // please put in the shell first
				return;
			}
		}
		// gem
		else if(g_stUIPurify.GetType() == CPurifyMgr::GETSTONE_TYPE)
		{
			if(NULL == pMainItem)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(828)); // please put in the main equipment first
				return;
			}
		}
		// repairing the broken pot
		else if(g_stUIPurify.GetType() == CPurifyMgr::REPAIR_OVEN_TYPE)
		{
			if(NULL == pMainItem)
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(892)); // please put in the life skill tool first
				return;
			}

			if(! g_stUIPurify.IsRepairLifeItem(*pItemCommand))
			{
				g_pGameApp->MsgBox(g_oLangRec.GetString(893)); // what you put in is not a repair tool; please choose again
				return;
			}
		}
		// the type is not valid
		else
		{
			return;
		}

		g_stUIPurify.PushItem(PURIFY_TWO, *pItemCommand);
	}


	// mouse button click event
	void CPurifyMgr::_evtMainMouseButton(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey)
	{
		string strName = pSender->GetName();

		if(strName == "btnForgeYes")
		{
			g_stUIPurify.SendNetProtocol();
			g_stUIPurify.CloseAllForm();
		}
	}


	// close window event
	void CPurifyMgr::_evtClosePurifyForm(CForm* pForm, bool& IsClose)
	{
		g_stUIPurify.ClearAllCommand();
		CS_ItemForgeAsk(false);
	}

}

