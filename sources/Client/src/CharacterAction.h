#pragma once

enum EActionNumber
{
    POSE_WAITING =       01,//	ordinary waiting
    POSE_SHOW =          02,//	cool pose
    POSE_SLEEP =         03,//	hibernation
    POSE_WAITING2 =      04,//	alert waiting
    POSE_RUN =           05,//	ordinary running
    POSE_RUN2 =          06,//	alert running
    POSE_ATTACK =        07,//	ordinary attack 1
    POSE_ATTACK1 =       8, //	ordinary attack 2
    POSE_ATTACK2 =       9, //	ordinary attack 3
    POSE_POWER_ATTACK =  10,//	heavy attack
    POSE_SKILL1 =        11,//	skill 1
    POSE_SKILL2 =        12,//	skill 2
    POSE_SKILL3 =        13,//	skill 3
    POSE_SKILL4 =        14,//	skill 4	011
    POSE_SKILL5 =        15,//	skill 5	012
    POSE_SEAT =          16,//	ordinary sitting
    POSE_DIE =           17,//	death (common)
    POSE_WAVE =          18,//	waving (common)
    POSE_CRY =           19,//	heavy crying (common)
    POSE_JUMP =          20,//	gleeful (common)
    POSE_ANGER =         21,//	angry (common)
    POSE_DARE =          22,//	taunting (common)
    POSE_GAY =           23,//	narcissistic (male common)
    POSE_EMOTE7 =        24,//	emote action 7
    POSE_EMOTE8 =        25,//	emote action 8
    POSE_EMOTE9 =        26,//	emote action 9
    POSE_EMOTE10 =       27,//	emote action 10
    POSE_MINE =          28,//	mining (scene interaction)
    POSE_COLLECT =       29,//	gathering (scene interaction)
    POSE_SEAT2 =         30,//	sitting on a stool (scene interaction)
    POSE_LEAN =          31,//	leaning against a wall (scene interaction)
    POSE_LEAN2 =         32,//	leaning against a tree (scene interaction)
    POSE_HAPPY =         33,//	character interaction (scene interaction)
    POSE_HAPPY7 =        34,//	scene interaction action 7
    POSE_HAPPY8 =        35,//	scene interaction action 8
    POSE_HAPPY9 =        36,//	scene interaction action 9
    POSE_HAPPY10 =       37,//	scene interaction action 10
    POSE_RESERVED1 =     38,//	spare action 1
    POSE_FALLDOWN =      39,//	spare action 2
    POSE_SKILL6 =        40,//	skill action 6
    POSE_SKILL7 =        41,//	skill action 7
    POSE_FLY_WAITING =   42,//	flight				//POSE_SKILL8 =        42,//	技能动作8	
    POSE_FLY_RUN =       43,//	flight movement			//POSE_SKILL9 =        43,//	技能动作9	
    POSE_FLY_SHOW =      44,//	cool pose in the air			//POSE_SKILL10 =       44,//	技能动作10	
    POSE_FLY_SEAT =      45,//	sitting suspended in the air			//POSE_SKILL11 =       45,//	技能动作11	
    POSE_SKILL12 =       46,//	skill action 12
    POSE_SKILL13 =       47,//	skill action 13
    POSE_SKILL14 =       48,//	skill action 14
    POSE_SKILL15 =       49,//	skill action 15
    POSE_SKILL16 =       50,//	skill action 16
    POSE_SKILL17 =       51,//	skill action 17
    POSE_SKILL18 =       52,//	skill action 18
    POSE_SKILL19 =       53,//	skill action 19
    POSE_SKILL20 =       54,//	skill action 20
};

struct SActionInfo
{
	short		m_sActionNO;	// action number
	//short		m_sStartFrame;	// 起始帧
	//short		m_sEndFrame;	// 结束帧

	//short		m_sKeyFrameNum;	// 关键帧个数
	//short		*m_sKeyFrame;	// 关键帧集
    lwPoseInfo info;
};

struct SCharacterAction
{
	short		m_iCharacterType;	// character type
	short		m_iMaxActionNum;	// maximum number of actions of the character (the highest action number read from the file)
	short		m_iActualActionNum;	// number of valid actions of the character (the number of actions read from the file)
	SActionInfo	*m_SActionInfo;		// single action information of the character
};

class	CGameCharacterAction
{
public:
	CGameCharacterAction();
	~CGameCharacterAction();

	bool		Init(const char* ptcsFileName);
	void		Free(void);

	void		GetMaxCharType(int *iMaxType) {*iMaxType = m_iMaxCharacterType;};
	void		GetActualCharType(int *iActualType) {*iActualType = m_iActualCharacterType;};
	bool		GetCharMaxActNum(int iCharType, int *iMaxActNum);
	bool		GetCharActualActNum(int iCharType, int *iActualActNum);
	bool		GetCharFrameInfo(int iCharType, int iActionNO, int *iStartFrame, int *iEndFrame);
	bool		GetCharAction(int iCharType, SCharacterAction *SCharAct);

protected:
	short		m_iMaxCharacterType;		// maximum number of character types (the highest character type number read from the file)
	short		m_iActualCharacterType;		// number of valid character types (the number of characters read from the file)
	SCharacterAction	*m_SCharacterAction;
};

long StringGet(char* out, long out_max, char* in, long* in_from, const char* end_list, long end_len);
void StringSkipCompartment(char* in, long* in_from, const char* skip_list, long skip_len);


extern CGameCharacterAction g_PoseData;
extern void InitPoseData();
extern bool GetCharacterAction(int nTypeID, SCharacterAction *SCharAct);
