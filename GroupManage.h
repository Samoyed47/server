#ifndef GROUPMANAGE_H
#define GROUPMANAGE_H
#include<iostream> 
#pragma once
#pragma comment(lib, "sqlite3.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include "sqlite3.h"
#include "databases.h"
#include <cstring>
using namespace std;   

class GroupManage {
public:
	string  CreatGroup(string buf);
	string  Invite(string buf);
	string  Search(string buf);
	string  Dissolve(string buf);
	string ShowMember(string buf);
private:
	DataBases D;
};
#endif   // MY_GROUP_MANGE
