#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;
class CGameData
{
private:
	char name[20];
public:
	CGameData(char* cname) { strcpy(name, cname); }
	CGameData() { strcpy(name,""); }
	~CGameData();
	void getName() { cout << name << endl; }
};

