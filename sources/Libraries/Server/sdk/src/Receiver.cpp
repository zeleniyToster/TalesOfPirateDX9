//================================================================
// It must be permitted by Dabo.Zhang that this program is used for
// any purpose in any situation.
// Copyright (C) Dabo.Zhang 2000-2003
// All rights reserved by ZhangDabo.
// This program is written(created) by Zhang.Dabo in 2000.3
// This program is modified recently by Zhang.Dabo in 2003.7
//=================================================================
#include "DataSocket.h"
#include "Receiver.h"
#include "Packet.h"
#include "CommRPC.h"
#include <fstream>
#include "iostream"

_DBC_USING

// [NETLOG] packet receive diagnostics. Off by default: opened in the process
// working directory and grew to ~3 MB per session, several log lines per received
// packet. Added while chasing the GateServer handshake failure (client error 522),
// which is now fixed.
// To bring it back: set DIAG_RECEIVER_LOG to 1 and uncomment the [NETLOG] lines.
// See docs/BUILD_NOTES.md.
#define DIAG_RECEIVER_LOG 0

#if DIAG_RECEIVER_LOG
#include <fstream>
#include <iostream>
static std::ofstream debug_log("receiver_debug.log", std::ios::app);
#define DEBUG_LOG debug_log
#else
#define DEBUG_LOG if (false) std::cerr
#endif

extern PreAllocHeap<rbuf>	__bufheap;
PreAllocHeapPtr<OnProcessData>	Receiver::m_HeapProcData(0,30);

//================================================================================
inline void OnProcessData::Init(DataSocket * datasock,RPacket &rpk)
{
	m_datasock	=datasock;
	__tca		=m_datasock->GetTcpApp();
	m_rpk		=rpk;
}
//--------------------------------------------------
long OnProcessData::Process()
{
	try{
		__tca->OnProcessData(m_datasock,m_rpk);
	}catch(...){}
	m_datasock->m_procflag--;
	m_rpk	=0;
	return 0;
}
//================================================================================
Receiver::Receiver(DataSocket * datasock)
:m_datasock(datasock),_len_inc(0),__tca(0)
{
};

