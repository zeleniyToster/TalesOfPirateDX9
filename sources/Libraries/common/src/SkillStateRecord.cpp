//=============================================================================
// FileName: SkillStateRecord.cpp
// Creater: ZhangXuedong
// Date: 2005.02.04
// Comment: Skill State Record
//=============================================================================

#include "SkillStateRecord.h"
using namespace std;
//---------------------------------------------------------------------------
// class CSkillStateRecord
//---------------------------------------------------------------------------
void CSkillStateRecord::RefreshPrivateData()
{
	_nActNum = 0;
	for (int i = 0; i < defSKILLSTATE_ACT_NUM; i++)
	{		
		if( nActBehave[i]!=0 )
		{
			_nActNum++;
		}
		else
		{
			break;
		}
	}
}

//---------------------------------------------------------------------------
// class CSkillStateRecordSet
//---------------------------------------------------------------------------
CSkillStateRecordSet * CSkillStateRecordSet::_Instance = NULL;

BOOL CSkillStateRecordSet::_ReadRawDataInfo(CRawDataInfo *pRawDataInfo, vector<string> &ParamList)
{   
	if(ParamList.size()==0) return FALSE;

	CSkillStateRecord *pInfo = (CSkillStateRecord*)pRawDataInfo;

	int m = 0, n = 0;
    string strList[80];
	string strLine;

	// id
	pInfo->chID = (char)pInfo->nID;
	// name
	strncpy(pInfo->szName, pInfo->szDataName, defSKILLSTATE_NAME_LEN);
	pInfo->szName[defSKILLSTATE_NAME_LEN - 1] = '\0';
	// interval between ticks, in seconds
	pInfo->sFrequency = Str2Int(ParamList[m++]);
	// script applying the state to the target
	strncpy(pInfo->szOnTransfer, ParamList[m++].c_str(), defSKILLSTATE_SCRIPT_NAME);
	pInfo->szOnTransfer[defSKILLSTATE_SCRIPT_NAME - 1] = '\0';
	// script for the use effect
	strncpy(pInfo->szAddState, ParamList[m++].c_str(), defSKILLSTATE_SCRIPT_NAME);
	pInfo->szAddState[defSKILLSTATE_SCRIPT_NAME - 1] = '\0';
	// script for the removal effect
	strncpy(pInfo->szSubState, ParamList[m++].c_str(), defSKILLSTATE_SCRIPT_NAME);
	pInfo->szSubState[defSKILLSTATE_SCRIPT_NAME - 1] = '\0';
	// replacement policy
	pInfo->chAddType = Str2Int(ParamList[m++]);
	// whether the client may cancel it
	pInfo->bCanCancel = Str2Int(ParamList[m++]) != 0 ? true : false;
	// whether the character can move
	pInfo->bCanMove = Str2Int(ParamList[m++]) ? true : false;
	// whether magic skills can be used
	pInfo->bCanMSkill = Str2Int(ParamList[m++]) ? true : false;
	// whether normal skills can be used
	pInfo->bCanGSkill = Str2Int(ParamList[m++]) ? true : false;
	// whether trading is allowed
	pInfo->bCanTrade = Str2Int(ParamList[m++]) ? true : false;
	// whether items can be used
	pInfo->bCanItem = Str2Int(ParamList[m++]) ? true : false;
	// whether the character is invulnerable
	pInfo->bCanUnbeatable = Str2Int(ParamList[m++]) ? true : false;
	// whether items can be used on the character
	pInfo->bCanItemmed = Str2Int(ParamList[m++]) ? true : false;
	// whether skills can be used on the character
	pInfo->bCanSkilled = Str2Int(ParamList[m++]) ? true : false;
	// whether the character is invisible
	pInfo->bNoHide = Str2Int(ParamList[m++]) ? true : false;
	// whether the character is force-revealed
	pInfo->bNoShow = Str2Int(ParamList[m++]) ? true : false;
	// whether items can be manipulated
	pInfo->bOptItem = Str2Int(ParamList[m++]) ? true : false;
	// whether items can be manipulated
	pInfo->bTalkToNPC = Str2Int(ParamList[m++]) ? true : false;
	// id of the effect that is removed
	pInfo->bFreeStateID = Str2Int(ParamList[m++]);

	// screen effect
	pInfo->chScreen = Str2Int(ParamList[m++]);
	// spawn presentation
	memset( pInfo->nActBehave, 0, sizeof(pInfo->nActBehave) );
	strLine = ParamList[m++];
	n = Util_ResolveTextLine(strLine.c_str(), strList, 80, ',');
	n = n > defSKILLSTATE_ACT_NUM ? defSKILLSTATE_ACT_NUM : n;	
	for (int i = 0; i < n; i++)
	{		
		pInfo->nActBehave[i] = Str2Int(strList[i]);
	}

	// client presentation flag
    pInfo->sChargeLink = Str2Int(ParamList[m++]);

    // lasting ground effect
    pInfo->sAreaEffect = Str2Int(ParamList[m++]);
	// ground state effect; whether only the centre point is drawn
    pInfo->IsShowCenter = Str2Int(ParamList[m++]) ? true : false;
	// pose of the entity the effect is applied to
	pInfo->IsDizzy = Str2Int(ParamList[m++]) ? true : false;
	// effect presentation for the entity the effect is applied to
	pInfo->sEffect = Str2Int(ParamList[m++]);
	// dummy point used to play the effect on the affected player
	pInfo->sDummy1 = Str2Int(ParamList[m++]);
	// hit effect shown while in this state
	pInfo->sBitEffect = Str2Int(ParamList[m++]);
	// dummy point used to play the effect
	pInfo->sDummy2 = Str2Int(ParamList[m++]);
	// icon shown for the effect while it is attached to the character
	pInfo->sIcon = Str2Int(ParamList[m++]);

	memset(pInfo->szIcon, 0, sizeof(pInfo->szIcon));
	strLine = ParamList[m++];
	n = Util_ResolveTextLine(strLine.c_str(), strList, 10, ',');
	n = n > 10 ? 10 : n;
	for (int i = 0; i < n; i++)
	{
		strcpy(pInfo->szIcon[i], strList[i].c_str());
		pInfo->szIcon[defSKILLSTATE_NAME_LEN - 1][i] = _TEXT('\0');
	}

	strncpy(pInfo->szDesc, ParamList[m++].c_str(), defSKILLSTATE_DESC_NAME_LEN);
	pInfo->szDesc[defSKILLSTATE_DESC_NAME_LEN - 1] = '\0';

	pInfo->lColour = Str2Int(ParamList[m++]);
	
	return TRUE;
}

void CSkillStateRecordSet::_ProcessRawDataInfo(CRawDataInfo *pInfo)
{
	CSkillStateRecord *pState = (CSkillStateRecord*)pInfo;

    // refresh the dynamic attributes
	pState->RefreshPrivateData();	
}