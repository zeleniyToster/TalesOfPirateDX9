#include <iostream>
#include <time.h>
#include "DataSocket.h"
#include "GameCommon.h"
#include "OuterServer.h"
#include "Character.h"
#include "Player.h"
#include "GameServerApp.h"
#include "util.h"
#include "ThreadPool.h"
#include "TryUtil.h"
#include "Config.h"
#include "GameApp.h"

_DBC_USING
//OuterServer * g_gmout	=0;

OuterServer::OuterServer(ThreadPool *proc,ThreadPool *comm)
:TcpServerApp(this,proc,comm),RPCMGR(this)
,m_count(0)
{T_B
	SetPKParse(0,2,4*1024,400);
	BeginWork(g_Config.m_lSocketAlive);
	m_mutdisc.Create(false);
T_E}

OuterServer::~OuterServer()
{T_B
	ShutDown(12*1000);
T_E}

bool	OuterServer::OnConnect(DataSocket *datasock)					//return value: true - connection allowed, false - connection not allowed
{T_B
	datasock->SetRecvBuf(32*1024); 
	datasock->SetSendBuf(32*1024);
	LG(g_szConnectLog, "GateServer Cconnected! IP = [%s] port = %d\n",  datasock->GetPeerIP() , datasock->GetPeerPort());

    // notifies the application layer that a GateServer has been connected
    WPacket wpkt = g_gmsvr->GetWPacket();
    wpkt.WriteCmd(CMD_MM_GATE_CONNECT);
    wpkt.WriteChar(0);
    AddPK(datasock, wpkt);

	return true;
T_E}

void	OuterServer::OnDisconnect(DataSocket *datasock,int reason)		//reason值:0-本地程序正常退出；-3-网络被对方关闭；-1-Socket错误;-5-包长度超过限制。
{T_B
	LG(g_szConnectLog, "GateServer Disconnect! IP = [%s] port = %d, reason = [%d]\n",  datasock->GetPeerIP() , datasock->GetPeerPort(), reason);

    GateServer* gt = (GateServer *)datasock->GetPointer();
    if (gt == NULL) return;

	if (gt->IsValid())
	{
		// notifies the logic layer that the connection to that Gate has dropped
		WPacket WtPk=g_gmsvr->GetWPacket();
		WtPk.WriteCmd(CMD_MM_GATE_RELEASE);
		WtPk.WriteChar(0);
		AddPK(datasock, WtPk);

        // cleans up the GateServer
        gt->Invalid();      
    }
T_E}

void OuterServer::OnProcessData(DataSocket *datasock,RPacket recvbuf)
{T_B
	AddPK(datasock,recvbuf);
T_E}


WPacket	OuterServer::ServeCall(DataSocket *datasock,RPacket in_para)
{T_B
    m_count++;
    WPacket l_retpk = GetWPacket();
    return l_retpk;
T_E}

void OuterServer::ProcessData(DataSocket* datasock, RPacket pk)
{T_B
    if (datasock == NULL) return;
    GateServer* gt = (GateServer *)datasock->GetPointer();
    if (gt == NULL) return;

    int msg_id;
    uShort cmd = pk.ReadCmd();
    switch (cmd)
        {
        case CMD_MM_GATE_CONNECT:
            msg_id = NETMSG_GATE_CONNECTED;
            break;

        case CMD_MM_GATE_RELEASE:
            msg_id = NETMSG_GATE_DISCONNECT;
            break;

        default:
            msg_id = NETMSG_PACKET;
            break;
        }

    g_pGameApp->ProcessNetMsg(msg_id, gt, pk);
T_E}



char	g_szConnectLog[256] = "Connect";


long ToGateServer::Process()
{T_B
	DWORD	dwTick, dwCurTick;
	DWORD	dwInterval = 5000;
	DWORD	dwConnectTick = 0;
	
	dwTick = dwCurTick = GetTickCount();
	dwTick -= dwConnectTick; // makes sure the connect-GateServer operation runs as soon as the thread starts

	while (!GetExitFlag())
    {
		dwCurTick = GetTickCount();

		static DWORD dwLastRunTick = 0;
		if(dwCurTick - dwLastRunTick < dwInterval) 
   		{
			Sleep(10);
            continue;
		}
		
		dwLastRunTick = dwCurTick;

		// connects to every GateServer that is not yet connected
		if (dwCurTick - dwTick >= dwConnectTick)
	    {
			dwTick = dwCurTick;
			for (int i = 0; i < g_pGameApp->m_gatenum; i++)
				if (!g_pGameApp->m_gate[i].IsValid())
					g_gmsvr->LoginGate(&g_pGameApp->m_gate[i]);
		}
	}

	return 0;
T_E}
