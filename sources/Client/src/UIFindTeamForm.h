

#pragma once

#include "uiglobalvar.h"
#include "uiform.h"
#include "uiformmgr.h"
#include "uilabel.h"
#include "uitextbutton.h"
#include "uifastcommand.h"
#include <string>


namespace GUI
{

	class CFindTeamMgr : public CUIInterface
	{
	public:
		CFindTeamMgr();
		~CFindTeamMgr();

		void ShowFindTeamForm(bool bShow = true);
		bool IsShowFom(){return frmFindTeam->GetIsShow();}	//	Add by alfred.shi 20080902
		void SetFindTeamPage(int nCurPage, int nPageNum);
		void AddFindTeamInfo(int nSeq, const char* szName, long nLevel, long nJob, const char* szPlace);
		void RemoveTeamInfo();
		void SetOwnFindTeamState(bool bState);
		void FindTeamAsk(const char* szName);

		static const int FINDTEAM_PAGE_SIZE = 10;	// number of rows a team seeking page shows
		static const int FINDTEAM_INTERVAL  = 60;	// interval between two applications: 60 seconds

	protected:
		virtual bool Init();
		virtual void CloseForm();

	private:

		//
		// team finding
		//
		CForm*		frmFindTeam;

		CLabelEx*		labName[FINDTEAM_PAGE_SIZE];	// character name
		CLabelEx*		labLevel[FINDTEAM_PAGE_SIZE];	// character level
		CLabelEx*		labJob[FINDTEAM_PAGE_SIZE];		// character class
		CLabelEx*		labPlace[FINDTEAM_PAGE_SIZE];	// character location
		CTextButton*	btnSubmit[FINDTEAM_PAGE_SIZE];	// submit button

		CTextButton*	btnAddme;
		CTextButton*	btnDelme;
		CLabelEx*		labListPage;		// number of displayed pages

		int				m_nCurPage;			// current page
		int				m_nPageNum;			// total pages
		DWORD			m_dwLastTick;		// the time of the last party application
		std::string		m_strTeamLeader;	// the party leader applied to

		static void _evtFindTeamMouseButton(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);// mouse button click event
		static void	_evtFindTeamCheckEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey); // party application

	};

}

