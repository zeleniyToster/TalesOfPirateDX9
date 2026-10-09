#pragma once
#include "STStateObj.h"
#include "CharacterAction.h"

enum eActorState
{
	enumNormal,		//  normal state
	enumDied,		//  dead state
	enumRemains,	//  wreckage
};


//  executed on death
class CActorDie
{
public:
	virtual ~CActorDie() {}
	virtual void Exec()	 {}
};

//  monster loot
class CSceneItem;
class CMonsterItem : public CActorDie
{
public:
    CMonsterItem();
    virtual ~CMonsterItem();

    void	Exec();

    void    SetCha( CCharacter* pCha )	    { _pCha=pCha;	        }
    void    SetItem( CSceneItem* pItem )    { _pItem = pItem;       }
	CSceneItem*		GetItem()				{ return _pItem;		}

private:
    CSceneItem*         _pItem;
    CCharacter*         _pCha;

};

//  mission prompt
struct stNetNpcMission;
class CMissionTrigger : public CActorDie
{
public:
	CMissionTrigger();
	~CMissionTrigger();

	virtual void Exec();

	void	SetData( stNetNpcMission& v );

private:
	stNetNpcMission		*_pData;

};

class CCharacter;
class CActionState;
class CStateSynchro;
class CSkillRecord;
class CServerHarm;

class CActor
{
public:
	CActor(CCharacter *pCha);
	~CActor();

	void			InitState();

	CActionState*	GetCurState()				{ return _pCurState;			}
	CCharacter*		GetCha()					{ return _pCha;					}
	eActorState		GetState()					{ return _eState;				}
	void			SetState( eActorState v );

	bool			IsEmpty()					{ return _statelist.empty();	}

public:
    void            PlayPose( int poseid, bool isKeep=false, bool isSend=false );
	void			SetSleep()					{ _nWaitingTime = -1;			}

public:		//  used by CActionState
	bool	        SwitchState( CActionState* pState );						//  clears the current queue, interrupts the current action and runs the queued one
	bool	        InsertState( CActionState* pState, bool IsFront=false );	//  pushes the current action onto the queue to run later
    bool            AddState( CActionState* pState );

    CActionState*   FindStateClass( const type_info& info );            //  looks for a state of the same kind
    void            OverAllState();                                     //  ends every state; used on death when the server does not send an end

	void			CancelState();										//  interrupts the current state
	void			FrameMove(DWORD dwTimeParam);

	CActionState*   GetServerState();
	CActionState*   GetServerStateByID(int id);
	CActionState*   GetNextState();
	int				GetQueueCount()				{ return (int)_statelist.size();}
	bool			IsEnabled()					{ return GetState()==enumNormal;}

	void			FailedAction();

public:	//  skill
    void	        ActionBegin( DWORD pose_id );
	void			ActionKeyFrame( DWORD pose_id, int key_frame );
	void			ActionEnd( DWORD pose_id );

	void			Stop();
    void	        IdleState();

	void			ExecAllNet();			//  drains every queued network resource packet, used when the network removes a player

	void			ExecDied();
	bool			AddDieExec( CActorDie* pDieExec );
	
	void			ClearQueueState();

protected:
    //void            _InitIdle( DWORD pose );

    typedef std::list<CStateSynchro*>  synchro;
    void            _ExecSynchro( synchro& s );
    void            _ClearSynchro( synchro& s );

protected:
	CActionState*	_pCurState;				//  current state
	typedef std::list<CActionState*> states;
	states			_statelist;

	CCharacter*		_pCha;					//  the character the current action belongs to

public:
    CServerHarm*    CreateHarmMgr();
    CServerHarm*    FindHarm( int nFightID );
    CServerHarm*    FindHarm();				//  finds an available one

private:
    typedef std::vector<CServerHarm*>    fights;
    fights          _fights;

	typedef std::vector<CActorDie*>		dies;	//  network packet: monster loot
	dies			_dies;

protected:
    int             _nWaitingTime;     //  after waiting for this long, pick a random funny Pose
	eActorState		_eState;

};

//  inline function
inline void	CActor::ActionKeyFrame( DWORD pose_id, int key_frame )	
{ 
	if( _pCurState ) _pCurState->ActionFrame(pose_id, key_frame);	
}

inline void	CActor::ActionBegin( DWORD pose_id )							
{ 
    if ( _pCurState ) _pCurState->ActionBegin(pose_id);				
}

inline void CActor::ActionEnd( DWORD pose_id )
{ 
    if ( _pCurState ) 
    {
        _pCurState->ActionEnd(pose_id);
    }
    else if( _eState==enumNormal && (pose_id == POSE_SHOW || pose_id == POSE_FLY_SHOW ) )
    {
        IdleState();
    }
}

inline CActionState* CActor::GetServerState()
{
    if( _pCurState )
    {
        if( !_pCurState->GetIsOver() )
            return _pCurState;

        for( states::iterator it=_statelist.begin(); it!=_statelist.end(); ++it )
            if( !(*it)->GetIsOver() )
                return *it;
    }
    return NULL;
}

inline CActionState* CActor::GetServerStateByID(int id)
{
    if( _pCurState )
    {
        if( _pCurState->GetServerID()==id )
            return _pCurState;

        for( states::iterator it=_statelist.begin(); it!=_statelist.end(); ++it )
            if( (*it)->GetServerID()==id )
                return *it;
    }
    return NULL;
}

inline void CActor::_ExecSynchro( synchro& s )
{
    for( synchro::iterator it=s.begin(); it!=s.end(); ++it )
    {
        (*it)->Exec();
        // delete *it;
    }
    s.clear();
}

inline void CActor::_ClearSynchro( synchro& s )
{
    for( synchro::iterator it=s.begin(); it!=s.end(); ++it )
    {
        //delete *it;
		SAFE_DELETE(*it);//  UI freeze handling
    }
    s.clear();
}

inline void CActor::InitState()
{
	ClearQueueState();
	SAFE_DELETE( _pCurState );

	_eState = enumNormal;
    IdleState();
}

inline bool CActor::AddDieExec( CActorDie* pDieExec )
{
	_dies.push_back( pDieExec );
	return true;
}
