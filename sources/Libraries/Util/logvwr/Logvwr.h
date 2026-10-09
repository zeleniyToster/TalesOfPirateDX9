// Logvwr.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"       // primary symbols


// CLogvwrApp:
// see Logvwr.cpp for the implementation of this class
//

class CLogvwrApp : public CWinApp
    {
public:
    CLogvwrApp();

    // overrides
public:
    virtual BOOL InitInstance();

    // implementation

    DECLARE_MESSAGE_MAP()
    };

extern CLogvwrApp theApp;

class CLogUtil;
extern CLogUtil theLog;
