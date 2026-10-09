// RoleData.h Created by knight-gongjian 2004.11.22.
//---------------------------------------------------------
#pragma once

#ifndef _ROLEDATA_H_
#define _ROLEDATA_H_


#include "GameAppNet.h"
#include "GameCommon.h"
#include "TryUtil.h"
//---------------------------------------------------------

namespace mission
{

	// mission function item command structure
	typedef struct _ROLE_ITEM_COMMAND
	{
		USHORT usCmd;								// function item command
		char   szDesc[ROLE_MAXNUM_FUNCITEMSIZE];	// description information
		BYTE   byParam;								// parameter information
		WORD   wParam;
		DWORD  dwParam;

		union
		{
			struct 
			{
				DWORD  dwParam1;
				DWORD  dwParam2;
			};
			char szParam[16];
		};

	} ROLE_ITEM_COMMAND, *PROLE_ITEM_COMMAND;

	// shows the mission dialog description information
	typedef struct _ROLE_DESCRIPTION
	{
		BYTE byCmd;							// dialog button command ID for the description
		char szDesc[ROLE_MAXNUM_DESPSIZE];	// description information

	} ROLE_DESCRIPTION, *PROLE_DESCRIPTION;

	//　shows the mission function description information
	typedef struct _ROLE_FUNCTION
	{		
		BYTE byCount;		// function item count
		ROLE_ITEM_COMMAND FuncItem[ROLE_MAXNUM_FUNCITEM]; // the corresponding function item

	} ROLE_FUNCTION, *PROLE_FUNCTION;

	// NPC information storage structure
	// structure for a single history mission tag
	typedef struct _ROLE_FLAGINFO
	{
		WORD  wRoleID;
		WORD  wParam;
		char  szFlag[ROLE_MAXNUM_FLAGSIZE];

		_ROLE_FLAGINFO()
		{
			Clear();	
		}

		void Clear()
		{
			wRoleID = 0;
			wParam  = 0;
			memset( szFlag, 0, ROLE_MAXNUM_FLAGSIZE*sizeof(char) );
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE ) return FALSE;
			memcpy( szFlag, pBuf, dwSize );
			return TRUE;
		}

		BOOL IsValid( WORD wFlag )
		{
			return wFlag < ROLE_MAXNUM_FLAGSIZE<<3;
		}

		BOOL IsSet( WORD wFlag )
		{
			if( wFlag >= ROLE_MAXNUM_FLAGSIZE<<3 ) return FALSE;
			DWORD dwIndex = wFlag>>3;// wFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(wFlag%ROLE_MAXNUM_INDEXSIZE);
			return szFlag[dwIndex] & szTag;
		}

		// sets or clears the given tag
		BOOL SetFlag( WORD wFlag, BOOL bValid )
		{
			if( wFlag >= ROLE_MAXNUM_FLAGSIZE<<3 ) return FALSE;
			DWORD dwIndex = wFlag>>3;// wFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(wFlag%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szFlag[dwIndex] |= szTag;
			}
			else 
			{
				szFlag[dwIndex] &= ~szTag;
			}

