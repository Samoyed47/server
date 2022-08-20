#pragma once

#include <iostream>
#include <string>
#include <sqlite3.h>
using namespace std;

class UserInformation {
public: 	
	int account ; 
	
} ; 

class UserManage {
public:
	string  Register(string buf);
	string  SearchUser(string buf);
	string  AddFriend1(string buf);
	string  AddFriend2(string buf);
	string  ChangeData(string buf);
	string  Logging(string buf);
	string	LogSuccess(string buf);
	string  Receive(string buf);
	string  SendMsg(string buf, int confd);
	string  SelecteAccount(string buf);
	string  SelecteGroup(string buf);
protected:

private:
	sqlite3* handler;
	char* errormsg;
	int val = sqlite3_open("sqlite.db", &handler);
	static const int MaxAccount = 10000000;
};
