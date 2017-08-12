#pragma once

#include <WinSock2.h>
#include <Winsock.h>
#include <Windows.h>

#define	BUF_SIZE				1024
#define	WM_SOCKET				WM_USER + 1

enum PACKETTYPE { POSMOVE,CAMERAMOVE,JUMP , VOXELDEL, VOXELINS , VOXELCANCLE, TEAMCHANGE, EXIT, CHARACHANGE, READY};

void clienterror();
void ProcessPacket(char *ptr);
void ReadPacket(SOCKET sock);
void ClientMain(HWND main_window_handle, const char* serverip);
void SetPacket(PACKETTYPE type, int Posx, int Posz, int Look);
void WaitingPacket(PACKETTYPE type);

enum CHARACTER { pirate, cowgirl };

struct WaitingPlayer {
	bool connect;
	int id;
	BELONG_TYPE team;
	CHARACTER charac;
	bool ready;
};

extern WaitingPlayer waitingplayer[7];