			return TRUE;
		}

	} ROLE_FLAGINFO, *PROLE_FLAGINFO;

	typedef struct _ROLE_EXFLAGINFO
	{
		DWORD dwRoleID;
		WORD  wParam;
		char  szFlag[ROLE_MAXNUM_FLAGSIZE];
		char  szParam[ROLE_MAXNUM_FLAGSIZE*4];

		_ROLE_EXFLAGINFO()
		{
			Clear();
		}

		void Clear()
		{
			dwRoleID = 0;
			memset( szFlag, 0, ROLE_MAXNUM_FLAGSIZE*sizeof(char) );
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE ) return FALSE;
			return TRUE;
		}

		BOOL IsValid( DWORD dwFlag )
		{
			return dwFlag < ROLE_MAXNUM_FLAGSIZE<<3;
		}

		BOOL IsSet( DWORD dwFlag )
		{
			DWORD dwIndex = dwFlag>>3; // dwFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwFlag%ROLE_MAXNUM_INDEXSIZE);
			return szFlag[dwIndex] & szTag;
		}

		// sets or clears the given tag
		void SetFlag( DWORD dwFlag, BOOL bValid )
		{
			DWORD dwIndex = dwFlag>>3; // dwFlag/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwFlag%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szFlag[dwIndex] |= szTag;
			}
			else 
			{
				szFlag[dwIndex] &= ~szTag;
			}

			dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// high four bits
				szParam[dwIndex] &= 0xF;
			}
			else
			{
				// low four bits
				szParam[dwIndex] &= 0xF0;
			}
		}

		// parameter information for setting the tag
		BOOL SetParam( DWORD dwFlag, BYTE byParam )
		{
			if( byParam >= ROLE_MAXVALUE_PARAM )
				return FALSE;

			DWORD dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// high four bits
				szParam[dwIndex] &= 0xF;
				szParam[dwIndex] |= byParam<<4;
			}
			else
			{
				// low four bits
				szParam[dwIndex] &= 0xF0;
				szParam[dwIndex] |= byParam;
			}
			return TRUE;
		}

		BYTE GetParam( DWORD dwFlag )
		{
			DWORD dwIndex = dwFlag>>1;
			DWORD dwTag = dwFlag&1;
			if( dwTag == 0 )
			{
				// high four bits
				return (szParam[dwIndex]&0xF0)>>4;
			}
			else
			{
				// low four bits
				return szParam[dwIndex]&0xF;
			}

			return TRUE;
		}
		
	} ROLE_EXFLAGINFO, *PROLE_EXFLAGINFO;

	// structure holding the history mission tags executable by all characters
	typedef struct _ROLE_RECORDINFO
	{
		char  szID[ROLE_MAXNUM_RECORDSIZE];

		_ROLE_RECORDINFO()
		{
			Clear();
		}

		BOOL Load( LPBYTE pBuf, DWORD dwSize )
		{T_B
			if( dwSize >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			memcpy( szID, pBuf, dwSize );
			return TRUE;
		T_E}

		void Clear()
		{T_B
			memset( szID, 0, ROLE_MAXNUM_RECORDSIZE*sizeof(char) );
		T_E}

		BOOL IsValid( DWORD dwID )
		{T_B
			return dwID < ROLE_MAXNUM_RECORDSIZE<<3;
		T_E}

		BOOL IsSet( DWORD dwID )
		{T_B
			if( dwID >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			DWORD dwIndex = dwID>>3; // dwID/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwID%ROLE_MAXNUM_INDEXSIZE);
			return szID[dwIndex] & szTag;
		T_E}

		// sets or clears the given tag
		BOOL SetID( DWORD dwID, BOOL bValid )
		{T_B
			if( dwID >= ROLE_MAXNUM_RECORDSIZE<<3 ) return FALSE;
			DWORD dwIndex = dwID>>3; // dwID/ROLE_MAXNUM_INDEXSIZE;
			char szTag = 1<<(dwID%ROLE_MAXNUM_INDEXSIZE);
			if( bValid )
			{
				szID[dwIndex] |= szTag;
			}
			else 
			{
				szID[dwIndex] &= ~szTag;
			}

			return TRUE;
		T_E}

	} ROLE_RECORDINFO, *PROLE_RECORDINFO;


	// trigger information record
	typedef struct _TRIGGER_DATA
	{
		WORD wTriggerID;	// trigger ID
		WORD wMissionID;	// mission ID (checked when a mission is cancelled, to clear that mission's triggers)
		BYTE byType;		// event type
		WORD wParam1;		// trigger parameter 3
		WORD wParam2;		// trigger parameter 4, action count
		WORD wParam3;		// (for a random mission this is the start position recorded in the tag, passed on to the trigger script)
		WORD wParam4;		// 
		WORD wParam5;
		WORD wParam6;

		BYTE byIsDel : 1;	// flags whether the trigger should be deleted
		BYTE byParam : 7;

	} TRIGGER_DATA, *PTRIGGER_DATA;

	enum { TRIGGER_VALID, TRIGGER_DELED };

	// trigger information record
	typedef struct _NPC_TRIGGER_DATA
	{
		WORD wTID;			// trigger ID
		WORD wParam;
		BYTE byType;		// event type
		WORD wParam1;		// trigger parameter
		WORD wParam2;
		WORD wParam3;
		WORD wParam4;

	} NPC_TRIGGER_DATA, *PNPC_TRIGGER_DATA;

	// random mission data information
	typedef struct _MISSION_DATA
	{
		WORD	wParam1;				// parameters of the random mission execution content information
		WORD	wParam2;
		WORD	wParam3;
		WORD	wParam4;
		WORD	wParam5;
		WORD	wParam6;

		void*	pData;					// runtime temporary data
	} MISSION_DATA, *PMISSION_DATA;

	typedef struct _MISSION_INFO
	{
		WORD	wRoleID;				// mission ID
		BYTE	byState : 3;			// mission progress state
		BYTE	byMisType : 5;			// mission class
		BYTE	byType;					// random mission information type
		BYTE	byLevel;				// level information
		BYTE	byNumData;				// execution content count
		DWORD	dwExp;					// experience obtainable by executing the mission
		DWORD	dwMoney;				// money reward for executing the mission
		WORD	wItem;					// advanced item reward of the random mission (a value of 0 means none; the quantity is always 1)
		WORD	wParam1;				// parameters
		WORD	wParam2;
		ROLE_FLAGINFO RoleInfo;			// flag and mission ID information
		MISSION_DATA RandData[ROLE_MAXNUM_RAND_DATA];	// random mission data information
		
	} MISSION_INFO, *PMISSION_INFO;

	// structure counting random mission completions
	typedef struct _RAND_MISSION_COUNT
	{
		WORD	wRoleID;				// mission ID
		WORD	wCount;					// mission completion count
		WORD	wNum;					// number of rounds completed

	} RAND_MISSION_COUNT, *PRAND_MISSION_COUNT;

	// NPC mission state record
	typedef struct _STATE_DATA
	{
		BYTE byID : 5;		// index of the mission information list carried by each NPC
		BYTE byState : 3;	// single mission state
	
	} STATE_DATA, *PSTATE_DATA;

	typedef struct _MISSION_STATE
	{
		DWORD dwNpcID;
		BYTE  byNpcState;
		BYTE  byMisNum;			// completed mission count
		STATE_DATA StateInfo[ROLE_MAXNUM_MISSIONSTATE];	// mission data information
	
	} MISSION_STATE, *PMISSION_STATE;
}

//---------------------------------------------------------

#endif // _ROLEDATA_H_