#pragma once

#include "Scene.h"
#include "uiguidata.h"
#include "NetProtocol.h"

class CSceneObj;
namespace GUI
{
    class CForm;
    class C3DCompent;
    class CGuiData;
    class CLabelEx;
    class CEdit;
    class CTextButton;
    class CList;
    class CMemo;
    class CLabel;
}

/* extern char autoLoginChar[32] ;
extern bool canAutoLoginChar; */
/**
 * 摘要：本类主要处理登陆流程中的选择角色场景,包括场景动画和GUI表单.
 * 　　　本类继承自CGameScene类.
 *
 * @author: Michael Chen
 * @date: 2005-04-26
 */
class CSelectChaScene : public CGameScene
{
public:
    CSelectChaScene(stSceneInitParam& param);
    ~CSelectChaScene();

    virtual void        LoadingCall();          // refreshed after loading finishes
    virtual void		SetMainCha(int nChaID);

    static CSelectChaScene& GetCurrScene();

    //deletes the currently selected character
    void                DelCurrentSelCha();
    //shows the existing characters
	bool                SelectCharacters(std::span<const NetChaBehave> characters);
    //adds a character
    bool                CreateCha(const std::string& sName, int nChaIndex, stNetChangeChaPart* part);

    void                SelectChaError( int error_no, const char* error_info );

    //sends the delete character message to the server
    void SendDelChaToServer(const char szPassword2[]);

	// updates the button states
    void UpdateButton();

	// gets the number of characters
	int GetChaCount();

	void ShowWelcomeNotice(bool bShow = true);

	void UpdatePageLabel();
	int GetMaxPage() const;
protected:
    //scene related functions
    virtual void		_FrameMove(DWORD dwTimeParam);
    virtual void		_Render();

    virtual bool		_MouseButtonDown(int nButton);
    virtual bool		_MouseButtonDB(int nButton);
	virtual void		_KeyDownEvent( int key );


    virtual bool		_Init();	
    virtual bool		_Clear();
    virtual void        _RenderUI();

    //UI related functions
    bool                _InitUI();
	static void			_OnClickUpdatePass(CGuiData *pSender, int x, int y, DWORD key);
    static void         _SelChaFrmMouseEvent(CCompent *pSender, int nMsgType, 
                                             int x, int y, DWORD dwKey);

//	static void         _CheckFrmMouseEvent(CCompent *pSender, int nMsgType, 
//                                           int x, int y, DWORD dwKey);

    static void         _evtCreateDoublePwdEvent(CCompent *pSender, int nMsgType, 
                                            int x, int y, DWORD dwKey);		// asks whether to create a secondary password

    static void         _evtWelcomeNoticeEvent(CCompent *pSender, int nMsgType, 
                                            int x, int y, DWORD dwKey);		// welcome screen event handling

    static void         _evtCreateOKNoticeEvent(CCompent *pSender, int nMsgType, 
                                            int x, int y, DWORD dwKey);		// first character creation success prompt screen event handling

    static void         _evtChaNameAlterMouseEvent(CCompent *pSender, int nMsgType, 
                                            int x, int y, DWORD dwKey);		// first character creation success prompt screen event handling

	//~ logic functions: =================================================================


    //sends the start game message to the server
    void SendBeginPlayToServer();

    void SetChaDark(CCharacter* pCha);

private:
	void UpdateCharacterPositions();


	struct ChaFont
	{
		CCharacter* pCha{};
		int iLevel{ -1 };
		std::string sProfession;
		int iPos{ -1 };
		int iFontX{ -1 };
		int iFontY{ -1 };
		bool isOnCurPage{ false };
	};
	typedef std::vector<ChaFont> ChaFontPtrContainer;
    typedef ChaFontPtrContainer::iterator ChaFontPtrContainerIter;

    ChaFontPtrContainer m_CharactorPtrs;        //Characters in the scene (including fonts)
    std::vector<int>         m_XPositions;           //Scene X coordinates of the three characters in the scene
    std::vector<int>         m_YPositions;           //Y coordinates of the three characters in the scene
    std::vector<int>         m_ZPositions;
    std::vector<int>         m_Yaws;                 //The rotation of the characters in the scene
	std::vector<uint8_t> m_FreePositions;        //The index of the empty position of the scene
    BYTE                m_ChaColors[3][3];      //the colour of the character
	int					m_nChaRotates[3];       //the offset angle of the character (-180~+180)

    //scene rendering and animation related
    BYTE            _loadtex_flag;          //stores the scene rendering and animation settings
    BYTE            _loadmesh_flag;         //stores the scene rendering and animation settings

    CSceneObj*	    pObj;                   //scene object
    lwIPrimitive*   pAure[3];


    //UI
    CForm*			frmSelectCha; 
    CTextButton*    btnDel;
    CTextButton*    btnYes;
    CTextButton*    btnCreate;
    CTextButton*    btnExit;
	CTextButton*    btnAlter;
	CTextButton*    btnChangePassConf;
    CTextButton*    btnPrevPage;
    CTextButton*    btnNextPage;
    CLabel*         labPage;

    CForm*          frmCheck;

    //logic
public:
	uint8_t         m_nCurChaIndex{ static_cast<decltype(m_nCurChaIndex)>(-1) };
	uint8_t             m_iCurPage;
	uint8_t         m_MaxCharacters{};
private:
    //
    bool            m_isInit;

	bool			m_isCreateCha;

	CForm*			frmWelcomeNotice;	// defines the welcome screen; it appears only when the current account has no character
	CForm*			frmCreateOKNotice;	// defines the first character creation success prompt screen; it is shown only once this account has gone through the creation flow for its first character
	CForm*			frmChaNameAlter;	// defines the rename screen; it is only valid for the Taiwan version, please back up your work
};
