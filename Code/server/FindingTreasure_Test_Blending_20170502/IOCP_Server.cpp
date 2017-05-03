#include "stdafx.h"
#include "IOCP_Init.h"
#include "IOCP_Server.h"
#include "RespawnManager.h"

HANDLE g_hiocp;
SOCKET g_ssocket;
CPlayer* g_box=NULL;
CRespawnManager g_RespawnManager;

enum OPTYPE { OP_SEND, OP_RECV };

struct OverlappedEX {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
};

struct CLIENT {
	bool team;
	bool connect;

	CPlayer player;

	SOCKET client_socket;
	OverlappedEX recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packat_data;
	int curr_packat_size;
};

CLIENT g_clients[MAX_USER];

void error_display(char *msg, int err_no)
{
	WCHAR *lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::cout << L"에러\n" << std::endl;
	std::cout << lpMsgBuf;
	LocalFree(lpMsgBuf);
}

void Initialize_Server()
{
	std::wcout.imbue(std::locale("korean"));

	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_hiocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	g_ssocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = INADDR_ANY;

	bind(g_ssocket, reinterpret_cast<sockaddr*>(&ServerAddr), sizeof(ServerAddr));

	listen(g_ssocket, 5);

	for (int i = 0; i < MAX_USER; ++i)
	{
		g_clients[i].connect = false;
		g_clients[i].player.SetMesh(CMeshResource::pPirateMesh);
		g_clients[i].player.SetBelongType(i < 4 ? BELONG_TYPE_BLUE : BELONG_TYPE_RED);
	}
	g_RespawnManager.InitRespawnManager();
}

void SendPacket(int cl, void *packet)
{
	int psize = reinterpret_cast<unsigned char *>(packet)[0];
	int ptype = reinterpret_cast<unsigned char *>(packet)[1];
	OverlappedEX *over = new OverlappedEX;
	over->event_type = OP_SEND;
	memcpy(over->IOCP_buf, packet, psize);
	ZeroMemory(&over->over, sizeof(over->over));
	over->wsabuf.buf = reinterpret_cast<CHAR *>(over->IOCP_buf);
	over->wsabuf.len = psize;
	int ret = WSASend(g_clients[cl].client_socket, &over->wsabuf, 1, NULL, 0, &over->over, 0);
	if (0 != ret)
	{
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)
		{
			error_display("Error in SendPacket:", err_no);
		}
	}
	std::cout << "Send Packet To Client [" << ptype << "]: " << cl << std::endl;
}

void SendInitPacket(int client)
{
	sc_packet_pos packet;
	packet.id = client;
	packet.size = sizeof(packet);
	packet.type = SC_INIT;

	packet.x = g_clients[client].player.GetPosition().x;
	packet.y = g_clients[client].player.GetPosition().y;
	packet.z = g_clients[client].player.GetPosition().z;

	SendPacket(client, &packet);
}


void SendPutPlayerPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = g_clients[object].player.GetPosition().x;
	packet.y = g_clients[object].player.GetPosition().y;
	packet.z = g_clients[object].player.GetPosition().z;

	SendPacket(client, &packet);
}

void SendRemovePlayerPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_REMOVE_PLAYER;

	SendPacket(client, &packet);
}

void SendPositionPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_POS;
	packet.x = g_clients[object].player.GetPosition().x;
	packet.y = g_clients[object].player.GetPosition().y;
	packet.z = g_clients[object].player.GetPosition().z;

	SendPacket(client, &packet);
}


void DisconnectClient(int ci)
{
	closesocket(g_clients[ci].client_socket);
	g_clients[ci].connect = false;
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (true == g_clients[i].connect)
			SendRemovePlayerPacket(i, ci);
	}
}

