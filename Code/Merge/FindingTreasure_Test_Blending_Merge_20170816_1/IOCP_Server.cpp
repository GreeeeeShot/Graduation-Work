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
CPlayer* g_box = NULL;
CRespawnManager g_RespawnManager;
bool GameStart = false;
bool Close_Server = false;
//std::mutex g_Respawnlock;
int red, blue;
float g_GameTime = 300;
int g_SelectMap = 0;

enum OPTYPE { OP_SEND, OP_RECV, USER_MOVE, SEND_SYNC, BOX_MOVE, BOX_SYNC , WAS };

struct OverlappedEX {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
};

struct CLIENT {
	bool connect;
	int MoveX;
	int MoveZ;

	int cameraYrotate;
	int cameraXrotate;

	bool cameraFar;
	bool VoxDelOrIns;
	bool ready;
	bool throwbox;

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

struct BOX {
	CPlayer player;
	high_resolution_clock::time_point last_move_time;
	std::mutex vl_lock;
};


enum Event_Type { E_MOVE, RESPAWN_TIME, SYNC_TIME, BOX_EVENT, BOX_SYNC_TIME, CLOSE_SOCKET, GAME_TIME_M ,HOST_CLOSE , IN_GAME_TIME };

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
BOX g_TreasureBox;
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
	Timer_Event event = { id, high_resolution_clock::now() + 23ms, E_MOVE };
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
		//g_clients[i].player.SetMesh(CMeshResource::pPirateMesh);
		//g_clients[i].player.SetBelongType(red < blue ? BELONG_TYPE_BLUE : BELONG_TYPE_RED);
	}
	blue = 0;
	red = 0;

	g_RespawnManager.InitRespawnManager();
	Timer_Event event = { -1, high_resolution_clock::now() + 100ms, RESPAWN_TIME };
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

	packet.Lookx = g_clients[client].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[client].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[client].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[client].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[client].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[client].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[client].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[client].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[client].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[client].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[client].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[client].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[client].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[client].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[client].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[client].player.m_CameraOperator.GetRight().z;

	SendPacket(client, &packet);
}

void SendJumpPacket(int client, int object)
{
	sc_packet_jump packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_JUMP;

	SendPacket(client, &packet);
}

void SendPutPlayerPacket(int client, int object)
{
	sc_packet_put_player packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.team = g_clients[object].player.m_BelongType;

	SendPacket(client, &packet);
}

void SendInstallVoxel(int client, int object, int pocket)
{
	sc_packet_vox packet;
	packet.size = sizeof(packet);
	packet.type = SC_INSVOX;
	packet.id = object;
	packet.pocket = pocket;

	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

	SendPacket(client, &packet);
}

void SendDeleteVoxel(int client, int object, int pocket)
{
	sc_packet_vox packet;
	packet.size = sizeof(packet);
	packet.type = SC_DELVOX;
	packet.id = object;
	packet.pocket = pocket;

	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

	SendPacket(client, &packet);
}

void SendLiftBox(int client, int object)
{
	sc_packet_lift packet;
	packet.size = sizeof(packet);
	packet.type = SC_LIFTBOX;
	packet.id = object;

	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

	SendPacket(client, &packet);
}

void SendLiftEnd(int client, int object)
{
	sc_packet_lift packet;
	packet.size = sizeof(packet);
	packet.type = SC_OUTBOX;
	packet.id = object;

	packet.Posx = g_clients[object].player.GetPosition().x;
	packet.Posy = g_clients[object].player.GetPosition().y;
	packet.Posz = g_clients[object].player.GetPosition().z;

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

	SendPacket(client, &packet);
}


