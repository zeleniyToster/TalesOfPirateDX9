// stdafx.h : include file for standard system include files,
// or for frequently used but rarely changed
// project specific include files

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // exclude rarely-used stuff from the Windows headers
#endif

// If you must use a platform older than the ones named below, change the definitions.
// See MSDN for the latest information on the values for the different platforms.
#ifndef WINVER              // Allow the specific features of Windows 95 and Windows NT 4 or later.
#define WINVER 0x0400       // Change to the appropriate value for Windows 98 and Windows 2000 and later.
#endif

#ifndef _WIN32_WINNT        // Allow the specific features of Windows NT 4 or later.
#define _WIN32_WINNT 0x0400     // Change to the appropriate value for Windows 98 and Windows 2000 and later.
#endif                      

#ifndef _WIN32_WINDOWS      // Allow the specific features of Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change to the appropriate value for Windows Me and later.
#endif

#ifndef _WIN32_IE           // Allow the specific features of Internet Explorer 4.0 or later.
#define _WIN32_IE 0x0400    // Change to the appropriate value for Internet Explorer 5.0 and later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS  // some CString constructors will be explicit

// turn off MFC's hiding of some common and frequently safely ignored warnings
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxmt.h>

#include <afxdtctl.h>       // MFC support for the Internet Explorer 4 common controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC support for the Windows common controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>        // MFC socket extensions

#include <vector>
#include <map>
#include <string>

// base id used when inserting LG menu items dynamically
enum {MY_LGMENU_BASE = 6000};
// base id used when creating the ListCtrl for an LG dynamically
enum {MY_LGLIST_BASE = 6500};
// base id used when inserting GPL menu items dynamically
enum {MY_GPLMENU_BASE = 7000};

#include "util.h"
#pragma comment(lib, "winmm.lib")

#include "ResourceBundleManage.h"			// Add by lark.li 20080228
#define RES_STRING(a) g_ResourceBundleManage.LoadResString("" #a  "")

//extern CResourceBundleManage  g_ResourceBundleManage; //Add by lark.li 20080128
