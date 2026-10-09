#pragma once

class CSynchroManage;
class CServerHarm;
class CCharacter;

// used to keep the client synchronised with what the server displays
class CStateSynchro
{
    friend class CSynchroManage;
public:
    CStateSynchro();
    virtual ~CStateSynchro();

    // splits the current packet off into another packet (create) by the given proportion
    virtual CStateSynchro*   Gouge( float fRate )    { return NULL;  }

    void    Exec();
    void    Exec( DWORD time );

    bool    GetIsExec()							{ return _isExec;       }

	void	SetServerHarm( CServerHarm* pHarm )	{ _pServerHarm=pHarm; 	}
	CServerHarm*	GetServerHarm()				{ return _pServerHarm;	}

	void	Reset()								{ _isExec=false;		}

	virtual CCharacter*	GetHarmCha() = 0;
	virtual const char* GetClassName()			{ return "CStateSynchro";}

protected:
    virtual void _Exec(){}

    bool			_isExec;

	CServerHarm*	_pServerHarm;

private:
    int				_nID;				// index into CSynchroManage
	DWORD			_dwExecTime;		// execution time
	DWORD			_dwCreateTime;

};

// used to manage synchronised packets so they run in a fixed order
class CSynchroManage
{
    friend class CStateSynchro;
public:
    CSynchroManage();
    ~CSynchroManage();

    void	FrameMove( DWORD dwTime );
	void	Reset();

public: 
    static CSynchroManage* I() { return _pInstance;     }

private:
    void    _CollectEmpty();        // the head and tail are already joined, but there is still room, so collect the remaining room

private:
    int     _AddState( CStateSynchro* pState );
    bool    _DelState( CStateSynchro* pState );

private:
    enum 
    {
        MAX_SIZE = 4096,
        ERROR_ID = -1,
    };

    CStateSynchro*  _All[MAX_SIZE];
    DWORD           _dwHead, _dwTail;       // valid region at the head and tail
    DWORD           _nSynchroNum;           // number of valid states

private:
    static CSynchroManage* _pInstance;

};

// inline function
inline void CStateSynchro::Exec()
{ 
    _isExec = true; 
}

inline void CStateSynchro::Exec( DWORD time )
{
	if( time<_dwExecTime )
	{
		_dwExecTime = time;
	}
}