void SendCancleVoxel(int client, int object)
{
	sc_packet_voxcancle packet;
	packet.size = sizeof(packet);
	packet.type = SC_CANCLEVOX;
	packet.id = object;

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
	packet.MoveX = g_clients[object].MoveX;
	packet.MoveZ = g_clients[object].MoveZ;
	packet.CameraY = g_clients[object].cameraYrotate;
	packet.CameraX = g_clients[object].cameraXrotate;

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

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

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

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;

	packet.Velx = g_clients[object].player.m_d3dxvVelocity.x;
	packet.Vely = g_clients[object].player.m_d3dxvVelocity.y;
	packet.Velz = g_clients[object].player.m_d3dxvVelocity.z;

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

	packet.Lookx = g_clients[object].player.m_CameraOperator.GetPosition().x;
	packet.Looky = g_clients[object].player.m_CameraOperator.GetPosition().y;
	packet.Lookz = g_clients[object].player.m_CameraOperator.GetPosition().z;

	packet.Operx = g_clients[object].player.m_CameraOperator.GetOperatorPosition().x;
	packet.Opery = g_clients[object].player.m_CameraOperator.GetOperatorPosition().y;
	packet.Operz = g_clients[object].player.m_CameraOperator.GetOperatorPosition().z;

	packet.Dirx = g_clients[object].player.m_CameraOperator.GetDir().x;
	packet.Diry = g_clients[object].player.m_CameraOperator.GetDir().y;
	packet.Dirz = g_clients[object].player.m_CameraOperator.GetDir().z;

	packet.distance = g_clients[object].player.m_CameraOperator.GetDistance();

	packet.oLookx = g_clients[object].player.m_CameraOperator.GetLook().x;
	packet.oLooky = g_clients[object].player.m_CameraOperator.GetLook().y;
	packet.oLookz = g_clients[object].player.m_CameraOperator.GetLook().z;

	packet.Rightx = g_clients[object].player.m_CameraOperator.GetRight().x;
	packet.Righty = g_clients[object].player.m_CameraOperator.GetRight().y;
	packet.Rightz = g_clients[object].player.m_CameraOperator.GetRight().z;


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

void SendTeamPacket(int client, int team)
{
	sc_packet_initteam packet;
	packet.id = client;
	packet.team = team;
	packet.size = sizeof(packet);
	packet.type = SC_INIT_TEAM;
	packet.map = g_SelectMap;

	SendPacket(client, &packet);
}



void SendCharaChangePacket(int client, int object)
{
	sc_packet_waiting packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_CHARAC_CHANGE;

	SendPacket(client, &packet);
}

void SendTeamChangePacket(int client, int object)
{
	sc_packet_waiting packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_TEAM_CHANGE;

	SendPacket(client, &packet);
}

void SendReadyPacket(int client, int object)
{
	sc_packet_waiting packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_READY;

	SendPacket(client, &packet);
}

void SendExitPacket(int client, int object)
{
	sc_packet_waiting packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_EXIT;

	SendPacket(client, &packet);
}

void SendStartPacket(int client)
{
	sc_packet_waiting packet;
	packet.size = sizeof(packet);
	packet.type = SC_GAME_START;

	SendPacket(client, &packet);
}

void SendBoxPacket(int client)
{
	sc_packet_box packet;
	packet.size = sizeof(packet);
	packet.type = SC_BOX;

	packet.Posx = g_TreasureBox.player.GetPosition().x;
	packet.Posy = g_TreasureBox.player.GetPosition().y;
	packet.Posz = g_TreasureBox.player.GetPosition().z;
	/*
	packet.Posx = g_TreasureBox.player.m_d3dxvVelocity.x;
	packet.Posy = g_TreasureBox.player.m_d3dxvVelocity.y;
	packet.Posz = g_TreasureBox.player.m_d3dxvVelocity.z;
	*/
	SendPacket(client, &packet);
}

void SendBoxMissPacket(int client, int object)
{
	sc_packet_missbox packet;
	packet.size = sizeof(packet);
	packet.type = SC_MISSBOX;
	packet.id = object;

	packet.Posx = g_TreasureBox.player.GetPosition().x;
	packet.Posy = g_TreasureBox.player.GetPosition().y;
	packet.Posz = g_TreasureBox.player.GetPosition().z;

	SendPacket(client, &packet);
}

void SendWinPacket(int client)
{
	sc_packet_result packet;
	packet.size = sizeof(packet);
	packet.type = SC_WIN;
	SendPacket(client, &packet);
}

void SendDrawPacket(int client)
{
	sc_packet_result packet;
	packet.size = sizeof(packet);
	packet.type = SC_DRAW;
	SendPacket(client, &packet);
}

void SendDefeatPacket(int client)
{
	sc_packet_result packet;
	packet.size = sizeof(packet);
	packet.type = SC_DEFEAT;
	SendPacket(client, &packet);
}

void SendGameTimePacket(int client, int gametime)
{
	sc_packet_gametime packet;
	packet.size = sizeof(packet);
	packet.type = SC_GAMETIME;
	packet.time = gametime;

	SendPacket(client, &packet);
}

void SendThrowBox(int client, int object)
{
	sc_packet_throwbox packet;
	packet.size = sizeof(packet);
	packet.type = SC_THROWBOX;
	packet.id = object;

	SendPacket(client, &packet);
}

void SendMapChangePacket(int client)
{
	sc_packet_waiting packet;
	packet.size = sizeof(packet);
	packet.type = SC_MAPCHANGE;

	SendPacket(client, &packet);
}

void SendCameraFar(int client, int object)
{
	sc_packet_camerafar packet;
	packet.size = sizeof(packet);
	packet.type = SC_CAMERAFAR;
	packet.id = object;
	packet.cf = g_clients[object].cameraFar;

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
	Timer_Event event;
	switch (packet[1])
	{
	case CS_POSMOVE:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].MoveX = (int)(CHAR)packet[2];
		g_clients[ci].MoveZ = (int)(CHAR)packet[3];

		for (int i = 0; i < MAX_USER; ++i) {
			if (true == g_clients[i].connect)
			{
				if (i != ci)
					SendPositionPacket(i, ci);
			}
		}
		g_clients[ci].vl_lock.unlock();
		break;
	case CS_CAMERAMOVE:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].cameraYrotate = (int)(CHAR)packet[2];
		for (int i = 0; i < MAX_USER; ++i) {
			if (true == g_clients[i].connect)
			{
				if (i != ci)
					SendPositionPacket(i, ci);
			}
		}
		g_clients[ci].vl_lock.unlock();
		break;
	case CS_CAMERAXMOVE:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].cameraXrotate = (int)(CHAR)packet[2];
		for (int i = 0; i < MAX_USER; ++i) {
			if (true == g_clients[i].connect)
			{
				if (i != ci)
					SendPositionPacket(i, ci);
			}
		}
		g_clients[ci].vl_lock.unlock();
		break;
	case CS_CAMERAFAR:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].cameraFar = (int)(CHAR)packet[2];
		for (int i = 0; i < MAX_USER; ++i) {
			if (true == g_clients[i].connect)
			{
				if (i != ci)
					SendCameraFar(i, ci);
			}
		}
		g_clients[ci].vl_lock.unlock();
		break;
	case CS_JUMP:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].player.m_d3dxvMoveDir.y = 5.f;
		g_clients[ci].vl_lock.unlock();
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendJumpPacket(i, ci);
		}
		break;
	case CS_INSVOX:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendInstallVoxel(i, ci, packet[2]);
		}
		break;
	case CS_DELVOX:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendDeleteVoxel(i, ci, packet[2]);
		}
		break;
	case CS_CANCLEVOX:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendCancleVoxel(i, ci);
		}
		break;
	case CS_CHARAC_CHANGE:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendCharaChangePacket(i, ci);
		}
		break;
	case CS_MAPCHANGE:
		g_SelectMap = (g_SelectMap + 1) % 2;
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendMapChangePacket(i);
		}
		break;
	case CS_TEAM_CHANGE:
		g_clients[ci].player.m_BelongType = (BELONG_TYPE)((g_clients[ci].player.m_BelongType + 1) % 2);
		if (g_clients[ci].player.m_BelongType)
		{
			red++;
			blue--;
		}
		else
		{
			red--;
			blue++;
		}
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendTeamChangePacket(i, ci);
		}
		break;
	case CS_READY:
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendReadyPacket(i, ci);
		}
		g_clients[ci].ready = (g_clients[ci].ready + 1) % 2;
		break;
	case CS_EXIT:
		if (ci == 0)
		{
			event = { -1, high_resolution_clock::now(), HOST_CLOSE };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			break;
			//ShutDown_Server();
		}
		g_clients[ci].connect = false;
		if (g_clients[ci].player.m_BelongType) red--;
		else blue--;

		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendExitPacket(i, ci);
		}
		event = { -1, high_resolution_clock::now(), CLOSE_SOCKET };
		tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
		break;
	case CS_LIFTBOX:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].player.m_IsLift = true;
		g_clients[ci].vl_lock.unlock();
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendLiftBox(i, ci);
		}
		break;
	case CS_OUTBOX:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].player.m_IsLift = false;
		g_clients[ci].vl_lock.unlock();
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
				SendLiftEnd(i, ci);
		}
		break;
	case CS_THROWBOX:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].throwbox = true;
		g_clients[ci].vl_lock.unlock();
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			if (i != ci)
			{
				SendThrowBox(i, ci);
			}
		}
		break;
	case CS_GAMEREADY:
		g_clients[ci].vl_lock.lock();
		g_clients[ci].player.m_pMesh = new CTexturedLightingPirateMesh(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDevice);
		g_clients[ci].player.m_fJumpdVelYM = 4.97f;   //Y축에 대한 순간 점프 속도 크기
		g_clients[ci].player.m_fMaxRunVelM = 5.0f;   // 달리는 속도에 대한 크기
		g_clients[ci].player.m_fMass = 40.0f;               // 질량
		g_clients[ci].player.m_fRecoveryStaminaPerSec = 40.0f;
		g_clients[ci].player.m_fDecrementStaminaPerSec = 12.0f;
		g_clients[ci].player.InitCameraOperator();
		g_clients[ci].ready = false;
		g_clients[ci].vl_lock.unlock();

		g_RespawnManager.RegisterRespawnManager(&g_clients[ci].player, false);

		MovePlayer(ci);
		event = { ci, high_resolution_clock::now() + 200ms, SYNC_TIME };
		tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
		SendInitPacket(ci);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (!g_clients[i].connect)
				continue;
			SendRespawnPacket(i, ci);
		}

		if (ci == 0)
		{
			g_GameTime = 300;

			g_TreasureBox.vl_lock.lock();
			//g_TreasureBox.player.m_pMesh = new TreasureChestMesh(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDevice);
			g_TreasureBox.player.m_pMesh = new CTexturedLightingCubeMesh(CGameManager::GetInstance()->m_pGameFramework->m_pd3dDevice, 1.5f, 0.5f, 1.5f);

			g_TreasureBox.player.m_fJumpdVelYM = 0.0f;			//Y축에 대한 순간 점프 속도 크기 // 보물상자는 스스로 점프하지 못한다.
			g_TreasureBox.player.m_fMaxRunVelM = 15.0f;			// 밀리는 속도에 대한 최대 크기
			g_TreasureBox.player.m_fMass = 40.0f;               // 질량

			g_TreasureBox.player.SetPosition(D3DXVECTOR3(0.f, 10.f, 0.f));
			g_TreasureBox.vl_lock.unlock();



			event = { -1, high_resolution_clock::now() + 33ms, BOX_EVENT };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			event = { -1, high_resolution_clock::now() + 200ms, BOX_SYNC_TIME };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			event = { -1, high_resolution_clock::now() + 100ms, IN_GAME_TIME };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();

			//GameStart = true;
		}
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
			continue;
			//while (true);
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
			if (!g_TreasureBox.player.m_pMesh)
			{
				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				if (GameStart)
				{
					MovePlayer(ci);
				}
				delete over;
				continue;
			}
			if (!CGameManager::GetInstance()->m_pGameFramework->m_pScene)
			{
				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				if (GameStart)
				{
					MovePlayer(ci);
				}
				delete over;
				continue;
			}
			if (!CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pVoxelTerrain->m_ppVoxelObjectsForCollision)
			{
				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				if (GameStart)
				{
					MovePlayer(ci);
				}
				delete over;
				continue;
			}
			if (!g_clients[ci].player.m_bIsActive) {
				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				if (GameStart)
				{
					MovePlayer(ci);
				}
				delete over;
				continue;
			}

			if (g_clients[ci].connect == false)
				continue;
			CPlayer currentclient;
			float x, z;
			int cameraY, cameraX;
			g_clients[ci].vl_lock.lock();
			currentclient = g_clients[ci].player;
			x = g_clients[ci].MoveX;
			z = g_clients[ci].MoveZ;
			cameraY = g_clients[ci].cameraYrotate;
			cameraX = g_clients[ci].cameraXrotate;
			g_clients[ci].vl_lock.unlock();
			if (!g_clients[ci].is_active && currentclient.m_bIsActive)
			{
				for (int i = 0; i<MAX_USER; ++i)
					if (g_clients[i].connect)
					{
						g_clients[ci].vl_lock.lock();
						SendRespawnPacket(i, ci);
						g_clients[ci].vl_lock.unlock();
					}
				g_clients[ci].vl_lock.lock();
				g_clients[ci].is_active = true;
				g_clients[ci].player.m_fJumpdVelYM = 4.97f;   //Y축에 대한 순간 점프 속도 크기
				g_clients[ci].player.m_fMaxRunVelM = 5.0f;   // 달리는 속도에 대한 크기
				g_clients[ci].player.m_fMass = 40.0f;               // 질량
				g_clients[ci].player.m_fRecoveryStaminaPerSec = 40.0f;
				g_clients[ci].player.m_fDecrementStaminaPerSec = 12.0f;
				g_clients[ci].vl_lock.unlock();
			}
			g_clients[ci].vl_lock.lock();
			duration<float> sec = high_resolution_clock::now() - g_clients[ci].last_move_time;
			g_clients[ci].vl_lock.unlock();

			float t = sec.count();


			currentclient.m_d3dxvMoveDir.x += x;
			currentclient.m_d3dxvMoveDir.z += z;

			currentclient.m_CameraOperator.RotateLocalY(CAMERA_ROTATION_DEGREE_PER_SEC * cameraY, t);
			currentclient.m_CameraOperator.RotateLocalX(CAMERA_ROTATION_DEGREE_PER_SEC * cameraX, t);
			currentclient.m_CameraOperator.OriginalZoomState();
			if(g_clients[ci].cameraFar)
				currentclient.m_CameraOperator.ZoomOutAtOnce(ZOOM_AT_ONCE_DISTANCE);
			
			//else
				//currentclient.m_CameraOperator.ZoomOutAtOnce(-ZOOM_AT_ONCE_DISTANCE);
			currentclient.RotateMoveDir(t);
			//std::cout <<"m_d3dxvMoveDirX" << (float)g_clients[ci].player.GetPosition().x << std::endl;
			//std::cout << "m_d3dxvMoveDirZ"<< (float)g_clients[ci].player.GetPosition().z << std::endl;
			

			if (currentclient.m_bIsActive)
			{
				CPlayer box;
				g_TreasureBox.vl_lock.lock();
				box = g_TreasureBox.player;
				g_TreasureBox.vl_lock.unlock();

				/*
				AABB playeraabb = CPhysicalCollision::MoveAABB(currentclient.m_pMesh->m_AABB, currentclient.GetPosition());
				AABB boxaabb = CPhysicalCollision::MoveAABB(box.m_pMesh->m_AABB, box.GetPosition());


				printf("Player: MIN(%f %f %f) MAX(%f %f %f)  BOX:MIN(%f %f %f) MAX(%f %f %f)\n",
				playeraabb.m_d3dxvMin.x, playeraabb.m_d3dxvMin.y, playeraabb.m_d3dxvMin.z, playeraabb.m_d3dxvMax.x, playeraabb.m_d3dxvMax.y, playeraabb.m_d3dxvMax.z,
				boxaabb.m_d3dxvMin.x, boxaabb.m_d3dxvMin.y, boxaabb.m_d3dxvMin.z, boxaabb.m_d3dxvMax.x, boxaabb.m_d3dxvMax.y, boxaabb.m_d3dxvMax.z);
				*/
				if (CPhysicalCollision::IsCollided(
					&CPhysicalCollision::MoveAABB(currentclient.m_pMesh->m_AABB, currentclient.GetPosition()),
					&CPhysicalCollision::MoveAABB(box.m_pMesh->m_AABB, box.GetPosition())))
				{
					CPhysicalCollision::ImpurseBasedCollisionResolution(&currentclient, &box);
					box.BeRelievedFromLiftingPlayer();
				}
				g_TreasureBox.vl_lock.lock();
				g_TreasureBox.player = box;
				g_TreasureBox.vl_lock.unlock();
				// i 번째 플레이어를 기준으로 한 플레이어 간 충격량 기반 충돌 체크
				for (int i = 0; i < MAX_USER; i++)
				{
					if (i == ci)
						continue;
					if (g_clients[i].connect == false)
						continue;
					if (currentclient.m_bIsActive)
					{
						CPlayer PlayerA;
						g_clients[ci].vl_lock.lock();
						PlayerA = g_clients[i].player;
						g_clients[ci].vl_lock.unlock();

						CPhysicalCollision::ImpurseBasedCollisionResolution(&currentclient, &PlayerA);

						g_clients[i].vl_lock.lock();
						g_clients[i].player = PlayerA;
						g_clients[i].vl_lock.unlock();
					}
				}
				// 복셀 터레인 및 씬의 환경 변수에 기반한 충돌 체크 및 물리 움직임
				CGameManager::GetInstance()->m_pGameFramework->m_pScene->MoveObjectUnderPhysicalEnvironment(&currentclient, t);
				currentclient.m_CameraOperator.GenerateViewMatrix(t, true);

				g_clients[ci].vl_lock.lock();
				g_clients[ci].last_move_time = high_resolution_clock::now();
				g_clients[ci].vl_lock.unlock();
				/*
				if (g_clients[ci].player.GetPosition().y < -10.f);
				{
				g_clients[ci].vl_lock.lock();
				g_clients[ci].player.SetPosition(D3DXVECTOR3(g_clients[ci].player.GetPosition().x, 10.f, g_clients[ci].player.GetPosition().z));
				g_clients[ci].player.m_d3dxvVelocity.y = 0;
				g_clients[ci].vl_lock.unlock();
				}
				*/

				g_TreasureBox.vl_lock.lock();
				g_TreasureBox.player = box;
				g_TreasureBox.vl_lock.unlock();

				g_clients[ci].vl_lock.lock();
				g_clients[ci].player = currentclient;
				g_clients[ci].vl_lock.unlock();
				if (g_clients[ci].player.m_IsLift && !g_clients[ci].player.m_bIsPushed)
				{
					if (g_clients[ci].player.m_fMass > 40.0f)
					{
						g_clients[ci].vl_lock.lock();
						g_clients[ci].player.m_fJumpdVelYM = 4.97f;
						g_clients[ci].player.m_fMaxRunVelM = 5.0f;   
						g_clients[ci].player.m_fMass = 40.0f;               
						g_clients[ci].vl_lock.unlock();
					}
					g_TreasureBox.vl_lock.lock();
					g_clients[ci].player.LiftPlayer(&g_TreasureBox.player, CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pVoxelTerrain);
					g_TreasureBox.vl_lock.unlock();

					g_clients[ci].vl_lock.lock();
					g_clients[ci].player.m_bIsPushed = true;
					g_clients[ci].vl_lock.unlock();

				}
				else if (!g_clients[ci].player.m_IsLift && g_TreasureBox.player.m_pLiftingPlayer&&g_clients[ci].player.m_pLiftedPlayer)
				{
					g_clients[ci].vl_lock.lock();
					g_clients[ci].player.m_bIsPushed = false;
					g_clients[ci].player.m_pLiftedPlayer = NULL;
					g_clients[ci].vl_lock.unlock();
					g_TreasureBox.vl_lock.lock();
					g_TreasureBox.player.BeRelievedFromLiftingPlayer();
					g_TreasureBox.vl_lock.unlock();
				}
				else if (!g_clients[ci].player.m_IsLift && !g_TreasureBox.player.m_pLiftingPlayer&&g_clients[ci].player.m_pLiftedPlayer)
				{
					g_clients[ci].vl_lock.lock();
					g_clients[ci].player.m_pLiftedPlayer = NULL;
					g_clients[ci].vl_lock.unlock();
				}
				else if (g_clients[ci].player.m_IsLift && !g_TreasureBox.player.m_pLiftingPlayer&&g_clients[ci].player.m_pLiftedPlayer)
				{
					g_clients[ci].vl_lock.lock();
					g_clients[ci].player.m_pLiftedPlayer = NULL;
					g_clients[ci].player.m_bIsPushed = false;
					g_clients[ci].vl_lock.unlock();
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (!g_clients[i].connect)
							continue;
						SendBoxMissPacket(i, ci);
					}
				}
				else if (!g_clients[ci].player.m_IsLift && g_clients[ci].player.m_bIsPushed)
				{
					g_clients[ci].vl_lock.lock();
					g_clients[ci].player.m_bIsPushed = false;
					g_clients[ci].vl_lock.unlock();
				}

				g_TreasureBox.vl_lock.lock();
				box = g_TreasureBox.player;
				g_TreasureBox.vl_lock.unlock();


				g_clients[ci].vl_lock.lock();
				currentclient = g_clients[ci].player;
				g_clients[ci].vl_lock.unlock();
				
				if (g_clients[ci].throwbox && g_clients[ci].player.m_IsLift)
				{
					g_TreasureBox.vl_lock.lock();
					g_clients[ci].player.ThrowPlayer();
					g_TreasureBox.vl_lock.unlock();
					g_clients[ci].vl_lock.lock();
					g_clients[ci].throwbox = false;
					g_clients[ci].vl_lock.unlock();
				}

				g_clients[ci].vl_lock.lock();
				g_clients[ci].player = currentclient;
				g_clients[ci].vl_lock.unlock();

				/*
				if (g_TreasureBox.player.m_pLiftingPlayer)
				{
				printf("들렸다!");
				}
				else {
				printf("바닥에 있다!");
				}*/

				


				

				if (g_clients[ci].player.GetPosition().y < -1.0f)
				{
					if (g_TreasureBox.player.m_pLiftingPlayer)
					{
						g_clients[ci].vl_lock.lock();
						g_clients[ci].player.m_bIsPushed = false;
						g_clients[ci].player.m_pLiftedPlayer = NULL;
						g_clients[ci].vl_lock.unlock();
						g_TreasureBox.vl_lock.lock();
						g_TreasureBox.player.BeRelievedFromLiftingPlayer();
						g_TreasureBox.vl_lock.unlock();
					}
					//printf("서버에서 플레이어가 물에 빠져버렸습니다!");
					g_clients[ci].vl_lock.lock();
					g_clients[ci].is_active = false;
					g_clients[ci].player.m_fJumpdVelYM = 4.97f;   //Y축에 대한 순간 점프 속도 크기
					g_clients[ci].player.m_fMaxRunVelM = 5.0f;   // 달리는 속도에 대한 크기
					g_clients[ci].player.m_fMass = 40.0f;               // 질량
					g_clients[ci].player.m_fRecoveryStaminaPerSec = 40.0f;
					g_clients[ci].player.m_fDecrementStaminaPerSec = 12.0f;
					g_clients[ci].vl_lock.unlock();
					g_RespawnManager.RegisterRespawnManager(&g_clients[ci].player, true);

					currentclient.m_bIsActive = false;
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (g_clients[i].connect)
							SendDeadPacket(i, ci);
					}
				}
				g_TreasureBox.vl_lock.lock();
				box = g_TreasureBox.player;
				g_TreasureBox.vl_lock.unlock();
				for (int i = 0; i < 2; i++)
				{
					if (CPhysicalCollision::IsCollided(
						&CPhysicalCollision::MoveAABB(CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pShips[i]->m_pMesh->m_AABB, CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pShips[i]->GetPosition()),
						&CPhysicalCollision::MoveAABB(box.m_pMesh->m_AABB, box.GetPosition())))
					{
						if (box.GetPosition().y > CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pShips[i]->GetPosition().y + CGameManager::GetInstance()->m_pGameFramework->m_pScene->m_pShips[i]->m_pMesh->m_AABB.m_d3dxvMax.y)
						{
							//printf("%d 팀이 승리하였습니다.\n", i);
							for (int j = 0; j < MAX_USER; ++j)
							{
								if (!g_clients[j].connect)
									continue;
								if (g_clients[j].player.m_BelongType == i)
								{
									SendWinPacket(j);
								}
								else
								{
									SendDefeatPacket(j);
								}
								g_clients[j].ready = false;
							}
							GameStart = false;
						}
					}
				}

				g_TreasureBox.vl_lock.lock();
				g_TreasureBox.player = box;
				g_TreasureBox.vl_lock.unlock();

			}

			/*
			for (int i = 0; i < MAX_USER; ++i)
			{
			if (g_clients[i].connect == false)
			continue;
			SendPositionPacket(i, ci);
			//PostQueuedCompletionStatus(g_hiocp, 0, j, reinterpret_cast<LPOVERLAPPED>(&g_clients[j].recv_over.over));
			}*/
			g_clients[ci].vl_lock.lock();
			g_clients[ci].player = currentclient;
			g_clients[ci].vl_lock.unlock();
			if (GameStart)
			{
				MovePlayer(ci);
			}
			delete over;
		}
		else if (SEND_SYNC == over->event_type)
		{
			/*
			if (!g_clients[ci].player.m_bIsActive)
			{
			Timer_Event event = { ci, high_resolution_clock::now() + 200ms, SYNC_TIME };
			tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			delete over;
			continue;
			}
			*/
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (!g_clients[i].connect)
					continue;
				g_clients[ci].vl_lock.lock();
				SendSyncPacket(i, ci);
				g_clients[ci].vl_lock.unlock();
			}
			if (GameStart)
			{
				Timer_Event event = { ci, high_resolution_clock::now() + 200ms, SYNC_TIME };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			}
			delete over;
		}
		else if (BOX_MOVE == over->event_type)
		{
			g_TreasureBox.vl_lock.lock();
			duration<float> sec = high_resolution_clock::now() - g_TreasureBox.last_move_time;
			g_TreasureBox.vl_lock.unlock();

			float t = sec.count();

			if(CGameManager::GetInstance()->m_pGameFramework) CGameManager::GetInstance()->m_pGameFramework->m_pScene->MoveObjectUnderPhysicalEnvironment(&g_TreasureBox.player, t);
			g_TreasureBox.vl_lock.lock();
			g_TreasureBox.player.BeDraggedAwayByLiftingPlayer(t);
			if (g_TreasureBox.player.m_d3dxvVelocity.y < -10.f)
				g_TreasureBox.player.m_d3dxvVelocity.y = 1.f;
			if (g_TreasureBox.player.GetPosition().y < -4.f)
				g_TreasureBox.player.SetPosition(D3DXVECTOR3(g_TreasureBox.player.GetPosition().x, 10.f, g_TreasureBox.player.GetPosition().z));
			else if (g_TreasureBox.player.GetPosition().y < -1.f)
				g_TreasureBox.player.SetPosition(D3DXVECTOR3(g_TreasureBox.player.GetPosition().x, 0.f, g_TreasureBox.player.GetPosition().z));
			g_TreasureBox.last_move_time = high_resolution_clock::now();
			g_TreasureBox.vl_lock.unlock();

			/*if(g_TreasureBox.player.m_pLiftingPlayer)
			printf("플레이어의 위치 : %f %f %f\n", g_TreasureBox.player.m_pLiftingPlayer->GetPosition().x, g_TreasureBox.player.m_pLiftingPlayer->GetPosition().y, g_TreasureBox.player.m_pLiftingPlayer->GetPosition().z);
			*/
			if (GameStart)
			{
				Timer_Event event = { -1, high_resolution_clock::now() + 23ms, BOX_EVENT };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			}
			delete over;
		}
		else if (BOX_SYNC == over->event_type)
		{
			for (int i = 0; i < MAX_USER; ++i)
			{
				if (!g_clients[i].connect)
					continue;
				g_TreasureBox.vl_lock.lock();
				SendBoxPacket(i);
				g_TreasureBox.vl_lock.unlock();
			}
			if (GameStart)
			{
				Timer_Event event = { -1, high_resolution_clock::now() + 200ms, BOX_SYNC_TIME };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
			}
			delete over;
		}
		else if (WAS == over->event_type)
		{
			delete over;
			return;
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
				g_RespawnManager.UpdateRespawnManager(0.1f);

				Timer_Event event = { -1, high_resolution_clock::now() + 100ms, RESPAWN_TIME };
				tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
				delete over;
			}
			else if (SYNC_TIME == t.event) {
				over->event_type = SEND_SYNC;
				PostQueuedCompletionStatus(g_hiocp, 1, t.object_id, &over->over);
			}
			else if (BOX_SYNC_TIME == t.event)
			{
				over->event_type = BOX_SYNC;
				PostQueuedCompletionStatus(g_hiocp, 1, -1, &over->over);
			}
			else if (BOX_EVENT == t.event)
			{
				over->event_type = BOX_MOVE;
				PostQueuedCompletionStatus(g_hiocp, 1, -1, &over->over);
			}
			else if (CLOSE_SOCKET == t.event)
			{
				over->event_type = BOX_MOVE;
				PostQueuedCompletionStatus(g_hiocp, 0, t.object_id, &over->over);
			}
			else if (HOST_CLOSE == t.event)
			{	
				for (int i = 0; i < MAX_USER; ++i)
				{
					OverlappedEX *work_over = new OverlappedEX;
					work_over->event_type = WAS;
					
					PostQueuedCompletionStatus(g_hiocp, 1, -1, &work_over->over);
					
				}
				Close_Server = true;
				timer_queue.empty();
				ShutDown_Server();
				delete over;
				return;
			}
			else if (IN_GAME_TIME == t.event)
			{
				g_GameTime--;
				if (g_GameTime <= 0)
				{
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (!g_clients[i].connect)
							continue;

						SendDrawPacket(i);
					}
					GameStart = false;
				}
				else {
					for (int i = 0; i < MAX_USER; ++i)
					{
						if (!g_clients[i].connect)
							continue;
						SendGameTimePacket(i, g_GameTime);
					}
					Timer_Event event = { -1, high_resolution_clock::now() + 1s, IN_GAME_TIME };
					tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
				}
				delete over;
			}
			/*
			else if (GAME_TIME_M == t.event)
			{
				
				if (GameStart)
				{
					g_GameTime--;
					if (g_GameTime <= 0)
					{
						for (int i = 0; i < MAX_USER; ++i)
						{
							if (!g_clients[i].connect)
								continue;

							SendDrawPacket(i);
						}
						GameStart = false;
					}
					else {
						for (int i = 0; i < MAX_USER; ++i)
						{
							if (!g_clients[i].connect)
								continue;
							SendGameTimePacket(i, g_GameTime);
						}
						Timer_Event event = { -1, high_resolution_clock::now() + 100ms, GAME_TIME_M };
						tq_lock.lock();  timer_queue.push(event); tq_lock.unlock();
					}
				}
				delete over;
			}
			*/
			if (!GameStart)
			{
				
				for (int i = 0; i < MAX_USER; ++i)
				{
					if (i == 7 && ((g_clients[i].connect&&g_clients[i].ready) || !g_clients[i].connect))
					{
						for (int i = 0; i < MAX_USER; ++i)
						{
							if (!g_clients[i].connect)
								continue;
							SendStartPacket(i);
						}
						GameStart = true;
					}
					if (!g_clients[i].connect)
						continue;
					if (!g_clients[i].ready)
						break;

				}
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
			return;
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



		DWORD recv_flag = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), g_hiocp, new_id, 0);
		WSARecv(new_client, &g_clients[new_id].recv_over.wsabuf, 1, NULL, &recv_flag, &g_clients[new_id].recv_over.over, NULL);

		float fx = 0.2f, fy = 0.45f, fz = 0.2f;
		g_clients[new_id].vl_lock.lock();
		g_clients[new_id].player.SetPosition(D3DXVECTOR3(0, 10, 0));
		g_clients[new_id].MoveX = 0.f;
		g_clients[new_id].MoveZ = 0.f;
		g_clients[new_id].cameraYrotate = 0;
		g_clients[new_id].is_active = true;
		g_clients[new_id].ready = false;
		g_clients[new_id].last_move_time = high_resolution_clock::now();
		g_clients[new_id].player.SetBelongType(red < blue ? BELONG_TYPE_RED : BELONG_TYPE_BLUE);
		g_clients[new_id].player.m_BelongType == BELONG_TYPE_RED ? red++ : blue++;
		g_clients[new_id].vl_lock.unlock();
		SendTeamPacket(new_id, g_clients[new_id].player.m_BelongType);
		for (int i = 0; i < MAX_USER; ++i)
		{
			if (g_clients[i].connect == true)
				if (i != new_id) {
					SendPutPlayerPacket(new_id, i);
					SendPutPlayerPacket(i, new_id);
				}
		}

		//SendInitPacket(new_id);
		//SendPutPlayerPacket(new_id, new_id);



		std::cout << "new client accept: " << new_id << std::endl;
	}
}

void ShutDown_Server()
{
	closesocket(g_ssocket);
	CloseHandle(g_hiocp);
	g_hiocp = INVALID_HANDLE_VALUE;
	WSACleanup();
}

/*
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
}*/