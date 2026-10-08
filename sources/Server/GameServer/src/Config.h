#pragma once

#include <string>

#define MAX_GATE 5
#define MAX_MAP  100
#define MAX_MAPNAME_LENGTH	32

class CGameCommand
{
public:
	CGameCommand();
	bool	Load(const  char *pszFileName);
	void	SetDefault();

	// commands
	char	m_cMove[32];
	char	m_cMake[32];
	char	m_cNotice[32];
	char	m_cHide[32];
	char	m_cUnhide[32];
	char	m_cGoto[32];
	char	m_cKick[32];
	char	m_cMute[32];
	char	m_cReload[32];
	char	m_cRelive[32];
	char	m_cQcha[32];
	char	m_cQitem[32];
	char	m_cCall[32];
	char	m_cGamesvrstop[32];
	char	m_cUpdateall[32];
	char	m_cMisreload[32];
	char	m_cSummon[32];
	char	m_cSummonex[32];
	char	m_cKill[32];
	char	m_cAddmoney[32];
	char	m_cAddexp[32];
	char	m_cAttr[32];
	char	m_cItemattr[32];
	char	m_cSkill[32];
	char	m_cDelitem[32];
	char	m_cLuaall[32];
	char	m_cAddkb[32];
	char	m_cLua[32];
	char	m_cAddImp[32];
	char	m_cScrollNotice[32];
	char	m_cgenCharBag[32];
	char	m_cDistance[32];
};

class CGameConfig
{

public:	
	
    CGameConfig();
	
	bool	Load(char *pszFileName);
	void	SetDefault();
	bool	Reload(char *pszFileName);
	bool	LoadCmd(char *pszFileName);
	
public:

	// note: do not place any variable of non-fixed memory size here, such as templates or strings,
	// those are written to and read from disk verbatim
	char    m_szGateIP[MAX_GATE][64];  // Gate IP address list
    int     m_nGatePort[MAX_GATE];     // Gate port list
    int     m_nGateCnt;                // number of Gates
    char    m_szInfoIP[64];            // InfoServer IP
    int     m_nInfoPort;               // InfoServer Port
	char	m_szInfoPwd[33];		   // InfoServer verification code
	int		m_nSection;				   // shard number
    char    m_szMapList[MAX_MAP][MAX_MAPNAME_LENGTH];
	BYTE	m_btMapOK[MAX_MAP];		   // whether the map initialised successfully
	int     m_nMapCnt;				   // number of maps
	char	m_szEqument[MAX_MAPNAME_LENGTH];   // world guardian
    char    m_szName[64];			   // server name
	char	m_szDBIP[64];			   // DB IP
	char	m_szDBUsr[32];			   // DB user name
	char	m_szDBPass[32];			   // DB password

	// Add by lark.li 20080321 begin
	char	m_szTradeLogDBIP[64];			   // DB IP
	char	m_szTradeLogDBName[32];			   // DB IP
	char	m_szTradeLogDBUsr[32];			   // DB user name
	char	m_szTradeLogDBPass[32];			   // DB password

	BOOL	m_bTradeLogIsConfig;
	// End
	char	m_szDBName[32];            // kong@pkodev.net 09.22.2017

	long	m_lSocketAlive;            // socket keepalive
	int		m_nMaxPly;                 // maximum number of players
	int		m_nMaxCha;                 // maximum number of characters
	int		m_nMaxItem;                // maximum number of items
	int		m_nMaxTNpc;                // maximum number of dialog NPCs
	unsigned long	m_ulBaseID;        // server ID base
	long	m_lItemShowTime;           // item lifetime
	long	m_lItemProtTime;           // item protection time
	long	m_lSayInterval;            // shout interval
	char	m_szResDir[255];		   // directory holding the runtime resources
	char	m_szLogDir[255];		   // directory holding the log
	char	m_chMapMask;               // whether the big map is saved and loaded
	long	m_lDBSave;                 // interval between periodic database saves

	BOOL	m_bLogAI;				   // whether AI logging is on
	BOOL	m_bLogCha;				   // whether character logging is on
	BOOL	m_bLogCal;				   // whether value calculation logging is on
	BOOL	m_bLogMission;			   // whether Mission logging is on

	BOOL	m_bSuperCmd;
	
	// Add by lark.li 20080731 begin
	std::vector<int>	m_vGMCmd;
	// End
	short	m_sGuildNum;
	short	m_sGuildTryNum;
	BOOL	m_bOfflineMode;
	BOOL	m_bDiscStall;
	BOOL	m_bBlindChaos;
	char	m_szChaosMap[32];
	DWORD	m_dwStallTime;		
	
	BOOL	m_bLogDB;				   // whether player actions are recorded in the database
	BOOL    m_bInstantIGS;

	long	m_lWeather;

	unsigned char m_cSaveState[32];
};

#define	defCONFIG_FILE_NAME_LEN	512
extern char	szConfigFileN[defCONFIG_FILE_NAME_LEN];

extern CGameConfig g_Config;
extern CGameCommand g_Command;

