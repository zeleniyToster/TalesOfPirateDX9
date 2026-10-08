#pragma once

#include "udpsocket.h"
#include <vector>
#include <map>

namespace client_udp
{
enum
{
	GATE_NAME_MAX = 16,
	NAME_MAX = 32,
};

class	CGroupInfo;
class	CRegionInfo;
class	CRegionMgr;

class	CGateInfo
{
public:
	CGateInfo( CGroupInfo* pGroup, const char* name );
	void			Start();			// initialises the socket and fetches the data
	void			Exit();				// closes the socket
	CGroupInfo*		GetOwner()	{ return pOwner;			}
	bool			Read();
	bool			Send();
	CUdpClient&		GetUdp()	{ return Socket;			}

	char			szName[GATE_NAME_MAX];
	int				nGateNum;

private:
	CUdpClient		Socket;
	CGroupInfo*		pOwner;

};

class	CGroupInfo
{
public:
	CGroupInfo( CRegionInfo* pRegion, const char* name );
	void			Add( CGateInfo* pGate )		{ if( pGate ) Gates.push_back( pGate );		}
	CGateInfo*		Find( const char* Name );
	CRegionInfo*	GetOwner()	{ return pOwner;		}
	int				GetNum();
	CGateInfo*		GetMinGate();		// gets the Gate with the smallest load

	void			Start();
	void			Exit();
	void			Send();

	char			szName[NAME_MAX];
	int				nShowMin;
	int				nShowMax;

private:
	typedef std::vector<CGateInfo*> gates;
	gates			Gates;
	CRegionInfo*	pOwner;

};

class	CRegionInfo
{
public:
	CRegionInfo( CRegionMgr* Mgr, const char* name );
	CGroupInfo*		Find( const char* Name );
	void			Add( CGroupInfo* pGroup )	{ if( pGroup ) Groups.push_back( pGroup );	}
	void			Start();
	void			Exit();
	void			Send();
	CRegionMgr*		GetOwner()	{ return pOwner;		}

	char			szName[NAME_MAX];

private:
	typedef std::vector<CGroupInfo*> groups;
	groups			Groups;
	CRegionMgr*		pOwner;

};

typedef void (*GateChangeEvent) ( CGateInfo* pGate );

// used for sending on the client
class CRegionMgr
{
public:
	CRegionMgr();
	void	Init( HWND hWnd, UINT uRegeditNetMes );

	bool	Add( const char* szRegion, const char* szGroup, const char* szGate );	// adds the base data

	bool	EnterRegion( const char* szRegion );	// has entered a zone and is ready to fetch the data
	int		GetGroupNum( const char* szGroup );		// gets the number of people in a Group of the current zone; -1 on failure
	bool	ExitRegion();							// leaves the current Group
	CRegionInfo*	GetActiovRegion()	{ return _pRegion;		}

	void	Exit();

	bool	OnMessage( UINT Message, WPARAM wParam, LPARAM lParam );

	void	FrameMove( DWORD dwTime );

public:		// list operations
	CRegionInfo*	Find( const char* Name );
	void			Add( CRegionInfo* pRegion )		{ if( pRegion ) _Regions.push_back( pRegion );	}

	void			GateCreateSocketCallBack( CGateInfo* pGate );
	void			GateCloseSocketCallBack( CGateInfo* pGate );

public:
	GateChangeEvent	evtGateChanteEvent;

private:
	typedef std::vector<CRegionInfo*> regions;
	regions			_Regions;			// every zone

	CRegionInfo*	_pRegion;			// the currently active zone; may be empty
	HWND			_hWnd;
	UINT			_uNetMessage;

	typedef std::map<SOCKET, CGateInfo*> gates;
	gates			_gates;				// used to look up CGateInfo quickly by socket

	DWORD			_dwLastSendTime;

};

extern CRegionMgr* GetRegionMgr();

}