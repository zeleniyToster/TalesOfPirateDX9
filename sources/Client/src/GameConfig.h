#pragma once

#include <string>

struct SPlaceCha 
{
	int nTypeID;
	int nX;
	int nY;
};

class CGameConfig
{
public:	
	CGameConfig();
	
	void	Load(const char *pszFileName); // read the configuration file
	void	SetDefault();  //default configuration
	void	SetMoveClient( bool v ); //whether it is synchronised with the client
	
public:

	// note: do not place any variable of non-fixed memory size here, such as templates or strings,
	// those are written to and read from disk verbatim
	BOOL		m_bAutoLogin;
	BOOL		m_bFullScreen;
	SPlaceCha	m_ChaList[20];
	int			m_nChaCnt;
	float		m_LightDir[3];
	float		m_LightColor[3];
    BOOL        m_bNoObj;
    BOOL        m_bEditor;
    BYTE        m_btScreenMode;

	int			m_iFogR;
	int			m_iFogG;
	int			m_iFogB;
	float		m_fExp2;


    int         m_nMaxChaType;
    int         m_nMaxSceneObjType;
    int         m_nMaxEffectType;
	int			m_nMaxResourceNum;
    int         m_nMaxItemType;
    BOOL        m_bEnableMusic;
	BOOL		m_bCheckOvermax;

	// network
    BOOL        m_nSendHeartbeat;       // how often a heartbeat is sent, in seconds; minimum 10s, 0 disables it
	DWORD		m_nConnectTimeOut;		// network connection timeout

    BOOL        m_bEnableLG;            // whether LG output is allowed
    BOOL        m_bEnableLGMsg;         // whether the LG-Box may pop up
	BOOL		m_bMThreadRes;			// multithreaded resource loading


	int		m_nCreateScene;

	//{lemon add@2004.9.16
	float	m_fCameraVel;
	float	m_fCameraAccl;

	//lemon add@2004.11.26, login only
	float	eyeX;
	float   eyeY;      
	float	eyeZ;      
	float   refX;      
	float   refY;      
	float   refZ;
	float   fov;
	//lemon add@2005.1.24
	float   fAspect;

	float	fnear;
	float	ffar;
	
    float   m_fLgtFactor;
    DWORD   m_dwLgtBkColor;

    // lh add@2004.12.8 used so a standalone game can still get a weapon
    int     nLeftHand;
    int     nRightHand;


	BOOL	m_bRenderSceneObj;
	BOOL    m_bRenderCha;
	BOOL    m_bRenderEffect;
	BOOL    m_bRenderTerrain;
	BOOL	m_bRenderUI;
	BOOL    m_bRenderMinimap;
	BOOL	m_bRender;

    DWORD   m_dwFullScreenAntialias;

	// parameters used when switching maps over the network
	DWORD	m_dwMaxCha;
	DWORD	m_dwMaxEff;
	DWORD	m_dwMaxItem;
	DWORD	m_dwMaxObj;

	char	m_szMD5Pass[48];	// the MD5 password entered

	bool	m_IsShowConsole;	// whether the console may be operated
	bool	m_IsTomServer;		// log in to the Tom server

	bool	m_IsMoveClient;		// whether the client walks without synchronisation

	char	m_szVerErrorHTTP[256];	// the web page called when the versions do not match

	bool	m_IsBill;

	bool	IsPower()		{ return m_IsShowConsole;	}		// in editor mode the console can be operated automatically

	bool	m_IsDoublePwd;	// whether there is a secondary password

	// Add by lark.li 20080429 for res
	char   m_szLocale[256];
	char   m_szResPath[256];

	char   m_szFontName1[50];
	char   m_szFontName2[50];
	// End

	// Add by lark.li 20080825 begin
	int		m_nMovieW;
	int		m_nMovieH;
	// End
};


extern CGameConfig g_Config;

