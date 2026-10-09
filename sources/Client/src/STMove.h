#pragma once
#include "STStateObj.h"
#include <queue>
#include "NetProtocol.h"

class CSceneItem;

class CWaitMoveState : public CActionState
{
public:
	CWaitMoveState(CActor* p);
	~CWaitMoveState();

	virtual void FrameMove();
	virtual void PushPoint( int x, int y );
    virtual const char* GetExplain()            { return "CWaitMoveState";  }
	virtual void Cancel(){}

	CMoveList*		GetMoveList()				{ return &_cMoveList;		}

    void            Clear()                     { _cMoveList.Clear();		}
    void			ChaRun();

	static void		SetPreMoveTime( long time );
	static int		GetPreMoveTime()				{ return _ulPreMoveTime;	}
	static float	GetMoveRate()					{ return _fMoveRate;		}
	static DWORD	GetPreMoveDis()					{ return _dwPreMoveDis;		}

	static void		RenderPing();
	static void		RenderCameraInfo();

protected:
	virtual bool _Start();
    virtual bool _IsAllowCancel()				{ return false;				}

	bool		IsAllowMove();

protected:
	CMoveList	_cMoveList;
	CCharacter*	_pCha;

	static float			_fMoveRate;
	static	unsigned long	_ulPreMoveTime;		// predicted movement time
	static DWORD			_dwPreMoveDis;

private:		// the ping value that was measured
	enum  
	{
		LAST_NUM = 3,
	};
	static int MaxPing;
	static int MinPing;
	static int LastPing[LAST_NUM];
	static int LastPingCnt;
	static int LastPingShow[LAST_NUM];			// used to show the most recent ping value
	static int nTotalPing;
	static int nTotalPingCnt;
	static float fAveragePing;
};

// free movement
class CMoveState : public CWaitMoveState
{
public:
	CMoveState(CActor* p);

	virtual void MoveEnd(int x, int y, int nState);
    virtual void PushPoint( int x, int y );
    virtual void FrameMove();
    virtual void Cancel();
    virtual const char* GetExplain()        { return "CMoveState";      }

	void			SetWalkLine( bool v )		{ _IsWalkLine = v;			}
	void			SetMoveTo( int x, int y )	{ _nTargetX=x; _nTargetY=y; }

    stNetMoveInfo&  GetPathInfo()           { return _stPathInfo;       }

    static bool     IsSameServer( CCharacter* pCha );

protected:
	virtual bool _Start();
	virtual void _End();
    virtual bool _IsAllowCancel();

protected:
    void     WriteInfo( S_BVECTOR<D3DXVECTOR3>& path, stNetMoveInfo& info );
    void     _SynchroServer();

protected:
	int			_nTargetX,		_nTargetY;

    stNetMoveInfo   _stPathInfo;    // used for the server side path finding list
	bool	        _IsWalkLine;	// walk in a straight line

};


// obey the movement given by the server
class CServerMoveState : public CMoveState
{
public:
    CServerMoveState(CActor* p) : CMoveState(p), _IsClientMove(true) {}

    virtual const char* GetExplain()        { return "CServerMoveState";		}

    virtual void Cancel();
    virtual void FrameMove();
    virtual void MoveEnd(int x, int y, int nState);
    virtual void PushPoint( int x, int y );

	void	PreMove();
	static  DWORD			GetLastTime()	{ return _dwLastMoveTime;			}

protected:
    virtual bool _Start();
    virtual void _End();

	int			_nLocalTargetX, _nLocalTargetY;

	static DWORD	_dwLastMoveTime;
	static DWORD	_dwLastEndTime;	// used to decide whether it is in a stationary state

	bool		_IsClientMove;

};

// walk backwards without changing direction; used only for standalone testing
class CBackMoveState : public CMoveState
{
public:
	CBackMoveState(CActor* p) : CMoveState(p) {}
	virtual void FrameMove();

protected:
    virtual bool _Start();

};

// integrated movement
class COneMoveState : public CWaitMoveState
{
public: 
	COneMoveState(CActor* p);

    virtual void Cancel();
	virtual void FrameMove();
	virtual void PushPoint( int x, int y );
    virtual void MoveEnd(int x, int y, int nState);
    virtual const char* GetExplain()            { return "COneMoveState";	}

	void	SetMoveTo( int x, int y )			{ _nStartX=x; _nStartY=y;	}
	void	SetWalkLine( bool v )				{ _IsStartWalkLine = v;		}
	bool	ContinueMove( int nTargetX, int nTargetY, bool isWalkLine, bool IsCheckTime=false );
	bool	AddPath( int x, int y, bool IsWalkLine );
	bool	IsSameServerPos( int x, int y );
	void    SetRate(float r) { _fRate = r; }
	float   GetRate() { return _fRate; }
	// splits the path finding path into segments
	// the parameters are: input list, input path, segment length (less than zero means no splitting)
	static void	CompartMoveList( CMoveList& outlist, S_BVECTOR<D3DXVECTOR3>& path, int length );

protected:
    virtual bool _Start();
    virtual void _End();

    void    WriteInfo( S_BVECTOR<D3DXVECTOR3>& path, stNetMoveInfo& info );
	bool	SendInfo();
	bool	StartMove( int nTargetX, int nTargetY, bool isWalkLine );
	void    SynchroPos( int x, int y );

	float	RefreshRate( int nLocalLen, int nServerLen );
	int		GetSyschroDistance();

private:
	int			_nStartX,	_nStartY;		// the target point when the movement started
	bool		_IsStartWalkLine;			// the parameters when the movement started

	bool		_IsWalkLine;
	int			_nTargetX,	_nTargetY;

	CMoveList	_cLocalList;	// the local movement sequence
	int			_nLocalDis;		// the local distance

	int			_nServerDis;		// the total distance the server has to walk
	int			_nServerStepDis;	// the distance the server has already walked

	int			_nMoveCount;		// number of movement requests
	int			_nStartCount;		// number of movement request attempts
	int			_nSendCount;
	int			_nEndCount;			// number of completions returned by the server
	bool		_IsSendCancel;

	DWORD		_dwSendTime;
	DWORD		_dwEndTime;			// the time when it ended

	float		_fRate;

private:
	struct NeedPath
	{
		NeedPath( int nX, int nY, bool bLine ) : x(nX), y(nY), IsLine(bLine) {}

		int		x, y;
		bool	IsLine;
	};
	typedef std::list<NeedPath> path;
	path		_cNeedList;			// the points that have to be walked

	static CMoveList	_List;

};

