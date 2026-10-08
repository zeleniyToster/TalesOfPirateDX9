//================================================================
// It must be permitted by Dabo.Zhang that this program is used for
// any purpose in any situation.
// Copyright (C) Dabo.Zhang 2000-2003
// All rights reserved by ZhangDabo.
// This program is written(created) by Zhang.Dabo in 2000.3
// This program is modified recently by Zhang.Dabo in 2003.7
//=================================================================
#pragma once

#ifndef	USING_TAO		// use the base Win32 Platform SDK
#include <winsock2.h>		// pulls in WinSock 2.2 (the 2.2 revision)
#include <windows.h>
#else
#include "TAOSpecial.h"
#endif

#include "DataSocket.h"
#include "Packet.h"

#define DS_SHUTDOWN (0xFFFF)
#define DS_DISCONN  (0xFFFE)

_DBC_BEGIN
#pragma pack(push)
#pragma pack(4)

class RPCMGR;
class Task;
class ThreadPool;
struct selparm;
struct BandwidthStat
{
	volatile uLong		m_tick;
	volatile uLong		m_sendbyteps,m_recvbyteps,m_sendpktps,m_recvpktps;
	volatile LLong		m_sendbytes,m_recvbytes;
	volatile LLong		m_sendpkts,m_recvpkts;
};

//=========================TcpCommApp=================================

extern void SetCommAppDebug( bool bDebug );

class TcpCommApp
{
	friend	class	TcpClientApp;
	friend	class	TcpServerApp;
	friend	class	RPCMGR;
	friend	class	AcceptConnect;
	friend	class	DelConnect;
	friend	class	OnConnect;
	friend	class	OnServeCall;
public:
	static	int		WSAStartup();
	static	void	WSACleanup();
// entry points called by the upper layer
	virtual	bool	OnConnect(DataSocket *datasock){return true;}	// return value: true = allow the connection, false = refuse it
	virtual	void	OnConnected(DataSocket *datasock){}			// return value: true = allow the connection, false = refuse it
	virtual	void	OnDisconnect(DataSocket *datasock,int reason){}// reason: 0 = local process shut down normally; -1 = socket error; -3 = connection closed by the peer; -5 = packet length out of range; -7 = blocking too severe; -9 = keepalive failed
	virtual	void	OnProcessData(DataSocket *datasock,RPacket &recvbuf)=0;
	virtual	void	OnSendAll(DataSocket *datasock){}
	virtual	bool	OnSendBlock(DataSocket *datasock){return false;}// return value: true = keep waiting, false = drop this connection
	virtual bool	OnEncrypt(DataSocket* datasock, char* ciphertext, uLong ciphertext_len, cChar* text, uLong& len);			// encrypt
	virtual bool	OnDecrypt(DataSocket* datasock, char* ciphertext, uLong& len) { return false; }	// decrypt
	virtual void	TaskDispatcher(Task *)=0;

	long			SendData(DataSocket *datasock,WPacket sendbuf);
	void			Disconnect(DataSocket *datasock,uLong remain =0,int reason =DS_DISCONN);

	BandwidthStat	GetBandwidthStat()const;
	uLong			GetSockTotal()const			{return __socklist.GetTotal();}
	ThreadPool	*	GetProcessor()const			{return __processor;}
	ThreadPool	*	GetCommunicator()const		{return __communicator;}
	uLong			GetPktHead()const;
	WPacket			GetWPacket()const;
	uLong			GetCurrentTick()const		{return m_TickCount;}
	uLong			GetRecvTime(DataSocket *datasock)const{if(datasock){return datasock->m_recvtime;}else{return uLong(long(-1));}}
	std::string			GetDisconnectErrText(int reason) const;

// const members
	cLong					__maxsndque;
	cuLong					__len_offset,__pkt_maxlen;
	cuChar					__len_size{};
	bool			const	__mode;
	RPCMGR		*	const	__rpc;
protected:
	void			SetPKParse(uLong len_offset,uChar len_size,uLong pkt_maxlen,long maxsndque);
	void			BeginWork(uLong keepalive_seconds=10,uLong delay =0);

	virtual void	ShutDown(uLong ulMilliseconds);

	void			DisconnectAll();
private:
	TcpCommApp(RPCMGR *rpc,ThreadPool *processor,ThreadPool *communicator,bool mode);
	virtual			~TcpCommApp();

	void			BeforeSel(selparm &p);
	void			AfterSel(selparm &p);
	bool			AddSocket(DataSocket *datasock);
	bool			DelSocket(DataSocket *datasock);
	mutable RunBiDirectChain<DataSocket> __socklist;
	BandwidthStat		m_band;
	InterLockedLong		m_selexit;

	ThreadPool			*const __communicator,*const __communicator1,*const __processor,*const __processor1;
	InterLockedLong		m_TickCount;
	uLong			const	__delay;
	uLong				m_keepalive;

	InterLockedLong		__atnotconn,__conntotal,__deltotal;
	long				_SendData(DataSocket *datasock,WPacket &sendbuf);
};

//==============================================================================
class TcpClientApp:public TcpCommApp
{
	friend class TcpServerApp;
public:
	TcpClientApp(RPCMGR *rpc,ThreadPool *processor,ThreadPool *communicator,bool mode =true);// mode = false forces the packet to be handled inline instead of on the thread pool
	~TcpClientApp(){};
	DataSocket *	Connect(cChar *hostname,uShort port,SOCKET *sock_out =0);
private:
	virtual void	TaskDispatcher(Task	*);
};
//=====TcpServerApp==============================================================================
class TcpServerApp:public TcpClientApp
{
public:
	TcpServerApp(RPCMGR *rpc,ThreadPool *processor,ThreadPool *communicator,bool mode =true);// mode = false forces the packet to be handled inline instead of on the thread pool
	~TcpServerApp();
	int				OpenListenSocket(uShort port,cChar *cp);	// return value: 0 = success, 1 = failure
	void			CloseListenSocket();
	const SOCKET	GetListenSocket()const	{return __socket;}
	cChar	*		GetListenIP()const		{return __localaddr;}
	uShort			GetListenPort()const	{return __port;}
	void			ZeroAcceptFlag()		{__acceptflag	=false;}
protected:
	virtual	void	ShutDown(uLong ulMilliseconds);
private:
	virtual	void	TaskDispatcher(Task	*);

	bool	volatile	__acceptflag;
	SOCKET	volatile	__socket;
	uShort				__port;							// server listen port (host byte order)
	char				__localaddr[16];				// server listen address (IP)
};

#pragma pack(pop)
_DBC_END
#pragma warning( disable :4355)
