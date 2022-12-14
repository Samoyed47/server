#pragma once
#pragma comment(lib, "sqlite3.lib")

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string.h>
#include <vector>
#include "sqlite3.h"
#include "database.h"

using namespace std;


class UserManage {
public:
	string  Register(string buf);
	string  SearchUser(string buf);
	string  AddFriend1(string buf);
	string  AddFriend2(string buf);
	string  ChangeData(string buf);
	string  Logging(string buf);
	string LogSuccess(string buf);
	string  Receive(string buf);
	string  SendMsg(string buf, int confd);
	string  SelecteAccount(string buf);
	string  SelecteGroup(string buf);
protected:

private:
	DataBases D;
};
