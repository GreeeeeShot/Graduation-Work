#include "stdafx.h"
#include "IOCP_Init.h"
#include "IOCP_Server.h"


HANDLE g_hiocp;
SOCKET g_ssocket;


enum OPTYPE { OP_SEND, OP_RECV };

struct OverlappedEX {
	WSAOVERLAPPED over;
	WSABUF wsabuf;
	unsigned char IOCP_buf[MAX_BUFF_SIZE];
	OPTYPE event_type;
};

struct CLIENT {
	float x;
	float y;
	float z;
	bool team;
	bool connect;

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
	}
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

void SendPutPlayerPacket(int client, int object)
{
	sc_packet_pos packet;
	packet.id = object;
	packet.size = sizeof(packet);
	packet.type = SC_PUT_PLAYER;
	packet.x = g_clients[object].x;
	packet.z = g_clients[object].z;

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
	packet.x = g_clients[object].x;
	packet.z = g_clients[object].z;

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
		if (g_clients[ci].z > 0)
			g_clients[ci].z-=2.0f;
		break;
	case CS_DOWN:
		if (g_clients[ci].z < BOARD_HEIGHT - 1)
			g_clients[ci].z+= 2.0f;
		break;
	case CS_LEFT:
		if (g_clients[ci].x > 0)
			g_clients[ci].x-= 2.0f;
		break;
	case CS_RIGHT:
		if (g_clients[ci].x < BOARD_WIDTH - 1)
			g_clients[ci].x+= 2.0f;
		break;
	default:
		std::cout << "Unknown Packet Type from Client" << ci << std::endl;
		while (true);
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
		g_clients[new_id].x = 4;
		g_clients[new_id].z = 4;
		g_clients[new_id].team = new_id % 2;

		DWORD recv_flag = 0;

		CreateIoCompletionPort(reinterpret_cast<HANDLE>(new_client), g_hiocp, new_id, 0);
		WSARecv(new_client, &g_clients[new_id].recv_over.wsabuf, 1, NULL, &recv_flag, &g_clients[new_id].recv_over.over, NULL);
		SendPutPlayerPacket(new_id, new_id);

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

void ServerMain()
{
	std::vector<std::thread *> worker_threads;
	Initialize_Server();
	for (int i = 0; i < 6; ++i) worker_threads.push_back(new std::thread{ Worker_Thread });
	std::thread Accept_Thread(Accept_Thread);
	Accept_Thread.join();
	//Create_Accept_Thread();
	for (auto pth : worker_threads) {
		pth->join();
		delete pth;
	}
	ShutDown_Server();
}