#include "stdafx.h"
#include "IOCP_Init.h"
#include "IOCP_Client.h"
#include "GameManager.h"
#include "GameFramework.h"

//CPlayer player;
#pragma comment (lib, "ws2_32.lib")


SOCKET g_mysocket;
WSABUF	send_wsabuf;
char 	send_buffer[BUF_SIZE];
WSABUF	recv_wsabuf;
char	recv_buffer[BUF_SIZE];
char	packet_buffer[BUF_SIZE];
DWORD		in_packet_size = 0;
int		saved_packet_size = 0;
int		g_myid;
int		g_left_x = 0;
int     g_top_y = 0;

Client_Data player;

void clienterror()
{
	exit(-1);
}

void ProcessPacket(char *ptr)
{
	static bool first_time = true;
	float x = 0, y = 0, z = 0;
	int id=-1;
	float px=0.0, py = 0.0, pz = 0.0;
	switch (ptr[1])
	{
	case SC_PUT_PLAYER:
	{
		sc_packet_put_player *my_packet = reinterpret_cast<sc_packet_put_player *>(ptr);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[my_packet->id]->SetPosition(D3DXVECTOR3(my_packet->x, my_packet->y, my_packet->z));
		break;
	}
	case SC_POS:
	{
		sc_packet_pos *my_packet = reinterpret_cast<sc_packet_pos *>(ptr);
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[my_packet->id]->SetPosition(D3DXVECTOR3(my_packet->x, my_packet->y, my_packet->z));
		x = my_packet->x;
		z = my_packet->z;
		id = my_packet->id;
		y = my_packet->y;
		//y = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->GetPosition().y;
		//std::cout<<"Position: " << x<<", " << y << ", " << z << std::endl;
		//CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetMove(x, y, z);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[id]->SetPosition(D3DXVECTOR3(x,y, z));
		break;
	}

	case SC_REMOVE_PLAYER:
	{
		sc_packet_remove_player *my_packet = reinterpret_cast<sc_packet_remove_player *>(ptr);
		break;
	}

	case SC_INIT:
	{
		sc_packet_init *my_packet = reinterpret_cast<sc_packet_init *>(ptr);
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iMyPlayerID = my_packet->id;
		CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_ppPlayers[my_packet->id]->SetPosition(D3DXVECTOR3(my_packet->x, my_packet->y, my_packet->z));
	}
	/*
	case SC_CHAT:
	{
		sc_packet_chat *my_packet = reinterpret_cast<sc_packet_chat *>(ptr);
		int other_id = my_packet->id;
		if (other_id == g_myid) {
			wcsncpy_s(player.message, my_packet->message, 256);
			player.message_time = GetTickCount();
		}
		else if (other_id < NPC_START) {
			wcsncpy_s(skelaton[other_id].message, my_packet->message, 256);
			skelaton[other_id].message_time = GetTickCount();
		}
		else {
			wcsncpy_s(npc[other_id - NPC_START].message, my_packet->message, 256);
			npc[other_id - NPC_START].message_time = GetTickCount();
		}
		break;

	}*/
	default:
		printf("Unknown PACKET type [%d]\n", ptr[1]);
	}
}

void ReadPacket(SOCKET sock)
{
	DWORD io_byte, io_flag = 0;

	int ret = WSARecv(sock, &recv_wsabuf, 1, &io_byte, &io_flag, NULL, NULL);
	if (ret) {
		int err_code = WSAGetLastError();
		printf("Recv Error [%d]\n", err_code);
	}

	BYTE *ptr = reinterpret_cast<BYTE *>(recv_buffer);

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(packet_buffer);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void ClientMain(HWND main_window_handle,const char* serverip)
{
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	g_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(serverip);

	int Result = WSAConnect(g_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(g_mysocket, main_window_handle, WM_SOCKET, FD_CLOSE | FD_READ);

	send_wsabuf.buf = send_buffer;
	send_wsabuf.len = BUF_SIZE;
	recv_wsabuf.buf = recv_buffer;
	recv_wsabuf.len = BUF_SIZE;
}

void SetPacket(int x,int y, int z)
{
	cs_packet_up *my_packet = reinterpret_cast<cs_packet_up *>(send_buffer);
	int id = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->m_iMyPlayerID;
	my_packet->size = sizeof(my_packet);
	send_wsabuf.len = sizeof(my_packet);
	DWORD iobyte;
	my_packet->Lookx = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.GetLook().x;
	my_packet->Lookz = CGameManager::GetInstance()->m_pGameFramework->m_pPlayersMgrInform->GetMyPlayer()->m_CameraOperator.GetLook().z;
	//std::cout<<"Client Look" << (float)my_packet->Lookx << ", " <<(float)my_packet->Lookz << std::endl;
	if (x>0)
	{
		my_packet->type = CS_RIGHT;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	if (x<0)
	{
		my_packet->type = CS_LEFT;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	if (z>0)
	{
		my_packet->type = CS_UP;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	if (z<0)
	{
		my_packet->type = CS_DOWN;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	if (y>0) {
		my_packet->type = CS_JUMP;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	/*
	if (move == X_STOP)
	{
		my_packet->type = CS_XSTOP;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}
	if (move == Z_STOP)
	{
		my_packet->type = CS_ZSTOP;
		int ret = WSASend(g_mysocket, &send_wsabuf, 1, &iobyte, 0, NULL, NULL);
		if (ret) {
			int error_code = WSAGetLastError();
			printf("Error while sending packet [%d]", error_code);
		}
	}*/
}