void ProcessPacket(int ci, unsigned char packet[])
{
	switch (packet[1])
	{
	case CS_UP:
		g_clients[ci].player.m_d3dxvMoveDir.z += 2.0;
		break;
	case CS_DOWN:
		g_clients[ci].player.m_d3dxvMoveDir.z += -2.0;
		break;
	case CS_LEFT:
		g_clients[ci].player.m_d3dxvMoveDir.x += -2.0;
		break;
	case CS_RIGHT:
		g_clients[ci].player.m_d3dxvMoveDir.x += 2.0;
		break;
	case CS_JUMP:
		g_clients[ci].player.m_d3dxvMoveDir.y += 5.f;
		break;
	default:
		std::cout << "Unknown Packet Type from Client" << ci << std::endl;
		while (true);
	}
	std::cout << ci <<" X Y Z : (" << g_clients[ci].player.GetPosition().x<<", " << g_clients[ci].player.GetPosition().y << ", " <<g_clients[ci].player.GetPosition().z <<")"<< std::endl;
	
	if (g_clients[ci].player.m_bIsActive)
	{
		float t = CGameManager::GetInstance()->m_pGameFramework->m_GameTimer.GetTimeElapsed();
		g_clients[ci].player.RotateMoveDir(t);
		/*
		if (CPhysicalCollision::IsCollided(
		&CPhysicalCollision::MoveAABB(g_clients[i].player.m_pMesh->m_AABB, g_clients[i].player.GetPosition()),
		&CPhysicalCollision::MoveAABB(g_box->m_pMesh->m_AABB, g_box->GetPosition())))
		{
		if (g_clients[i].player.m_d3dxvVelocity.y <= 0.0f && g_clients[i].player.GetPosition().y > g_box->GetPosition().y + g_box->m_pMesh->m_AABB.m_d3dxvMax.y)
		{
		//if (pPlayer->m_d3dxvVelocity.y > 0.0f)
		D3DXVECTOR3 d3dxvRevision = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		d3dxvRevision.y = g_box->GetPosition().y + g_box->m_pMesh->m_AABB.m_d3dxvMax.y - (g_clients[i].player.GetPosition().y + g_clients[i].player.m_pMesh->m_AABB.m_d3dxvMin.y);
		g_clients[i].player.Moving(d3dxvRevision);
		g_clients[i].player.m_CameraOperator.MoveCameraOperator(d3dxvRevision);
		g_clients[i].player.m_d3dxvVelocity.y = 0.0;
		if (g_clients[i].player.m_d3dxvMoveDir.y != 0.0f) g_clients[i].player.m_d3dxvVelocity.y = 4.95;
		}
		else CPhysicalCollision::ImpurseBasedCollisionResolution(&g_clients[i].player, g_box);
		}*/

		// i 번째 플레이어를 기준으로 한 플레이어 간 충격량 기반 충돌 체크
		for (int j = 0; j < MAX_USER; j++)
		{
			if (ci == j)
				continue;
			if (g_clients[ci].player.m_bIsActive)
			{
				CPhysicalCollision::ImpurseBasedCollisionResolution(&g_clients[ci].player, &g_clients[j].player);
			}
		}

		// 복셀 터레인 및 씬의 환경 변수에 기반한 충돌 체크 및 물리 움직임
		CGameManager::GetInstance()->m_pGameFramework->m_pScene->MoveObjectUnderPhysicalEnvironment(&g_clients[ci].player, t);

		if (g_clients[ci].player.GetPosition().y < -1.0f)
		{
			//printf("플레이어가 물에 빠져버렸습니다!");
			g_RespawnManager.RegisterRespawnManager(&g_clients[ci].player, true);
		}

	}

	for (int i = 0; i < MAX_USER; ++i) {
		if (true == g_clients[i].connect)
			SendPositionPacket(i, ci);
	}
}

