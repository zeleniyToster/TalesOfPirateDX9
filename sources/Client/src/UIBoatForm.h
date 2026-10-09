#pragma once
#include "UIGlobalVar.h"
#include "uiform.h"
#include "uigoodsgrid.h"

class CCharacter;

namespace GUI
{
#define defMaxBoat 3 // maximum number of boats obtainable

class CBoat // boat information class
{
public:
	CBoat();
	bool Init( int n, CForm* ship, GuiDragInGridEvent evt );

	void		Reset(); // resets the boat
	void		Link( CCharacter* pBoat )	{ _chtBoat = pBoat;			}  // the boat character corresponds to the character character
	bool		GetIsValid()				{ return _chtBoat!=NULL;	}  // gets the character information
	void		UnLink()					{ _chtBoat=NULL;			}  

	CCharacter* GetCha()			{ return _chtBoat;		}  // finds the character
	CGoodsGrid*	GetGoodsGrid()		{ return _grdHold;		}  // gets the character item
	CForm*		GetForm()			{ return _frmShipRoom;	}  // looks up the boat item slot information

private:
	static void _evtHoldGridRMouse(CGuiData *pSender,CCommandObj* pItem,int nGridID);
	static void _evtEscClose( CForm* pForm );	// ESC close window event  add by Philip.Wu  2006-06-22

private:
	int				_nIndex;

	CCharacter*		_chtBoat;
	CForm*			_frmShipRoom;
	CGoodsGrid*		_grdHold;

};

class CBoatMgr : public CUIInterface
{
public:
	void	Clear();

	bool	AddBoat( CCharacter* pBoat );  // the user adds a boat
	CBoat*	GetBoat( unsigned int n )		{ return &_cBoats[n];	} // gets the boat
	CBoat*	FindBoat( unsigned int ulWorldID );  // finds every boat of the user

	void	SetHuman( CCharacter* p )		{ _pHuman = p;			} 
	CCharacter*		GetHuman()				{ return _pHuman;		} // looks up the current character

	CGoodsGrid*		FindGoodsGrid( unsigned int ulWorldID );	// looks up the matching item slot by ID
	CCharacter*		FindCha( unsigned int ulWorldID );         // looks up the boat owner
	CCharacter*		FindCha( CGoodsGrid* pGoods );   // finds the boat owner by matching equipment

	CCharacter*		ChangeMainCha( unsigned int ulWorldID );	// switches the main character

	CBoat*	GetOtherBoat()					{ return &_cOther;		}  // views another player's boat

protected:
	virtual bool Init(); //user boat information initialisation
    virtual void End(); 
	virtual void SwitchMap()				{	Clear();			} // clears the boat information when jumping to another map

private:
	CBoat*		GetFreeBoat(); // obtains a free boat

private:
	CCharacter*	_pHuman;
	CBoat		_cBoats[defMaxBoat];        // number of boats the current character may own
	CBoat		_cOther;					// a temporary variable used to view another player's cargo hold and goods

};

}