void Receiver::Initially()
{
	m_p			=0;
}
void Receiver::Finally()
{
	m_rpk		=RPacket(0);
}
void Receiver::Init()
{
	__tca = m_datasock->GetTcpApp();
	*const_cast<uLong*>(&_len_inc)	=__tca->__len_offset + __tca->__len_size;
	m_rpk		=RPacket(m_datasock->GetTcpApp());

	#if DEBUG_RECEIVER
	// [NETLOG] DEBUG_LOG << "[RECEIVER INIT] __tca: " << __tca << ", m_datasock->GetTcpApp(): " << m_datasock->GetTcpApp() << std::endl;
	// [NETLOG] DEBUG_LOG << "[RECEIVER INIT] m_rpk valid: " << (bool)m_rpk << ", m_rpk.__tca: " << m_rpk.__tca << ", m_rpk.Size(): " << m_rpk.Size() << std::endl;
	#endif
}
//--------------------------------------------------
long Receiver::Process()
{
	#if DEBUG_RECEIVER
	// [NETLOG] DEBUG_LOG << "[RECEIVER PROCESS START] m_datasock: " << m_datasock << ", __tca: " << __tca << ", m_p: " << m_p << std::endl;
	// [NETLOG] DEBUG_LOG << "[RECEIVER PROCESS START] m_rpk valid: " << (bool)m_rpk << ", m_rpk.__tca: " << m_rpk.__tca << ", m_rpk.Size(): " << m_rpk.Size() << std::endl;
	#endif

	int			l_reallen	=0;
	long		l_retval	=0;

	while(!m_datasock->m_delflag && !GetExitFlag())
	{
		if(!HasSpace())
		{
			m_rpk	=__bufheap.Get(max(m_datasock->GetRecvBuf(),__tca->__pkt_maxlen));
			m_p		=0;
		}


		// l_reallen is the n bytes received, and the data is written to the buffer in m_rpk
		#if DEBUG_RECEIVER
		// [NETLOG] DEBUG_LOG << "[RECEIVER RECV] BEFORE recv() -> m_rpk valid: " << (bool)m_rpk << ", GetPktAddr: " << static_cast<const void*>(m_rpk.GetPktAddr()) << ", m_p: " << m_p << std::endl;
		// [NETLOG] DEBUG_LOG << "[RECEIVER RECV] __bufheap.unit: " << __bufheap.GetUnitSize() << ", __bufheap.used: " << __bufheap.GetUsedNum() << std::endl;
		// [NETLOG] DEBUG_LOG << "[RECEIVER RECV] HasSpace: " << HasSpace() << std::endl;
		#endif

		l_reallen	=recv(m_datasock->m_socket,const_cast<char*>(m_rpk.GetPktAddr()) + m_p,int(HasSpace()),0);

		#if DEBUG_RECEIVER
		// [NETLOG] DEBUG_LOG << "[RECEIVER RECV] AFTER recv() -> l_reallen: " << l_reallen << ", WSAGetLastError: " << WSAGetLastError() << std::endl;
		#endif

		if(l_reallen >0)
		{
			m_rpk.m_tickcount		=::GetTickCount();
			m_datasock->m_recvtime	=__tca->GetCurrentTick();
			m_p +=l_reallen;
			while(!m_datasock->m_delflag)
			{
				if(!m_rpk.GetPktLen()&&(m_p >=_len_inc))
				{
					m_rpk.ReadPktLen();

					if(m_rpk.GetPktLen() >__tca->__pkt_maxlen||m_rpk.GetPktLen()<_len_inc)
					{
						l_retval	=-5;
						break;
					}
				}else if(m_p <_len_inc)
				{
					l_retval	=0;
					break;
				}
				if((m_p	>=m_rpk.GetPktLen()) && m_rpk.GetPktLen())
				{
					m_datasock->m_rpks ++;
					m_datasock->m_rbts +=m_rpk.GetPktLen();

					if(m_rpk.GetPktLen() >_len_inc && m_datasock->m_isProcess)
					{
						//if (m_rpk.GetPktLen() < (8 + 16) && strcmp(m_datasock->GetPeerIP(), m_datasock->GetLocalIP())) {
						//	// Packet length must be atleast 6 bytes + 2 bytes + 16 bytes.
						//	// Head = len_offset(0 bytes) + len_size(2 bytes) + SESS(4 bytes) + CMD(2 bytes) + IV(16 bytes) 
						//	// Assuming no data.
						//	// Ignore packets coming from local, cause these can be smaller.
						//	l_retval = -32;
						//	break;
						//}
						RPacket	l_rpk	=m_rpk;
						uLong l_len =l_rpk.GetDataLen();
						try{
							bool bDec = __tca->OnDecrypt(m_datasock,const_cast<char*>(l_rpk.GetDataAddr()),l_len);
							// The above line doesn't do anything if the handshake is not completed.
						}
						catch (std::out_of_range const&) {
							__tca->Disconnect(m_datasock, 0, -32);
							// Every exit from Process() has to release m_recvflag, otherwise
							// TcpCommApp::BeforeSel() never manages to schedule DelConnect and
							// the DataSocket is leaked for good.
							m_datasock->m_recvflag --;
							return 0;
						}
						catch(...){

							__tca->Disconnect(m_datasock, 0, -31);

							m_datasock->m_recvflag --;
							return 0;
						}
						l_rpk.DiscardLast(l_rpk.GetDataLen() -l_len);
						try
						{
							if(!__tca->__rpc || !__tca->__rpc->ProcessSESS(m_datasock,l_rpk))
							{
								OnProcessData	*l_processdata	=m_HeapProcData.Get();
								l_processdata->Init(m_datasock,l_rpk);
								if(__tca->__mode && __tca->GetProcessor())
								{
									m_datasock->m_procflag++;
									try{
										__tca->GetProcessor()->AddTask(l_processdata);
									}catch(...)
									{
										m_datasock->m_procflag--;
									}
								}else
								{
									m_datasock->m_procflag++;
									try{
										l_processdata->Process();
									}catch(...){}
									m_datasock->m_procflag--;
									try{
										l_processdata->Lastly();
									}catch(...){}
								}
							}
						}catch(...){}
					}//if(m_rpk.GetPktLen() >_len_inc && m_datasock->m_isProcess)

					m_rpk.SetOffset(m_rpk.GetOffset()+m_rpk.GetPktLen());
					m_p			-=m_rpk.GetPktLen();
					m_rpk.SetPktLen(0);
				}else if(m_p <m_rpk.GetPktLen())
				{
					l_retval	=0;
					break;
				}
			}//while(!m_datasock->m_delflag)
			if(!l_retval && !HasSpace() && !m_datasock->m_delflag)
			{
				rbuf	*l_rbuf	=__bufheap.Get(max(m_datasock->GetRecvBuf(),__tca->__pkt_maxlen));
				if(m_p)
				{
					MemCpy(l_rbuf->getbuf(),m_rpk.GetPktAddr(),m_p);
				}
				uLong l_len	=m_rpk.GetPktLen();
				m_rpk	=l_rbuf;
				m_rpk.SetOffset(0);
				m_rpk.SetPktLen(l_len);

				continue;
			}
		}else if(l_reallen	==0)
		{
			l_retval	= -3;
		}else if(l_reallen	==SOCKET_ERROR)
		{
			l_retval	=WSAGetLastError();
		}else
		{
			l_retval	=-2;
		}
		break;
	}//while(!m_datasock->m_delflag && !GetExitFlag())
	if(m_datasock->m_delflag)
	{
		m_datasock->m_recvflag --;
		return	0;
	}else if(l_retval && (l_retval !=WSAEWOULDBLOCK))
	{
		__tca->Disconnect(m_datasock,0,l_retval);
	}
	m_datasock->m_recvflag --;
	return l_retval;
};

dbc::uLong dbc::Receiver::HasSpace() const
{
	#if DEBUG_RECEIVER
	// [NETLOG] DEBUG_LOG << "[RECEIVER HasSpace] m_rpk valid: " << (bool)m_rpk << ", m_p: " << m_p << ", m_rpk.GetOffset(): " << m_rpk.GetOffset() << std::endl;
	#endif

	auto current_size = m_rpk.Size();
	auto used_size = m_rpk.GetOffset() + m_p;

	#if DEBUG_RECEIVER
	// [NETLOG] DEBUG_LOG << "[RECEIVER HasSpace] current_size: " << current_size << ", used_size: " << used_size << std::endl;
	#endif

	if (current_size > used_size)
	{
		auto remain_size = m_rpk->Size() - m_rpk.GetOffset() - m_p;
		#if DEBUG_RECEIVER
		// [NETLOG] DEBUG_LOG << "[RECEIVER HasSpace] remain_size: " << remain_size << std::endl;
		#endif
		return remain_size;
	}
	return 0;
	//return (m_rpk.Size() > (m_rpk.GetOffset() + m_p)) ? (m_rpk.Size() - m_rpk.GetOffset() - m_p) : 0;
}