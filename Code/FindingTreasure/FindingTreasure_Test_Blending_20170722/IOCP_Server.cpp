#include "stdafx.h"

#include <mutex>
#include <unordered_set>
#include <chrono>
#include <queue>

#include "IOCP_Init.h"
#include "IOCP_Server.h"
#include "RespawnManager.h"

using namespace std::chrono;

HANDLE g_hiocp;
SOCKET g_ssocket;
CPlayer* g_box=NULL;
CRespawnManager g_RespawnManager;
std::mutex g_Respawnlock;

enum OPTYPE { OP_SEND, OP_RECV , USER_MOVE , SEND_SYNC };

struct OverlappedEX {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
};

struct CLIENT {
	bool team;
	bool connect;
	float xMove;
	float zMove;
	int cameraYrotate;

	high_resolution_clock::time_point last_move_time;
	bool is_active;

	CPlayer player;

	SOCKET client_socket;
	OverlappedEX recv_over;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	int prev_packat_data;
	int curr_packat_size;
	std::mutex vl_lock;
};

enum Event_Type { E_MOVE , RESPAWN_TIME , SYNC_TIME};

struct Timer_Event {
	int object_id;
	high_resolution_clock::time_point exec_time;
	Event_Type event;
};

class mycomp {
	bool reverse;
public:
	mycomp() {}
	bool operator() (const Timer_Event lhs, const Timer_Event rhs) const
	{
		return (lhs.exec_time > rhs.exec_time);
	}
};


std::priority_queue <Timer_Event, std::vector <Timer_Event>, mycomp> timer_queue;
std::mutex tq_lock;

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

void MovePlayer(int id)
{
	Timer_Event event = { id, high_resolution_clock::now() + 17ms, E_MOVE };
	tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
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
	g_Respawnlock.lock();
	g_RespawnManager.InitRespawnManager();
	g_Respawnlock.unlock();
	Timer_Event event = { -1, high_resolution_clock::now() + 10ms, RESPAWN_TIME };
	tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
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
	//std::cout << "Send Packet To Client [" << ptype << "]: " << cl << std::endl;
}

void SendInitPacket(int client)
{
	sc_packet_init packet;
	packet.id = client;
	packet.size = sizeof(packet);
	packet.type = SC_INIT;

	packet.Posx = g_clients[client].player.GetPosition().x;
	packet.Posy = g_clients[client].player.GetPosition().y;
	packet.Posz = g_clients[client].player.GetPosition().z;

	packet.Lookx = g_clients[client].player.m_CameraOperator.m_Camera.GetPosition().x;
	packet.Looky = g_clients[client].player.m_CameraOperator.m_Camera.GetPosition().y;
	packet.Lookz = g_clients[client].player.m_CameraOperator.m_Camera.GetPosition().z;

	SendPacket(client, &packet);
}


void SendPutPlayerPacket(int client, int object)
{
	sc_packet_init packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().z;
	
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
	packet.MoveX = g_clients[object].xMove;
	packet.MoveZ = g_clients[object].zMove;
	packet.CameraY = g_clients[object].cameraYrotate;
	//std::cout << "Position: " << g_clients[object].player.GetPosition().x << ", " << g_clients[object].player.GetPosition().z << std::endl;
	SendPacket(client, &packet);
}

void SendSetPositionPacket(int client, int object)
{
	sc_packet_init packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_SETPOS;
	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().z;

	SendPacket(client, &packet);
}

void SendSyncPacket(int client, int object)
{
	sc_packet_sync packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_SYNC;
	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().z;
	/*
	packet.Lookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetLook().z;
	*/
	SendPacket(client, &packet);
}

void SendRespawnPacket(int client, int object)
{
	sc_packet_respawn packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_RESPAWN;
	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.m_Camera.GetPosition().z;
	
	SendPacket(client, &packet);
}

