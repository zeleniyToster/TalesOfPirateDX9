
#include "stdafx.h"
#include "GameWG.h"
#include "PacketCmd.h"

#include <windows.h>
#include <process.h>
#include <tlhelp32.h>


CGameWG::CGameWG(void)
	: m_hThread(0)
{
}


CGameWG::~CGameWG(void)
{
	SafeTerminateThread();

	m_lstModule.clear();
}


// refreshes the modules in the current process
bool CGameWG::RefreshModule(void)
{
    bool          bRet        = false;

	try
	{
		MODULEENTRY32 me32        = {0};
		std::string   strModule;

		std::unique_ptr<void, decltype(&CloseHandle)> hModuleSnap(CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ::GetCurrentProcessId()), &CloseHandle);
		if (hModuleSnap.get() == INVALID_HANDLE_VALUE)
			return false;

		me32.dwSize = sizeof(MODULEENTRY32); 

		if(Module32First(hModuleSnap.get(), &me32)) 
		{
			// walks every module in the current process
			do
			{
				strModule = me32.szModule;
				m_lstModule.insert(strModule);
			}
			while(Module32Next(hModuleSnap.get(), &me32));

			bRet = true;
		}
		else
		{
			// enumeration failed
			bRet = false;
		}
	}
	catch(...)
	{
		
	}

    return bRet;
}


// the 'Pirate Angel' cheat was used
bool CGameWG::IsUseHdts(void)
{
	if (m_lstModule.contains("hookit.dll"))
	{
		return true;
	}

	return false;
}


// starts the thread
void CGameWG::BeginThread(void)
{
    m_hThread = (HANDLE)_beginthreadex(0, 0, Run, this, 0, 0);
}


// terminates the thread safely
void CGameWG::SafeTerminateThread()
{
	if(m_hThread)
	{
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);

		m_hThread = 0;
	}
}


// thread callback
UINT CALLBACK CGameWG::Run(void* param)
{
	CGameWG* pGameWG = (CGameWG*)(param);

	for(;;)
	{
		Sleep(60 * 1000);	// refreshed once a minute

		if(! g_NetIF || ! g_NetIF->IsConnected())
		{
			// not connected to the network
			continue;
		}

		if(! pGameWG->RefreshModule())
		{
			// refreshes the module list
			continue;
		}

		if(pGameWG->IsUseHdts())
		{
			// the 'Pirate Angel' cheat was used

			CS_ReportWG(g_oLangRec.GetString(143));
			break;
		}

	}

	return 0;
}

