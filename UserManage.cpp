#include "UserManage.h"

string UserManage::Register(string buf) //buf:nickname|password; return:5#账号(string)
{
	char* Nickname = new char[buf.length() + 1];
	char* Password = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", Nickname, Password);
	string NickName = Nickname;
	string PassWord = Password;
	cout << Nickname << endl;
	cout << Password << endl;

	D.SelectData(1, "/0");	
	size_t n = D.User1.size() - 1;	
	int MaxAccount = D.User1[n].Account;	
	cout << MaxAccount << endl;

	string data = to_string(MaxAccount+1) + ",'" + NickName + "','" + PassWord + "'," + "NULL" + "," + to_string(0) + "," + "NULL" + "," + "'/0'";
	bool b = D.AddData(1, data);	//表里没有显示增加一行
	if (b)	cout << "true" << endl;
	if (!b)	cout << "false" << endl;

	return string("5#" + to_string(MaxAccount+1));
}

string UserManage::Logging(string buf)//buf:账号|密码; return:9#账号#昵称#头像#好友账号1|好友账号2|#群号1|群号2| / Failed / Not Exist
{
	char* acc = new char[buf.length() + 1];
	char* pword = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", acc, pword);
	string Acc = acc;
	string PWord = pword;

	string SQL = "Account = " + Acc;
	D.SelectData(1, SQL);

	string ans;
	if (!D.User1.empty())
	{
		string Password = D.User1[0].Password;
		if (Password == PWord)
		{
			//ans = "9" + '#' + string("Success");
			ans = LogSuccess(buf);
		}
		else
		{
			ans = "9#Failed";
		}
	}
	else
	{
		ans = "9#Not Exist";
	}
	return ans;
}


string UserManage::LogSuccess(string buf)//buf:账号|密码; return:9#账号#昵称#头像#好友账号1|好友账号2|#群号1|群号2|
{
	char* acc = new char[buf.length() + 1];
	char* pword = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", acc, pword);
	string Acc = acc;
	string PWord = pword;
	cout << acc << endl;
	cout << pword << endl;

	int Account, Portrait;
	string UName;
	D.SelectData(1, "Account = " + Acc);
	Account = D.User1[0].Account;
	Portrait = D.User1[0].Potrait;
	UName = D.User1[0].UName;

	string Data1 = to_string(Account) + "#" + UName + "#" + to_string(Portrait);

	D.SelectData(1, "Account = " + Acc);
	string OffLineTime = D.User1[0].OffLineTime;
	int Socket = D.User1[0].Socket;

	//未发送消息
	string SQL = "CMember LIKE '%" + Acc + "%'";	
	D.SelectData(2, SQL); 
	if (D.SelectData(2, SQL))	cout << "true" << endl;
	if (!D.SelectData(2, SQL))	cout << "false" << endl;
	vector<string> MsgRecord;
	MsgRecord.clear();
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		MsgRecord.push_back(D.Cluster1[i].MsgRecord);
	}
	for (size_t i = 0; i < MsgRecord.size(); i++)//MsgRecord[i]:Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3
	{
		if (i >= MsgRecord.size() || i < 0) break;
		char* MR = new char[MsgRecord[i].length() + 1];
		strcpy(MR, MsgRecord[i].c_str());
		cout << MR << endl;
		char* AMsg;
		AMsg = strtok(MR, "|");//字符串拆分strtok()
		while (AMsg != NULL)	//AMsg:Time1,Acount1,Msg1
		{
			cout << AMsg << endl;
			string AM = AMsg;
			char* time = new char[AM.length() + 1];
			char* acc = new char[AM.length() + 1];
			char* msg = new char[AM.length() + 1];
			sscanf(AMsg, "%[^,],%[^,],%s", time, acc, msg);
			string Time = time;
			string Acc = acc;
			string Msg = msg;
			if (Time > OffLineTime)	//如果消息时间晚于上次下线时间   （字符串比较是逐位比较）
			{
				string SMsg = AMsg;
				SendMsg("10#" + SMsg, Socket);
			}
			AMsg = strtok(NULL, "|");
		}
	}
	//好友
	string SQL1 = "CMember LIKE '%" + Acc + "%' AND COwner=1";//搜索含Acc的两人群组
	D.SelectData(2, SQL1);
	if (D.SelectData(2, SQL1))	cout << "true" << endl;
	if (!D.SelectData(2, SQL1))	cout << "false" << endl;

	string AllFriend = "";
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		char* a = new char[D.Cluster1[i].CMember.length() + 1];
		char* f = new char[D.Cluster1[i].CMember.length() + 1];
		char* Buffer = new char[D.Cluster1[i].CMember.length() + 1];
		strcpy(Buffer, D.Cluster1[i].CMember.c_str());
		sscanf(Buffer, "%[^|]|%s", a, f);
		//cout << Buffer << endl;
		string A = a;
		string F = f;
		if (A == Acc)
		{
			AllFriend += F + "|";
		}
		else
		{
			AllFriend += A + "|";
		}
	}
	//群聊
	string SQL2 = "CMember LIKE '%" + Acc + "%' AND COwner<>1";
	D.SelectData(2, SQL2);
	//cout << D.Cluster1[0].CMember << endl;

	string AllGroup = "";
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		AllGroup += to_string(D.Cluster1[i].CNum) + "|";
	}

	return "9#" + Data1 + "#" + AllFriend + "#" + AllGroup;
}


