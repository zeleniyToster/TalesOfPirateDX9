// WorldEudemon.h Created by knight-gongjian 2005.3.9.
//---------------------------------------------------------
#pragma once

#ifndef _WORLD_EUDEMON_H_
#define _WORLD_EUDEMON_H_

#include "Npc.h"
//---------------------------------------------------------

namespace mission
{
	class CWorldEudemon : public CNpc
	{
	public:
		CWorldEudemon();
		virtual ~CWorldEudemon();

		virtual void SetType() { m_byType = EUDEMON; }

		// network message handler
		virtual HRESULT MsgProc( CCharacter& character, RPACKET& packet );
		
		// loads the world guardian configuration information
		virtual BOOL Load( const char szMsgProc[], const char szName[], dbc::uLong ulID );

	private:
		// loads the script information
		virtual BOOL InitScript( const char szFunc[], const char szName[] );
		
	};

	// not used at the moment
	class CEudemonManager
	{
	public:
		CEudemonManager();
		~CEudemonManager();

		// loads the world guardian list and generates the world guardians
		BOOL	Load( const char szTable[] );

	private:
		CWorldEudemon m_EudemonList[ROLE_MAXNUM_EUDEMON];
		BYTE	m_byNumEudemon;
	};

	extern CWorldEudemon g_WorldEudemon;
}

//---------------------------------------------------------

#endif // _WORLD_EUDEMON_H_