void SendDeadPacket(int client, int object)
{
	sc_packet_dead packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_DEAD;

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
	case CS_POSMOVE:
		g_clients[ci].xMove = (int)(CHAR)packet[2];
		g_clients[ci].zMove = (int)(CHAR)packet[3];
		break;
	case CS_CAMERAMOVE:
		g_clients[ci].cameraYrotate = (int)(CHAR)packet[2];
		break;
	case CS_JUMP:
		g_clients[ci].player.m_d3dxvMoveDir.y = 5.f;
		break;
	default:
		std::cout << "Unknown Packet Type from Client" << ci << std::endl;
		while (true);
	}
	cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(packet);
	//std::cout << "Client x,y,z :" << (float)my_packet->Movex << "," << (int)my_packet->Movey << "," << (int)my_packet->Movez << std::endl;
	
	//g_clients[ci].player.SetLook(D3DXVECTOR3((float)my_packet->Lookx, g_clients[ci].player.GetLook().y, (float)my_packet->Lookz));
	//g_clients[ci].player.m_CameraOperator.SetLook(D3DXVECTOR3((float)my_packet->Lookx, g_clients[ci].player.m_CameraOperator.GetLook().y, (float)my_packet->Lookz));
	/*
	std::cout << "LookX" <<(int)my_packet->Lookx;
	std::cout << "LookZ" <<(int)my_packet->Lookz;
	std::cout << "MoveX" << (int)my_packet->Movex;
	std::cout << "MoveZ" << (int)my_packet->Movez;
	*/
	

	//std::cout <<"x,y,z :" <<(int)my_packet->Movex << "," << (int)my_packet->Movey <<"," << (int)my_packet->Movez << std::endl;
	//std::cout << "Look : "<<(float)my_packet->Lookx << ", " << (double)my_packet->Lookz << std::endl;

	//std::cout << ci <<" X Y Z : (" << g_clients[ci].player.GetPosition().x<<", " << g_clients[ci].player.GetPosition().y << ", " <<g_clients[ci].player.GetPosition().z <<")"<< std::endl;
	
	
	g_clients[ci].vl_lock.lock();
	
	for (int i = 0; i < MAX_USER; ++i) {
		if (true == g_clients[i].connect)
		{
			SendPositionPacket(i, ci);
		}
	}
	g_clients[ci].vl_lock.unlock();
}

void Worker_Thread()
{
	while (true)
	{
		ULONG io_size;
		unsigned long long ci;
		OverlappedEX *over;
		BOOL ret = GetQueuedCompletionStatus(g_hiocp, &io_size, reinterpret_cast<PULONG_PTR>(&ci), reinterpret_cast<LPWSAOVERLAPPED *> (&over), INFINITE);
		//std::cout << "GQCS :";
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
			//std::cout << "RECV from Client " << ci;
			//std::cout << " IO_SIZE: " << io_size << std::endl;
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
		else if (USER_MOVE == over->event_type)
		{
			if (!g_clients[ci].player.m_bIsActive) {
				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				MovePlayer(ci);
				delete over;
				continue;
			}

			if (!g_clients[ci].is_active && g_clients[ci].player.m_bIsActive)
			{
				for(int i =0;i<MAX_USER;++i)
					if(g_clients[i].connect)
						SendRespawnPacket(i, ci);
				g_clients[ci].vl_lock.lock();
				g_clients[ci].is_active = true;
				g_clients[ci].vl_lock.unlock();
			}

			duration<float> sec = high_resolution_clock::now() - g_clients[ci].last_move_time;
			
			float t = sec.count();
			if (g_clients[ci].connect == false)
				continue;

			g_clients[ci].vl_lock.lock();
			g_clients[ci].player.m_d3dxvMoveDir.x += (float)g_clients[ci].xMove;
			g_clients[ci].player.m_d3dxvMoveDir.z += (float)g_clients[ci].zMove;

			g_clients[ci].player.m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC * g_clients[ci].cameraYrotate, t);
			
			g_clients[ci].player.RotateMoveDir(t);

			

			g_clients[ci].vl_lock.unlock();
			//std::cout <<"m_d3dxvMoveDirX" << (float)g_clients[ci].player.GetPosition().x << std::endl;
			//std::cout << "m_d3dxvMoveDirZ"<< (float)g_clients[ci].player.GetPosition().z << std::endl;
			if (g_clients[ci].player.m_bIsActive)
			{
				/*if (CPhysicalCollision::IsCollided(
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
				for (int i = 0; i < MAX_USER; i++)
				{
					if (i == ci)
						continue;
					if (g_clients[i].connect == false)
						continue;
					if (g_clients[ci].player.m_bIsActive)
					{
						CPlayer PlayerA, PlayerB;
						g_clients[ci].vl_lock.lock(); 
						PlayerA = g_clients[ci].player;
						g_clients[ci].vl_lock.unlock();
						g_clients[i].vl_lock.lock();
						PlayerB = g_clients[i].player;
						g_clients[i].vl_lock.unlock();
						CPhysicalCollision::ImpurseBasedCollisionResolution(&PlayerA, &PlayerB);
						g_clients[ci].vl_lock.lock();
						g_clients[ci].player = PlayerA;
						g_clients[ci].vl_lock.unlock();
						g_clients[i].vl_lock.lock();
						g_clients[i].player = PlayerB;
						g_clients[i].vl_lock.unlock();
					}
				}
				// 복셀 터레인 및 씬의 환경 변수에 기반한 충돌 체크 및 물리 움직임
				g_clients[ci].vl_lock.lock();
				CGameManager::GetInstance()->m_pGameFramework->m_pScene->MoveObjectUnderPhysicalEnvironment(&g_clients[ci].player, t);
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();

				if (g_clients[ci].player.GetPosition().y < -1.0f)
				{
					printf("서버에서 플레이어가 물에 빠져버렸습니다!");
					g_clients[ci].vl_lock.lock();
					g_clients[ci].is_active = false;
					g_clients[ci].vl_lock.unlock();
					g_Respawnlock.lock();
					g_RespawnManager.RegisterRespawnManager(&g_clients[ci].player, true);
					g_Respawnlock.unlock();
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (g_clients[i].connect)
							SendDeadPacket(i, ci);
					}
				}
			}
			/*
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (g_clients[i].connect == false)
					continue;
				SendPositionPacket(i, ci);
				//PostQueuedCompletionStatus(g_hiocp, 0, j, reinterpret_cast<LPOVERLAPPED>(&g_clients[j].recv_over.over));
			}*/

			MovePlayer(ci);
			delete over;
		}
		else if (SEND_SYNC == over->event_type)
		{
			if (!g_clients[ci].player.m_bIsActive)
			{
				Timer_Event event = { ci, high_resolution_clock::now() + 500ms, SYNC_TIME };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
				delete over;
				continue;
			}
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (!g_clients[i].connect)
					continue;
				SendSyncPacket(i, ci);
			}
			Timer_Event event = { ci, high_resolution_clock::now() + 500ms, SYNC_TIME };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			delete over;
		}
		else {
			std::cout << "Unkown GQCS event!\n";
			while (true);
		}
	}
}

