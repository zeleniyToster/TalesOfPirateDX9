// Logvwr.cpp : defines the class behaviour for the application
//

#include "stdafx.h"
#include "Logvwr.h"
#include "LogvwrDlg.h"

#include "LogUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CResourceBundleManage  g_ResourceBundleManage;

// CLogvwrApp

BEGIN_MESSAGE_MAP(CLogvwrApp, CWinApp)
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLogvwrApp construction

CLogvwrApp::CLogvwrApp()
    {
    // TODO: add the construction code here,
    // put all important initialisation in InitInstance
    }


// the one and only CLogvwrApp object

CLogvwrApp theApp;

// CLogvwrApp initialisation

BOOL CLogvwrApp::InitInstance()
    {
    // make sure only one instance runs
    HANDLE hPrevMutex;
    HANDLE hMutex;
    LPCTSTR pszMutex = "mUtEx Of kOp";

    hPrevMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, pszMutex);
    if (hPrevMutex)
        {
        ::CloseHandle(hPrevMutex);
        HWND hWnd = ::FindWindow(NULL, "kIng Of pIrAtEs lOgvIEwEr");
        if (hWnd)
            ::ShowWindow(hWnd, SW_SHOWMAXIMIZED);
        return FALSE;
        }

    // create the mutex
    hMutex = ::CreateMutex(NULL, 0, pszMutex);

    // if the application manifest on Windows XP declares that the app uses
    // ComCtl32.dll version 6 or later to enable visual styles,
    //则需要 InitCommonControls()。否则，将无法创建窗口。
    InitCommonControls();

    CWinApp::InitInstance();

    if (!AfxSocketInit())
        {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
        }

    AfxEnableControlContainer();

	// Add by lark.li
	if(!g_ResourceBundleManage.Init("E:\\Item\\Source\\VC2003\\Unicode\\Project_Kop\\vss_cs\\client\\bin\\system","zh_Hans_CN"))
	//if(!g_ResourceBundleManage.Init(".","zh_Hans_CN"))
	{
		AfxMessageBox("Cloud't load resource!\n");
		return -1;
	}
	else
	{
		printf(RES_STRING(VW_LOGVWR_CPP_00001));
	}

    // standard initialisation
    // if these features are unused and you want to shrink the final executable,
    // remove the following unneeded specific initialisation routines
    //
    // change the registry key used to store the settings
    // TODO: this string should be changed appropriately,
    // for example to your company or organisation name
    //SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
    if (!::mus_mgr_init(true))
        {
        AfxMessageBox("mus_mgr_init failed!");
        return FALSE;}

    CLogvwrDlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK)
        {
        // TODO: put the code that handles closing the dialog with OK here
        //
        }
    else if (nResponse == IDCANCEL)
        {
        // TODO: put the code that handles closing the dialog with Cancel here
        //
        }

    // the dialog has been closed, so return FALSE to quit the application
    // rather than starting the application's message pump.
    ::mus_mgr_exit();
    ::CloseHandle(hMutex);
    return FALSE;
    }
