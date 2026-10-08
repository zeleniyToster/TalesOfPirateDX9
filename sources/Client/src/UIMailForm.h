#pragma once


#include "UIGlobalVar.h"
#include "uiform.h"
#include "uilabel.h"
#include "uiedit.h"
#include "uimemo.h"


namespace GUI
{

	class CMailMgr : public CUIInterface
	{
	public:
		CMailMgr(void);
		~CMailMgr(void);

		void ShowQuestionForm();
		void ShowAnswerForm(const char* szTitle, const char* szContent);

		void SubmitQuestion();

    protected:
        virtual bool Init();
        virtual void CloseForm();
		virtual void FrameMove(DWORD dwTime);

	private:

		// question form
		CForm*		frmQuestion;
		CEdit*		edtQuestionTitle;
		CMemo*		memCentent;
		static void _evtQuestionFormEvent(CCompent *pSender, int nMsgType, int x, int y, DWORD dwKey);

		// answer form
		CForm*		frmAnswer;
		CMemo*		memMiss;
	};

}
