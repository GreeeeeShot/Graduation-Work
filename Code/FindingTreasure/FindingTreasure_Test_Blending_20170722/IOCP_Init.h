#pragma once

#include <iostream>
#include <thread>
#include <vector>

struct Client_Data
{
	enum Player_State {
		NORMAL, DISCONNECT
	};

	float x;
	float z;
	bool team;
	Player_State state;
	int vox;

};

#define MAX_BUFF_SIZE   4000
#define MAX_PACKET_SIZE  255

#define BOARD_WIDTH   8
#define BOARD_HEIGHT  8

#define VIEW_RADIUS   3

#define MAX_USER 8

#define NPC_START  1000
#define NUM_OF_NPC  10000

#define MY_SERVER_PORT  4000

#define MAX_STR_SIZE  100

#define CS_POSMOVE 1
#define CS_CAMERAMOVE 2
#define CS_JUMP 3
/*
#define CS_JUMP  1
#define CS_UP    2
#define CS_DOWN  3
#define CS_LEFT  4
#define CS_RIGHT 5
#define CS_XSTOP 6
#define CS_ZSTOP 7
#define CS_CHAT	 8
*/

#define SC_POS           1
#define SC_PUT_PLAYER    2
#define SC_REMOVE_PLAYER 3
#define SC_CHAT			 4
#define SC_SETPOS		 5
#define SC_INIT			 6

#pragma pack (push, 1)

struct cs_packet_up {
	BYTE size;
	BYTE type;
	CHAR Movex;
	CHAR Movez;
};

struct cs_packet_camera {
	BYTE size;
	BYTE type;
	CHAR Look;
};

struct cs_packet_chat {
	BYTE size;
	BYTE type;
	WCHAR message[MAX_STR_SIZE];
};

struct sc_packet_insertvox {

};

struct sc_packet_deletevox {

};

struct sc_packet_pos {
	BYTE size;
	BYTE type;
	WORD id;
	CHAR MoveX;
	CHAR MoveZ;
	CHAR CameraY;
};

struct sc_packet_put_player {
	BYTE size;
	BYTE type;
	WORD id;
	FLOAT x;
	FLOAT y;
	FLOAT z;
};
struct sc_packet_remove_player {
	BYTE size;
	BYTE type;
	WORD id;
};

struct sc_packet_chat {
	BYTE size;
	BYTE type;
	WORD id;
	WCHAR message[MAX_STR_SIZE];
};

struct sc_packet_init {
	BYTE size;
	BYTE type;
	WORD id;
	BYTE team;
	CHAR x, y, z;
};

#pragma pack (pop)