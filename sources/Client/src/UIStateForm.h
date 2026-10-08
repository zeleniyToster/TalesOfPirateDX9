#pragma once
#include "UIGlobalVar.h"


#define bpHP 10
#define bpSP 10
#define bpMN 10
#define bpMX 10
#define bpASPD 10
#define bpDEF 10
#define bpPR 10
#define bpHR 10
#define bpFLEE 10
#define bpMSPD 10




namespace GUI
{
// player attributes
class CStateMgr : public CUIInterface
{
public:
	

	void			RefreshStateFrm();

protected:
	virtual bool	Init();
    virtual void	End();
	virtual void	FrameMove(DWORD dwTime);

private:
	static void _evtMainShow(CGuiData *pSender);
	static void MainMouseDown(CGuiData *pSender, int x, int y, DWORD key);

private:
	//the controls of the frmState screen
	CForm*			frmState;
	CLabelEx*		labName;                    //create character name
	CLabelEx*		labGuildName;               //create guild name
	CLabelEx*		labStateLevel;              //create level
	CLabelEx*		labStatePoint;              //create attribute points
	CLabelEx*  		labSkillPoint;              //create skill points
	CLabelEx*		labJobShow;					//class
	CLabelEx *      labFameShow;				//create reputation

	//6 basic attributes
	CLabelEx*		labStrshow;
	CLabelEx*		labDexshow;
	CLabelEx*		labAgishow;
	CLabelEx*		labConshow;
	CLabelEx*		labStashow;
	CLabelEx*		labLukshow;
	CLabelEx*		labSailLevel;
	CLabelEx*		labSailEXP;

	//8 additional attributes
	CLabelEx*		labMinAtackShow;
	CLabelEx*		labMaxAtackShow;
	CLabelEx*		labFleeShow;
	CLabelEx*		labAspeedShow;
	CLabelEx*		labMspeedShow;
	CLabelEx*		labHitShow;
	CLabelEx*		labDefenceShow;
	//CLabelEx*		labCriticalShow;
	//CLabelEx*		labMfShow;
	CLabelEx*		labPhysDefineShow;

	//6 basic attribute buttons
	CTextButton*   btnStr;						//create strength bar
	CTextButton*   btnAgi;						//create agility bar
	CTextButton*   btnCon;						//create constitution bar
	CTextButton*   btnSta;						//create spirit bar
	//CTextButton*   btnLuk;						//创建幸运栏
	CTextButton*   btnDex;						//create focus bar

	CLabelEx*		labStateEXP;
	CLabelEx*		labStateHP;
	CLabelEx*		labStateSP;

	CLabelEx*		labFameSho;              //create reputation
	CLabelEx*		labStateName;
	CLabelEx*		labStateJob;


};

}

