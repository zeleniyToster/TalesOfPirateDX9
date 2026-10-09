// LogvwrDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// the CLogvwrDlg dialog
class CLogPacket;
class CLogTypeData;
class CGplTypeData;
class CGplViewDlg;
class CLogvwrDlg : public CDialog
    {
    // construction
public:
    CLogvwrDlg(CWnd* pParent = NULL);   // standard constructor
	~CLogvwrDlg();

    // dialog data
    enum {IDD = IDD_LOGVWR_DIALOG};

	// pointer to the main menu object
	CMenu* m_pMainMenu;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


    // implementation
protected:
    HICON m_hIcon;

    // generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    // hand added message map functions
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

private:
	// context menu object
	CMenu m_PopMenu;

    // container for all display elements
    CStatic m_Container;

	// currently active window
	CWnd* m_pActiveWnd;

    // list showing all LOG entries
    CListCtrl m_LogList;

	// ListCtrl objects per log category, plus the category identifier strings
	std::vector<CLogTypeData*> m_LGData;

	// dialog showing all GPL entries
	CGplViewDlg* m_pGplDlg;

	// per category objects for the GPL entries
	std::vector<CGplTypeData*> m_GPLData;

	// test and clear buttons
    CButton m_btnTestLog;
    CButton m_btnClearLog;

    // synchronisation object guarding display, clearing and re-entrancy
    CCriticalSection m_CS;

    // LOG packet object
    CLogPacket* m_pLogPacket;

    // count of all packets received
    DWORD m_dwPacketCnt;

public:
    afx_msg void OnBnClickedClearlog();
    afx_msg void OnBnClickedTestlog();
	afx_msg void OnFileTest();
	afx_msg void OnViewAll();
	afx_msg void OnNMRclickLoglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkLoglist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnViewGpl();
	BOOL HaveThisLGType(const char* szTypeName, CLogTypeData** ppltd);
	BOOL HaveThisGPLType(const char* szTypeName, CGplTypeData** ppgtd);
	BOOL SwitchView(CWnd& ActiveWnd);
	BOOL SwitchView(CString& strActiveWnd);
    void ToggleTopMost();
	afx_msg void OnViewTest();
	afx_msg void OnViewClear();
	afx_msg void OnViewPlaymp3();
	afx_msg void OnViewStopmp3();
	afx_msg void OnViewPlaywave();
	afx_msg void OnTimer(UINT nIDEvent);
	};
