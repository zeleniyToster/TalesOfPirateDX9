#pragma once

#include "HMSynchroObj.h"

class CStateSynchro;
class CSkillRecord;
class CActor;
class CCharater;
class CAttackEffect;
class CCharacter;
class CAttackRepSynchro;

const int ERROR_FIGHT_ID = INT_MAX;

class CServerHarm
{
    typedef std::list<CAttackEffect*> synchros;
    friend CActor;
public:
    CServerHarm( CActor* pActor );
    ~CServerHarm();

	void	InitMemory();

    void    Exec();             
    void    Gouge( int nGouge );                // splits one packet into several executions

    bool    AddHarm( CAttackEffect* s, CSkillRecord *pSkill );        // adds the damage packets that came over the network, executed on damage
    bool    AddRep( CAttackRepSynchro* s );         // presentation packets, executed when the execution starts
    bool    IsAllowStateOver();                 // whether ending the state is allowed

    void    ReadyExec()             { _nReadyExec++;            }
	void    SetIsOuter( bool v );

	void	ExecAll(int nCount);
	void	ExecAll();

	void	SetFightID( int v )		{ _nFightID = v;			}
	void	SetSkill( CSkillRecord* p )	{ _pSkill = p;			}
	
	int		GetCount()				{ return (int)_harm.size();	}

	void	DelHarm( CAttackEffect* p )	{ _harm.remove( p );	}

	void 	HitRepresent( CSkillRecord* pSkill, int nAngle );

public:
    CSkillRecord*   GetSkill()      { return _pSkill;           }

    int     GetFightID()            { return _nFightID;         }
	bool	GetIsExecEnd()		    { return _nFightID==ERROR_FIGHT_ID; }

private:

private:
    int         _nFightID;
    CActor*     _pActor;            // the Actor it belongs to
	CCharacter* _pCha;

    synchros    _harm;              // damage packets, played on key frames
    synchros    _rep;               // presentation packets, played at the start

    CSkillRecord*   _pSkill;

    int         _nReadyExec;        // the number of executions the client is preparing

	bool		_IsOuter;			// whether there is an external call

};

// inline function
