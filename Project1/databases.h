#ifndef DATABASES_H
#define DATABASES_H
#include <iostream>
#include <string>
#include<vector>
#include <sqlite3.h>

using namespace std;
typedef struct User {
	int Account;
	string UName;
	string Password;
	int Potrait;
	int LogStatus;
	int Socket;
	string OffLineTime;
};
typedef struct Cluster {
	int CNum;
	string CName;
	int COwner;
	string CMember;
	string MsgRecord;
};
class DataBases {
	sqlite3* handler;//���ݿ���
	sqlite3_stmt* pStmt;//�����õ�sql���

public:
	vector<User> User1;
	vector<Cluster> Cluster1;
	DataBases() {
		sqlite3* h;
		int val = sqlite3_open("server.db", &h);
		if (val != SQLITE_OK)
		{
			//���ݿ�����ʧ��
			cout << "connect database failed��" << endl;
		}
		handler = h;
	}
	bool AddData(int tablenum, string data, int size = 0, const void* blobdata = NULL);
	bool AddImage(int num, int size = 0, const void* blobdata = NULL);
	bool DeleteImage(int num);
	bool ChangeData(int tablenum, string data, string where = "/0", int size = 0, const void* blobdata = NULL);
	bool DeleteData(int tablenum, string where = "/0");
	bool SelectData(int tablenum, string where = "/0");
};

#endif