string UserManage::SendMsg(string buf, int confd)//单发消息，buf:发送时间，发信人账号，消息内容；confd:收信人socket;;;return: 11#Done(收信人已收到)
{
	//write(confd, buf, sizeof(buf));
	cout << buf << " to " << confd << endl;
	return "11#Done";
}

string UserManage::Receive(string buf)//buf:群号或好友账号|用户账号|发送时间|消息内容; return:10#Done(表示收到消息且已发出）
{
	char* num = new char[buf.length() + 1];
	char* acc = new char[buf.length() + 1];
	char* time = new char[buf.length() + 1];
	char* msg = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%[^|]|%s", num, acc, time, msg);
	string Num = num, Acc = acc, Time = time, Msg = msg;
	cout << num << endl;
	cout << acc << endl;
	cout << time << endl;
	cout << msg << endl;

	string CNum;
	if (Num < "40000000")//buf是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
		string SQL = "CMember = '" + Member1 + "'or CMember = '" + Member2 + "'";
		D.SelectData(2, SQL);
		if (D.SelectData(2, SQL))	cout << "true" << endl;
		if (!D.SelectData(2, SQL))	cout << "false" << endl;

		int Num = D.Cluster1[0].CNum;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}
	cout << CNum << endl;
	string SMsg = "|" + Time + "," + Acc + "," + Msg + "'";
	cout << SMsg.c_str() << endl;

	D.SelectData(2, "CNum = " + CNum);
	if (D.SelectData(2, "CNum = " + CNum))	cout << "true" << endl;
	if (!D.SelectData(2, "CNum = " + CNum))	cout << "false" << endl;

	string MsgRecord = D.Cluster1[0].MsgRecord;//Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3
	cout << MsgRecord.c_str() << endl;
	string RMsg = "'" + MsgRecord + SMsg;
	cout << RMsg.c_str() << endl;

	D.ChangeData(2, "MsgRecord = " + RMsg, "CNum = " + CNum);
	if (D.ChangeData(2, "MsgRecord = " + RMsg, "CNum = " + CNum))	cout << "true" << endl;
	if (!D.ChangeData(2, "MsgRecord = " + RMsg, "CNum = " + CNum))	cout << "false" << endl;


	D.SelectData(2, "CNum = " + CNum);
	if (D.SelectData(2, "CNum = " + CNum))	cout << "true" << endl;
	if (!D.SelectData(2, "CNum = " + CNum))	cout << "false" << endl;

	string CMember = D.Cluster1[0].CMember;
	cout << CMember.c_str() << endl;	//FALSE
	char* CM = new char[CMember.length() + 1];
	strcpy(CM, CMember.c_str());
	char* User;
	User = strtok(CM, "|");//字符串拆分strtok()
	while (User != NULL)
	{
		string A = User;
		cout << User << endl;
		D.SelectData(1, "Account = " + A);
		int LogStatus = D.User1[0].LogStatus;
		if (LogStatus == 1)
		{
			int Socket = D.User1[0].Socket;
			SendMsg("10#" + SMsg, Socket);
		}
		User = strtok(NULL, "|");
	}

	return "10#Done";
}

string UserManage::SearchUser(string buf)//buf:用户账号; return : 6#账号#昵称#头像#登录状态#上次下线时间 / 该用户不存在
{
	string data;
	int Account, Portrait, LogStatus;
	string UName, OffLineTime;

	D.SelectData(1, "Account = " + buf);
	if (!D.User1.empty())
	{
		Account = D.User1[0].Account;
		Portrait = D.User1[0].Potrait;
		LogStatus = D.User1[0].LogStatus;
		UName = D.User1[0].UName;
		OffLineTime = D.User1[0].OffLineTime;
		data = "6#" + to_string(Account) + "#" + UName + "#" + to_string(Portrait) + "#" + to_string(LogStatus) + "#" + OffLineTime;
		return data;
	}
	else
	{
		D.SelectData(1, "UName = '" + buf + "'");
		if (!D.User1.empty())
		{
			Account = D.User1[0].Account;
			Portrait = D.User1[0].Potrait;
			LogStatus = D.User1[0].LogStatus;
			UName = D.User1[0].UName;
			OffLineTime = D.User1[0].OffLineTime;
			data = "6#" + to_string(Account) + "#" + UName + "#" + to_string(Portrait) + "#" + to_string(LogStatus) + "#" + OffLineTime;
			return data;
		}
		else
		{
			return "6#The User Does Not Exist!";
		}
	}
}
