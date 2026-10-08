#include "StdAfx.h"
#include "uiboatform.h"
#include "uiform.h"
#include "uiedit.h"
#include "uilabel.h"
#include "tools.h"
#include "uiformmgr.h"
#include "character.h"
#include "uigoodsgrid.h"
#include "uitemplete.h"
#include "uiequipform.h"
#include "uitradeform.h"
#include "gameapp.h"
#include "uistartform.h"
#include "uibourseForm.h"
#include "uiItemCommand.h"

using namespace GUI;

//---------------------------------------------------------------------------
// class CBoat  user boat class
//---------------------------------------------------------------------------
CBoat::CBoat() 
: _chtBoat(0), _frmShipRoom(0)
, _grdHold(0), _nIndex(0) 
{
}

bool CBoat::Init( int n, CForm* ship, GuiDragInGridEvent evt )  // user boat information initialisation
{
	_frmShipRoom = ship;
	if( !_frmShipRoom ) return false;

	_nIndex = n;

	_grdHold = dynamic_cast<CGoodsGrid*>(_frmShipRoom->Find("grdRoom"));
	if( !_grdHold ) 
	{
		LG("gui", g_oLangRec.GetString(443) );
		return false;
	}

	_grdHold->SetIsHint(false);
	_grdHold->SetParent(ship);
	_frmShipRoom->evtEscClose = _evtEscClose;	// ESC close event add by Philip.Wu  2006-06-22

	// with no event it is a temporary boat and nothing can be done with it
	if( evt )
	{
		_grdHold->evtBeforeAccept = evt;
		_grdHold->evtThrowItem = CEquipMgr::evtThrowItemEvent;
		_grdHold->evtSwapItem = CEquipMgr::evtSwapItemEvent;
		_grdHold->evtRMouseEvent = _evtHoldGridRMouse;
	}
	return true;
}

void CBoat::_evtHoldGridRMouse(CGuiData *pSender,CCommandObj* pItem,int nGridID)
{
	if (!pItem) return;  

	// exchange right click
	if (g_stUIBourse.GetForm()->GetIsShow())
	{
		CItemCommand* pkItemCmd = dynamic_cast<CItemCommand*>(pItem);
		if (!pkItemCmd)	return;

		g_stUIBourse.SaleGoods(*pkItemCmd, nGridID);
	}
}

void CBoat::Reset()  //  if the character does not exist the user boat information is cleared
{
	_chtBoat = NULL;
	_grdHold->Clear();
}


// ESC close window event; the parent window is set to not shown as well  add by Philip.Wu  2006-06-22
void CBoat::_evtEscClose( CForm* pForm )
{
	if(pForm)
	{
		pForm->SetIsShow(false);

		if(pForm->GetParent())
		{
			pForm->GetParent()->SetIsShow(false);
		}
	}
}


//---------------------------------------------------------------------------
// class CBoatMgr user boat management class
//---------------------------------------------------------------------------
bool CBoatMgr::Init() // user boat management information initialisation
{
	CForm* frm[defMaxBoat+1] = { 0 }; 
	CForm* frmShipRoom = _FindForm("frmShipRoom"); // looks up the boat form
	if(!frmShipRoom) return false;
	frm[0] = frmShipRoom;

	CForm* frmClone = NULL;
	char buf[128] = { 0 };
	for( int i=1; i<defMaxBoat+1; i++ ) // 
	{
		frmClone = dynamic_cast<CForm*>( frmShipRoom->Clone() );
		if( !frmClone ) return false;

		sprintf( buf, "frmShipRoom%d", i );
		frmClone->SetName( buf );

		CFormMgr::s_Mgr.AddForm( frmClone, enumMainForm );
		frmClone->Init();

		frm[i] = frmClone;
	}

	GuiDragInGridEvent evtGrid = _evtDragToGoodsEvent;
	for( int i=0; i<defMaxBoat; i++ )
	{
		if( !_cBoats[i].Init( i, frm[i], evtGrid ) ) return false;
	}

	if( !_cOther.Init( defMaxBoat+1, frm[defMaxBoat], NULL ) )
		return false;

	_pHuman = NULL;
	return true;
}

void CBoatMgr::End()
{

}

bool CBoatMgr::AddBoat( CCharacter* pBoat )
{
	CBoat* p = GetFreeBoat();
	if( !p ) return false; //already have the maximum count, no more free boats can be obtained

	p->Link( pBoat );
	return true;
}

CBoat*	CBoatMgr::GetFreeBoat()   // obtains a free boat
{ 
	for( int i=0; i<defMaxBoat; i++ ) // the maximum number of boats obtainable
		if( !_cBoats[i].GetIsValid() )
			return &_cBoats[i];

	return NULL;
}

CBoat* CBoatMgr::FindBoat( unsigned int ulWorldID )  // looks up every boat the user has
{
	for( int i=0; i<defMaxBoat; i++ )
	{
		if( _cBoats[i].GetIsValid() && _cBoats[i].GetCha()->getAttachID()==ulWorldID )
		{
			return &_cBoats[i];
		}
	}

	return NULL;
}

CGoodsGrid*	CBoatMgr::FindGoodsGrid( unsigned int ulWorldID ) // looks up the boat item
{
	if( _pHuman && _pHuman->getAttachID()==ulWorldID )
		return g_stUIEquip.GetGoodsGrid();

	CBoat* pBoat = FindBoat( ulWorldID );
	if( pBoat ) return pBoat->GetGoodsGrid();

	return NULL;
}

CCharacter*	CBoatMgr::FindCha( unsigned int ulWorldID ) // looks up the boat owner
{
	if( _pHuman && _pHuman->getAttachID()==ulWorldID ) return _pHuman;

	CBoat* pBoat = FindBoat( ulWorldID );
	if( pBoat ) return pBoat->GetCha();

	return NULL;
}

CCharacter* CBoatMgr::ChangeMainCha( unsigned int ulWorldID ) // switches the main character
{
	CCharacter* pCha = FindCha( ulWorldID );
	if( pCha ) 
	{
		pCha->SetHide( FALSE );

		CGameScene* pScene = CGameApp::GetCurScene();
		if( pScene )
		{
			pScene->SetMainCha( pCha->getID() );
			g_stUIStart.ShowShipSailForm( pCha->IsBoat() );

			// add by Philip.Wu  2006-07-03  closes the trade and equipment windows when switching maps (BUG fix)
			g_stUITrade.CloseAllForm();
			g_stUIEquip.CloseAllForm();
		}
	}
	else
	{
		LG( "error", g_oLangRec.GetString(444), ulWorldID );
	}
	return pCha;
}

void CBoatMgr::Clear()  //  clears every boat of the user
{
	_pHuman = NULL;
	for( int i=0; i<defMaxBoat; i++ )
	{
		_cBoats[i].Reset();
	}
}

CCharacter* CBoatMgr::FindCha( CGoodsGrid* pGoods ) // finds the boat owner by matching equipment
{
	if( pGoods==g_stUIEquip.GetGoodsGrid() )
		return _pHuman;

	for( int i=0; i<defMaxBoat; i++ )
	{
		if( _cBoats[i].GetGoodsGrid()==pGoods )
			return _cBoats[i].GetCha();
	}
	return NULL;
}



