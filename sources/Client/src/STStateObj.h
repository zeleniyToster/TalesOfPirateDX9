#pragma once

#define  _STATE_DEBUG

#include "HMSynchroObj.h"

class CStateSynchro;
class CMoveList;
class CActor;
class CCharacter;

// action base class
class CActionState
{
	friend class CActor;
public:
    CActionState(CActor* p);

    void Start();								// starts executing

	virtual void PushPoint( int x, int y ){}					// for derived classes that may have movement points
	virtual void ServerEnd(int nState)      { _IsOver = true;	}	// sets the server notification that execution has finished
	virtual void MoveEnd(int x, int y, int nState)	{ _IsOver = true;	}	// for an Action with movement, ends only the movement
    virtual void SetServerID( int n );
    virtual void Cancel();
	virtual void MouseRightDown(){}

    virtual const char*     GetExplain()    { return "CActionState";}

    int     GetServerID()       { return _nServerID;}

	void	PopState()			{_isExecEnd = true;	}

	CActor* GetActor() 			{ return _pActor;	}

	bool	GetIsOver()			{ return _IsOver;	}			// once the remaining actions are done the action is complete
	bool	GetIsExecEnd()		{ return _isExecEnd;}			// every action has finished executing (the server has notified that it is finished)
	bool	GetIsCancel()		{ return _IsCancel;	}			// this action has been cancelled by the player
	bool	GetIsWait()			{ return _isWait;	}

	// this action is a subclass of another action; note that CActor does not know about this action
    void			SetParent( CActionState* p ){ _pParent=p;		}	
	CActionState*	GetParent()				{ return _pParent;		}

	void			SetIsSend( bool v )		{ _IsSend = v;		    }
    bool            GetIsSend()             { return _IsSend;       }


    bool            GetIsInit()             { return _IsInit;       }

protected:
    virtual		~CActionState();

    virtual void End();												
    virtual void FrameMove();
    virtual void ActionBegin( DWORD pose_id ){}
    virtual void ActionFrame( DWORD pose_id, int key_frame ) {}	
    virtual void ActionEnd( DWORD pose_id )	{}
    virtual void BeforeNewState(){}
    virtual bool IsKeepPose()       { return false;     }       // whether to return to the idle state once the action has finished
    virtual bool IsAllowUse()       { return true;      }
	virtual void StartFailed()		{}

	virtual bool	_Start()		{ return true;		}
	virtual void	_End()			{}          // execution finished
    virtual bool    _IsAllowCancel();			// whether this state can be cancelled

	bool			_AllowCancel()		{ return !_isWait && !_IsCancel && !_IsOver;	}

private:
	void			SetIsWait( bool v )		    { _isWait=v;		}
    void			_StartFailed();		// called when starting fails

protected:
	bool	_IsCancel;		
	bool	_IsSend;
	bool	_IsOver;
    int     _nServerID;

private:
	CActor			*_pActor;
	CActionState	*_pParent;

	bool	_isWait;			// 为true,放在Actor的队列中,不立即执行,即IsAllowStart()返回false
	bool	_isExecEnd;
	bool	_IsInit;

};

// the list of points the Actor has to move to
class CMoveList
{
public:
	void	Clear(unsigned int count=0);
	void	GotoFront(CCharacter *pCha);
	void	PushPoint( int x, int y );

	bool	IsEmpty()						{ return _path.empty();	    }
	int		GetCount()						{ return (int)_path.size();	}

	void	PopFront()						{ _path.pop_front();		}
	POINT&	GetFront()						{ return _path.front();		}
	POINT&	GetBack()						{ return _path.back();		}

	int		GetListDistance(int x, int y);

private:
	typedef std::list<POINT> path;
	path	_path;

};

// inline function
inline void CActionState::SetServerID( int n )
{
    if( _nServerID==INT_MAX )
    {
        _nServerID = n;
    }
    else
    {
        LG( "state", g_oLangRec.GetString(409), GetExplain(), n );
    }
}

