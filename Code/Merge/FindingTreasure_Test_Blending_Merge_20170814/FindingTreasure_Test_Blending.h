#pragma once

#include "resource.h"

enum State { host_select, server_input, play, waitingroom };

extern State state;
extern bool IsWaitingRoom;
extern int my_team;
extern int my_id;
extern int user_num;
extern int charac;
extern bool host;
extern int strn;
extern bool ready;
extern char	server_ip[20];
extern char server_msg[20];