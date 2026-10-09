// npc.h Created by knight-gongjian 2004.11.19.
//---------------------------------------------------------
#pragma once

#ifndef _NPC_H_
#define _NPC_H_

#include "RoleData.h"
#include "Character.h"
//---------------------------------------------------------

class CNpcRecord;
class CChaRecord;

namespace mission
{
	using namespace dbc;

	#define EN_OK						 0		// correct
	#define EN_FAILER					-1		// error
	#define INVALID_SCRIPT_NPCHANDLE	USHORT(-1)		// invalid NPC script ID

	//	
	class CNpc : public CCharacter
	{
	public:
		enum NPC_TYPE { NPC, TALK, TRADE, TRADE_AGENCY, ROLE, FIGHT, EUDEMON };

		CNpc();
		virtual ~CNpc();

		// class type information
		virtual CNpc* IsNpc() { return this; }
		virtual void SetType() { m_byType = NPC; }
		BYTE GetType() { return m_byType; }
		BYTE GetShowType() { return m_byShowType; }
		
		// loads the NPC configuration information
		virtual BOOL Load( const CNpcRecord& recNpc, const CChaRecord& recChar );

		// network message handler
		virtual HRESULT MsgProc( CCharacter& character, RPACKET& packet );

		// mission state handler
		virtual BOOL MissionProc( CCharacter& character, BYTE& byState );

		// determines the map attribute of the NPC
		virtual BOOL IsMapNpc( const char szMap[], USHORT sID );

		// adds trigger information
		virtual BOOL AddNpcTrigger( WORD wID, mission::TRIGGER_EVENT e, WORD wParam1, WORD wParam2, WORD wParam3, WORD wParam4 );

		// trigger event handling
		virtual BOOL EventProc( TRIGGER_EVENT e, WPARAM wParam, LPARAM lParam );

		//sets the resource IDs for the NPC script dialog and trade information
		virtual void	SetScriptID( USHORT sID ) { m_sScriptID = sID; }
		virtual USHORT	GetScriptID() { return m_sScriptID; }
		virtual void	SetNpcHasMission( BOOL bHasMission ) { m_bHasMission = bHasMission; }
		virtual BOOL	GetNpcHasMission() { return m_bHasMission; }
		virtual const char* GetInitFunc() { return m_szMsgProc; }

		// the NPC has been summoned
		virtual void	Summoned( USHORT sTime ) {};
		
		const char* GetNpcName() { return m_szName; }

	protected:
		//
		virtual void Clear();

		// NPC type information
		BYTE	m_byType;

		// different types shown on the client
		BYTE	m_byShowType;

		// NPC configuration table number
		USHORT	m_sNpcID;

		// NPC information structure
		char	m_szMsgProc[ROLE_MAXSIZE_MSGPROC];

		// NPC script information ID
		USHORT	m_sScriptID;

		// whether the NPC carries mission information
		BOOL	m_bHasMission;

		char m_szName[128];

	};

	class CTalkNpc : public CNpc
	{
	public:
		CTalkNpc();
		virtual ~CTalkNpc();

		// class type information
		virtual void SetType() { m_byType = TALK; }

		// loads the NPC configuration information
		virtual BOOL Load( const CNpcRecord& recNpc, const CChaRecord& recChar );
		
		// loads the script information
		virtual BOOL InitScript( const char szFunc[], const char szName[] );

		//
		virtual HRESULT MsgProc( CCharacter& character, RPACKET& packet );

		//
		virtual BOOL MissionProc( CCharacter& character, BYTE& byState );

		// determines the map attribute of the NPC
		virtual BOOL IsMapNpc( const char szMap[], USHORT sID );

		// adds trigger information
		virtual BOOL AddNpcTrigger( WORD wID, mission::TRIGGER_EVENT e, WORD wParam1, WORD wParam2, WORD wParam3, WORD wParam4 );

		// trigger event handling
		virtual BOOL EventProc( TRIGGER_EVENT e, WPARAM wParam, LPARAM lParam );

		// the NPC has been summoned
		virtual void Summoned( USHORT sTime );

	protected:
		// loads the NPC script and initialises the NPC information
		virtual BOOL Load( const char szNpcScript[] );
		
		// 
		virtual void Clear();

		// clears the trigger information
		void	ClearTrigger( WORD wIndex );

		// mission event handler
		void	TimeOut( USHORT sTime );

		// trigger information carried by the NPC
		BYTE				m_byNumTrigger;
		NPC_TRIGGER_DATA	m_Trigger[ROLE_MAXNUM_NPCTRIGGER];

		// appears when a character carrying that mission is seen
		USHORT m_sTime;		// remaining display time
		BOOL   m_bSummoned; // whether this is a summoned NPC
	};
	
	class CTradeNpc : public CTalkNpc
	{
	public:
		enum ITEMCOUNT_TYPE { SINGLE, MULTIPLE };
		CTradeNpc();
		virtual ~CTradeNpc();

		virtual void SetType() { m_byType = TRADE; }

		// trade function interface
		virtual BOOL Sale( CCharacter& character, RPACKET& packet );
		virtual BOOL Buy( CCharacter& character, RPACKET& packet );

	private:

	};

	class CTradeAgencyNpc : public CTalkNpc
	{
	public:
		CTradeAgencyNpc();
		virtual ~CTradeAgencyNpc();

		virtual void SetType() { m_byType = TRADE_AGENCY; }

	private:
		// fixed me to remove
		struct AGENCY_ITEM
		{
			USHORT sItemID;
			BYTE   byType;
			BYTE   byCount;
			DWORD  dwOwnerID;
		};

		// item equipment information
		AGENCY_ITEM	m_sItemList[ROLE_MAXNUM_CAPACITY];
		USHORT		m_sNumItem;
	};

	class CRoleNpc : public CTalkNpc
	{
	public:
		CRoleNpc();
		virtual ~CRoleNpc();

		virtual void SetType() { m_byType = ROLE; }
		
	private:
		// mission information
		USHORT	m_sRoleList[ROLE_MAXNUM_CAPACITY];
		USHORT	m_sNumRole;
	};

	extern CTalkNpc* g_TalkNpc;	// set the global NPC class pointer while initialising the NPC script
}

//---------------------------------------------------------

#endif // _NPC_H_