void Worker_Thread()
{
	while (true)
	{
		ULONG io_size;
		unsigned long long ci;
		OverlappedEX *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, reinterpret_cast<PULONG_PTR>(&ci), reinterpret_cast<LPWSAOVERLAPPED *> (&over), INFINITE);
		std::cout << "GQCS :";
		if (FALSE == ret)
		{
			int err_no = WSAGetLastError();
			if (64 == err_no) DisconnectClient(ci);
			else error_display("errGQCS :", err_no);
			while (true);
		}
		if (0 == io_size)
		{
			closesocket(g_clients[ci].client_socket);
			g_clients[ci].connect = false;
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (true == g_clients[i].connect)
					SendRemovePlayerPacket(i, ci);
			}
			continue;
		}
		if (OP_RECV == over->event_type)
		{
			std::cout << "RECV from Client " << ci;
			std::cout << " IO_SIZE: " << io_size << std::endl;
			unsigned char *buf = g_clients[ci].recv_over.IOCP_buf;
			unsigned int psize = g_clients[ci].curr_packat_size;
			unsigned int pr_size = g_clients[ci].prev_packat_data;
			while (io_size > 0) {
				if (0 == psize) psize = buf[0];
				if (io_size + pr_size >= psize) {
					// 패킷완성이 가능
					unsigned char packet[MAX_PACKET_SIZE];
					memcpy(packet, g_clients[ci].packet_buf, pr_size);
					memcpy(packet + pr_size, buf, psize - pr_size);
					ProcessPacket(ci, packet);
					io_size -= psize - pr_size;
					buf += psize - pr_size;
					psize = 0; pr_size = 0;
				}
				else {
					memcpy(g_clients[ci].packet_buf + pr_size, buf, io_size);
					pr_size += io_size;
					io_size = 0;

				}
			}
			g_clients[ci].curr_packat_size = psize;
			g_clients[ci].prev_packat_data = pr_size;

			DWORD recv_flag = 0;
			WSARecv(g_clients[ci].client_socket, &g_clients[ci].recv_over.wsabuf,
				1, NULL, &recv_flag, &g_clients[ci].recv_over.over, NULL);
		}
		else if (OP_SEND == over->event_type) {
			if (io_size != over->wsabuf.len) {
				std::cout << "Send Incomplete Error\n";
				g_clients[ci].connect = false;
			}
			delete over;
		}
		else {
			std::cout << "Unkown GQCS event!\n";
			while (true);
		}
	}
}

void Create_Workker_Thread()
{
}

void Accept_Thread()
{
	while (true)
	{
		SOCKADDR_IN ClientAddr;
		ZeroMemory(&ClientAddr, sizeof(SOCKADDR_IN));
		ClientAddr.sin_family = AF_INET;
		ClientAddr.sin_port = htons(MY_SERVER_PORT);
		ClientAddr.sin_addr.s_addr = INADDR_ANY;
		int addr_size = sizeof(ClientAddr);
		SOCKET new_client = WSAAccept(g_ssocket, reinterpret_cast<sockaddr*>(&ClientAddr), &addr_size, NULL, NULL);

		if (INVALID_SOCKET == new_client)
		{
			int err_no = WSAGetLastError();
			error_display("WSAAccept: ", err_no);
		}

		int new_id = -1;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].connect == false) {
				new_id = i;
				break;
			}
		}
		if (new_id == -1)
		{
			std::cout << "MAX USER OVERFLOW!\n"; closesocket(new_client); continue;
		}

		g_clients[new_id].connect = true;
		g_clients[new_id].client_socket = new_client;
		g_clients[new_id].curr_packat_size = 0;
		g_clients[new_id].prev_packat_data = 0;
		ZeroMemory(&g_clients[new_id].recv_over, sizeof(g_clients[new_id].recv_over));
		g_clients[new_id].recv_over.event_type = OP_RECV;
		g_clients[new_id].recv_over.wsabuf.buf = reinterpret_cast<CHAR*>(g_clients[new_id].recv_over.IOCP_buf);
		g_clients[new_id].recv_over.wsabuf.len = sizeof(g_clients[new_id].recv_over.IOCP_buf);
		g_clients[new_id].team = new_id % 2;
		g_RespawnManager.RegisterRespawnManager(&g_clients[new_id].player, false);

		DWORD recv_flag = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), g_hiocp, new_id, 0);
		WSARecv(new_client, &g_clients[new_id].recv_over.wsabuf, 1, NULL, &recv_flag, &g_clients[new_id].recv_over.over, NULL);
		
		SendInitPacket(new_id);
		//SendPutPlayerPacket(new_id, new_id);
		

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].connect == true)
				if (i != new_id) {
					SendPutPlayerPacket(new_id, i);
					SendPutPlayerPacket(i, new_id);
				}
		}
	}
}

void ShutDown_Server()
{
	closesocket(g_ssocket);
	CloseHandle(g_hiocp);
	WSACleanup();
}

void Process_Thread()
{
	while (1)
	{
		g_RespawnManager.UpdateRespawnManager(CGameManager::GetInstance()->m_pGameFramework->m_GameTimer.GetTimeElapsed());
	}
}

void ServerMain()
{
	std::vector<std::thread *> worker_threads;
	Initialize_Server();
	for (int i = 0; i < 8; ++i) worker_threads.push_back(new std::thread{ Worker_Thread });
	std::thread Accept_Thread(Accept_Thread);
	Accept_Thread.join();
	//Create_Accept_Thread();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
	ShutDown_Server();
}