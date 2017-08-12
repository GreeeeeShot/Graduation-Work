#pragma once

#include "resource.h"

enum State { host_select, server_input, play, waitingroom };

extern State state;

extern int my_team;
extern int my_id;
extern int user_num;
extern int charac;