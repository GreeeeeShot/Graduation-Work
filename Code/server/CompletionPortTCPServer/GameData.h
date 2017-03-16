#pragma once
#include <iostream>
#include <stdlib.h>
#include <string>

using namespace std;

class CGameData
{
private:
	char name[20];
public:
	CGameData(char* cname) { strcpy(name, cname); }
	CGameData() { strcpy(name, ""); }
	~CGameData();
	void getName() { cout << name << endl; }
};


