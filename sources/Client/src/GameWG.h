
#pragma once

#include <unordered_set>
#include <string>


// game cheat class  add by Philip.Wu  2006-07-06
class CGameWG
{
public:
	CGameWG(void);
	~CGameWG(void);

	// refreshes the modules in the current process
	bool RefreshModule(void);

	// the 'Pirate Angel' cheat was used
	bool IsUseHdts(void);

	// starts the thread
	void BeginThread(void);

	// terminates the thread safely
	void SafeTerminateThread();

private:

	// holds the module list
	std::unordered_set<std::string> m_lstModule;

	// thread handle
	HANDLE m_hThread;

	// thread callback
	static UINT CALLBACK Run(void* param);

};

