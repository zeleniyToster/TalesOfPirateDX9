#pragma once
#include "UIGlobalVar.h"

#include "NetProtocol.h"

namespace GUI
{

	// item refining
	class CGuildChallengeMgr : public CUIInterface
	{
	public:
		void		Show(bool bShow = true);
		void		SetContent(const NET_GUILD_CHALLINFO& Info);
		void		ChallegeSuccess(long lChaID);
		void		ChallegeFailed(long lChaID);
		void		ChallegeOther(long lChaID);

	protected:
		virtual bool Init();

		bool        SendChallegeProtocol(int iIndex, long lMoney);
		void		ChargeMoney(int iIndex);

	private:
		static void _FirstChallengeButtonDown(CGuiData *pSender, int x, int y, DWORD key);
		static void _SecondChallengeButtonDown(CGuiData *pSender, int x, int y, DWORD key);
		static void _ThirdChallengeButtonDown(CGuiData *pSender, int x, int y, DWORD key);
		static void _enterChargeMoney(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);
		static void _ChargeEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

	private:
		void		ClearUI();
		bool		IsValid(int iIndex, long lMoney);
		int			GetChallengeMasterIndex(const char * szName);

	private:
		enum {
			FIRST = 0,
			SECOND = 1,
			THIRD = 2,
			NUM = 3,
		};
		const static int CHARGE_MONEY = 50000;
		const static int FIRST_CHARGE_MONEY[NUM];

		CForm		*frmGuildPK;
		CLabel		*labChallenger[NUM];	// the challenger
		CLabel		*labMoney[NUM];			// bidding
		CLabel		*labGuildName[NUM];		// name of the guild holding the title
		CTextButton	*btnCharge[NUM];		// the bid button
		CList		*lstGuildPK;			// other information

		int			m_iSelIndex;
		int			m_iChangeMoney;
		long		m_lChargeMoney[NUM];
		bool		m_bStart[NUM];
	};

}

