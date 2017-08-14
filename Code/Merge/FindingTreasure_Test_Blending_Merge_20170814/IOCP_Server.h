#pragma once

#include <WinSock2.h>
#include <Winsock.h>
#include <Windows.h>

void Initialize_Server();
void SendPacket(int cl, void *packet);
void SendPutPlayerPacket(int client, int object);
void SendRemovePlayerPacket(int client, int object);
void SendPositionPacket(int client, int object);
void DisconnectClient(int ci);
void ProcessPacket(int ci, unsigned char packet[]);
void Worker_Thread();
void Accept_Thread();
void ShutDown_Server();
void ServerMain();
void Time_Thread();
void Process_Thread();