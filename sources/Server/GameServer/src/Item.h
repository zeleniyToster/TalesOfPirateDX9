//=============================================================================
// FileName: Item.h
// Creater: ZhangXuedong
// Date: 2004.09.21
// Comment: CItem class
//=============================================================================

#ifndef ITEM_H
#define ITEM_H

#include "Character.h"
#include "ItemRecord.h"
#include "CompCommand.h"

#define defITEM_ON_TICK		3 * 60 * 1000

enum EItemProtType // item protection type
{
	enumITEM_PROT_OWN,	// anyone other than the owner
	enumITEM_PROT_TEAM,	// anyone outside the party
};

class CItem : public Entity
{
public:
	CItem();

	void	Initially();
	void	Finally();

	virtual void	Run(dbc::uLong ulCurTick);

	SItemGrid	*GetGridContent(void) {return &m_SGridContent;}
	void		SetFromID(dbc::Long lFromEntityID) {m_lFromEntityID = lFromEntityID;}
	void		SetSpawnType(dbc::Char chType) {m_chSpawType = chType;}
	void		SetStartTick(dbc::uLong ulTick) {m_ulStartTick = ulTick;}
	void		SetOnTick(dbc::uLong ulOnTick) {m_ulOnTick = ulOnTick;}
	void		SetProtOnTick(dbc::uLong ulProtOnTick) {m_ulProtOnTick = ulProtOnTick;}
	void		SetProtCha(dbc::uLong ulChaID, dbc::uLong ulChaHandle) {if (m_ulProtOnTick == 0) m_ulProtID = 0; else m_ulProtID = ulChaID, m_ulProtHandle = ulChaHandle;}
	dbc::uLong	GetProtChaID(void) {return m_ulProtID;}
	dbc::uLong	GetProtChaHandle(void) {return m_ulProtHandle;}
	void		SetProtType(dbc::Char chType = enumITEM_PROT_OWN) {m_chProtType = chType;}
	dbc::Char	GetProtType(void) {return m_chProtType;}

	char			chValid;
	CItemRecord		*m_pCItemRecord;

	SItemGrid		m_SGridContent;

protected:
	virtual CItem *IsItem(){return this;}

private:
	dbc::Char	m_chSpawType;
	dbc::Long	m_lFromEntityID;
	dbc::uLong	m_ulStartTick;
	dbc::uLong	m_ulOnTick;		// 0 means it is never destroyed

	dbc::Char	m_chProtType;	// protection type
	dbc::uLong	m_ulProtOnTick;	// protection time; 0 means permanent protection
	dbc::uLong	m_ulProtID;		// character ID that owns the item protection
	dbc::uLong	m_ulProtHandle;	// character handle that owns the item protection

	virtual void OnBeginSeen(CCharacter *pCCha);
	virtual void OnEndSeen(CCharacter *pCCha);
};

#endif // ITEM_H