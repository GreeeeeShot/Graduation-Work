#pragma once

#include <WinSock2.h>
#include <Winsock.h>
#include <Windows.h>

#define	BUF_SIZE				1024
#define	WM_SOCKET				WM_USER + 1

enum PACKETTYPE { POSMOVE,CAMERAMOVE,JUMP };

void clienterror();
void ProcessPacket(char *ptr);
void ReadPacket(SOCKET sock);
void ClientMain(HWND main_window_handle, const char* serverip);
void SetPacket(PACKETTYPE type, int Posx, int Posz, int Look);