void Time_Thread()
{
	for (;;) {
		Sleep(10);

		for (;;) {
			//std::cout << "타임스레드들어오냐?" << std::endl;
			tq_lock.lock();
			if (0 == timer_queue.size()) {
				tq_lock.unlock();  break;
			}
			Timer_Event t = timer_queue.top();
			if (t.exec_time > high_resolution_clock::now()) {
				tq_lock.unlock();  break;
			}
			timer_queue.pop();
			tq_lock.unlock();
			OverlappedEX *over = new OverlappedEX;
			if (E_MOVE == t.event) {
				over->event_type = USER_MOVE;
				PostQueuedCompletionStatus(g_hiocp, 1, t.object_id, &over->over);
			}
			else if (RESPAWN_TIME == t.event)
			{
				g_RespawnManager.UpdateRespawnManager(0.01f);
				Timer_Event event = { -1, high_resolution_clock::now() + 1ms, RESPAWN_TIME };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
				//printf("스폰시간지나간다.");
			}
			else if (SYNC_TIME == t.event) {
				over->event_type = SEND_SYNC;
				PostQueuedCompletionStatus(g_hiocp, 1, t.object_id, &over->over);
			}
		}
	}
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
		
		

		DWORD recv_flag = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), g_hiocp, new_id, 0);
		WSARecv(new_client, &g_clients[new_id].recv_over.wsabuf, 1, NULL, &recv_flag, &g_clients[new_id].recv_over.over, NULL);
		g_Respawnlock.lock();
		g_RespawnManager.RegisterRespawnManager(&g_clients[new_id].player, false);
		g_Respawnlock.unlock();
		float fx = 0.2f, fy = 0.45f, fz = 0.2f;
		g_clients[new_id].player.m_pMesh = new CTexturedLightingPirateMesh(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDevice);
		g_clients[new_id].player.InitCameraOperator();
		g_clients[new_id].xMove = 0;
		g_clients[new_id].zMove = 0;
		g_clients[new_id].cameraYrotate = 0;
		g_clients[new_id].is_active = true;
		g_clients[new_id].last_move_time = high_resolution_clock::now();
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].connect == true)
				if (i != new_id) {
					SendPutPlayerPacket(new_id, i);
					SendPutPlayerPacket(i, new_id);
				}
		}

		SendInitPacket(new_id);
		//SendPutPlayerPacket(new_id, new_id);

		MovePlayer(new_id);

		Timer_Event event = { new_id, high_resolution_clock::now() + 500ms, SYNC_TIME };
		tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].connect == true)
			SendRespawnPacket(i, new_id);
				
		}

		std::cout << "new client accept: " << new_id << std::endl;
	}
}

void ShutDown_Server()
{
	closesocket(g_ssocket);
	CloseHandle(g_hiocp);
	WSACleanup();
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