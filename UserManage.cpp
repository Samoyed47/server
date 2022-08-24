#include "UserManage.h"

//return: 编号#。。。。。。
string UserManage::Register(string buf) //buf:nickname|password; return:5#账号(string)
{
	char* Nickname = new char[buf.length() + 1];
	char* Password = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", Nickname, Password);
	string NickName = Nickname;
	string PassWord = Password;

	D.SelectData(1, "/0");	
	size_t n = D.User1.size() - 1;	
	int MaxAccount = D.User1[n].Account;

	string data = to_string(MaxAccount+1) + ",'" + NickName + "','" + PassWord + "'," + "NULL" + "," + to_string(0) + "," + "NULL" + "," + "'/0'";
	D.AddData(1, data);	

	delete[] Nickname;
	delete[] Password;
	delete[] Buffer;
	return string("5#" + to_string(MaxAccount + 1));
}

string UserManage::Logging(string buf)//buf:账号|密码|Socket; return:9#LogSuccess的return / 9#Failed
{
	char* acc = new char[buf.length() + 1];
	char* pword = new char[buf.length() + 1];
	char* socket = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%s", acc, pword, socket);
	string Acc = acc;
	string PWord = pword;
	string SK = socket;

	string SQL = "Account = " + Acc;
	D.SelectData(1, SQL);

	string ans;
	if (!D.User1.empty())
	{
		string Password = D.User1[0].Password;
		if (Password == PWord)
		{
			ans = "9#";
			ans += LogSuccess(buf);
		}
		else
		{
			ans = "9#Failed";
		}
	}
	else
	{
		ans = "9#Failed";
	}
	delete[] acc;
	delete[] pword;
	delete[] Buffer;
	delete[] socket;
	return ans;
}


string UserManage::LogSuccess(string buf)//buf:账号|密码|Socket; return:账号#昵称#好友账号1|好友账号2|#好友昵称1|好友昵称2|#群号1|群号2|#群名1|群名2|#未发送消息1|未发送消息2|；未发送消息：时间,发出消息的用户的账号,发出消息的用户的昵称,群号或好友账号,对话消息
{
	char* acc = new char[buf.length() + 1];
	char* pword = new char[buf.length() + 1];
	char* socket = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%s", acc, pword, socket);
	string Acc = acc;
	string PWord = pword;
	string SK = socket;
	delete[] acc;
	delete[] pword;
	delete[] Buffer;
	delete[] socket;
	//基本信息
	int Account, Portrait;
	string UName;
	D.SelectData(1, "Account = " + Acc);
	Account = D.User1[0].Account;
	Portrait = D.User1[0].Potrait;
	UName = D.User1[0].UName;

	string Data1 = to_string(Account) + "#" + UName;

	//改变登录状态，记录socket
	D.ChangeData(1, "LogStatus=1", "Account=" + Acc);
	D.ChangeData(1, "Socket=" + SK, "Account=" + Acc);

	D.SelectData(1, "Account = " + Acc);
	string OffLineTime = D.User1[0].OffLineTime;
	int Socket = D.User1[0].Socket;

	//未发送消息
	string UnSendMsg = "";
	string SQL = "CMember LIKE '%" + Acc + "%'";
	D.SelectData(2, SQL);
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		string RNum;
		string MsgRecord = D.Cluster1[i].MsgRecord;
		int COwner = D.Cluster1[i].COwner;
		if (COwner == 1)//好友关系
		{
			string CMember = D.Cluster1[i].CMember;
			char* N = new char[CMember.length() + 1];
			char* F = new char[CMember.length() + 1];
			char* B = new char[CMember.length() + 1];
			strcpy(B, CMember.c_str());
			sscanf(B, "%[^|]|%s", N, F);
			if (N == Acc)//拆分出来的N是当前用户账号，即F是好友账号
			{
				RNum = F;
			}
			else
			{
				RNum = N;
			}
			delete[] N;
			delete[] F;
			delete[] B;
		}
		else//群聊关系
		{
			RNum = to_string(D.Cluster1[i].CNum);
		}
		
		char* MR = new char[MsgRecord.length() + 1];
		strcpy(MR, MsgRecord.c_str());
		char* AMsg;
		AMsg = strtok(MR, "|");//字符串拆分strtok()
		while (AMsg != NULL)	//AMsg:Time1,Acount1,Msg1
		{
			string AM = AMsg;
			char* time = new char[AM.length() + 1];
			char* acc = new char[AM.length() + 1];
			char* msg = new char[AM.length() + 1];
			sscanf(AMsg, "%[^,],%[^,],%s", time, acc, msg);
			string Time = time;
			string Acc = acc;
			string Msg = msg;
			delete[] time;
			delete[] acc;
			delete[] msg;

			D.SelectData(1, "Account = " + Acc);
			string NickName = D.User1[0].UName;
			string SMsg2 = Time + "," + Acc + "," + NickName + "," + RNum + "," + Msg;
			if (Time > OffLineTime)	//如果消息时间晚于上次下线时间   （字符串比较是逐位比较）
			{
				UnSendMsg += SMsg2 + "|";
			}
			AMsg = strtok(NULL, "|");
		}
		delete[] MR;
	}
	//好友
	string SQL1 = "CMember LIKE '%" + Acc + "%' AND COwner=1";//搜索含Acc的两人群组
	D.SelectData(2, SQL1);
	string AllFriend = "";
	string AllFriendNick = "";
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		char* a = new char[D.Cluster1[i].CMember.length() + 1];
		char* f = new char[D.Cluster1[i].CMember.length() + 1];
		char* Buffer = new char[D.Cluster1[i].CMember.length() + 1];
		strcpy(Buffer, D.Cluster1[i].CMember.c_str());
		sscanf(Buffer, "%[^|]|%s", a, f);
		string A = a;
		string F = f;
		delete[] a;
		delete[] f;
		delete[] Buffer;
		if (A == Acc)
		{
			AllFriend += F + "|";
			D.SelectData(1, "Account=" + F);
			AllFriendNick += D.User1[0].UName + "|";
		}
		else
		{
			AllFriend += A + "|";
			D.SelectData(1, "Account=" + A);
			AllFriendNick += D.User1[0].UName + "|";
		}
	}
	//群聊
	string SQL2 = "CMember LIKE '%" + Acc + "%' AND COwner<>1";
	D.SelectData(2, SQL2);
	string AllGroup = "";
	string AllGroupName = "";
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		AllGroup += to_string(D.Cluster1[i].CNum) + "|";
		AllGroupName += D.Cluster1[i].CName + "|";
	}

	return Data1 + "#" + AllFriend + "#" + AllFriendNick + "#" + AllGroup + "#" + AllGroupName + "#" + UnSendMsg;
}

string UserManage::LogOut(string buf)//buf:账号; return:(无）
{
	string Acc = buf;
	//改变登录状态
	D.ChangeData(1, "LogStatus=0", "Account=" + Acc);
	//记录下线时间
	time_t timep;
	time(&timep);
	char tmp[64];
	strftime(tmp, sizeof(tmp), "%Y.%m.%d.%H.%M.%S", localtime(&timep));
	string time = tmp;//获取当前时间
	D.ChangeData(1, "OffLineTime='" + time + "'", "Account=" + Acc);
	return string();
}


string UserManage::Receive(string buf, vector<int>& Members)//buf:用户账号|群号或好友账号|发送时间|消息内容; return:发出者昵称#时间#信息内容
{
	char* num = new char[buf.length() + 1];
	char* acc = new char[buf.length() + 1];
	char* time = new char[buf.length() + 1];
	char* msg = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%[^|]|%s", acc, num, time, msg);
	string Num = num, Acc = acc, Time = time, Msg = msg;
	//cout << Num << Acc << Time << Msg << endl;
	//确定群组编号
	string CNum;
	if (Num < "40000000")//Num是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
		string SQL = "CMember = '" + Member1 + "'or CMember = '" + Member2 + "'";
		D.SelectData(2, SQL);

		int Num = D.Cluster1[0].CNum;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}
	//cout << CNum << endl;

	string SMsg = Time + "," + Acc + "," + Msg;//存入数据库的格式
	//cout << SMsg << endl;

	D.SelectData(1, "Account=" + Acc);
	string NickName = D.User1[0].UName;
	string SMsg2 = NickName + "#" + Time + "#" + Msg;//返回给客户端的格式
	//cout << SMsg2 << endl;

	//存入表中聊天记录
	D.SelectData(2, "CNum = " + CNum);
	string MsgRecord = D.Cluster1[0].MsgRecord;//Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3
	string RMsg;
	if (MsgRecord == "/0")
	{
		RMsg = "'" + SMsg + "'";
	}
	else
	{
		RMsg = "'" + MsgRecord + "|" + SMsg + "'";
	}
	//cout << RMsg << endl;
	D.ChangeData(2, "MsgRecord = " + RMsg, "CNum = " + CNum);
	//填充vector
	D.SelectData(2, "CNum = " + CNum);
	string CMember = D.Cluster1[0].CMember;
	char* CM = new char[CMember.length() + 1];
	strcpy(CM, CMember.c_str());
	char* User;
	User = strtok(CM, "|");//字符串拆分strtok()
	while (User != NULL)
	{
		string U = User;
		if (U != Acc)
		{
			Members.push_back(atoi(User));
		}
		
		User = strtok(NULL, "|");
	}

	delete[] num;
	delete[] acc;
	delete[] time;
	delete[] msg;
	delete[] Buffer;
	delete[] CM;
	return SMsg2;
}
string UserManage::SearchUser(string buf)//buf:该用户账号|被搜索的账号; return : 6#账号#昵称#登录状态#上次下线时间/ 该用户不存在
{
	char* now = new char[buf.length() + 1];
	char* search = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", now, search);
	string Now = now, Search = search;
	delete[] now;
	delete[] search;
	delete[] Buffer;
	D.SelectData(1, "Account = " + Now);
	int Socket = D.User1[0].Socket;

	string data = "";
	string tmpdata;
	int Account, Portrait, LogStatus;
	string UName, OffLineTime;

	if (D.SelectData(1, "Account = " + Search) && !D.User1.empty())
	{
		Account = D.User1[0].Account;
		Portrait = D.User1[0].Potrait;
		LogStatus = D.User1[0].LogStatus;
		UName = D.User1[0].UName;
		OffLineTime = D.User1[0].OffLineTime;
		data = "6#" + to_string(Account) + "#" + UName + "#" + to_string(LogStatus) + "#" + OffLineTime;
		return data;
	}
	else
	{
			return "6#The User Does Not Exist!";
	}
}

string UserManage::AddFriend1(string buf)//buf:用户账号|好友账号；return : 7#Y#好友账号#好友昵称/N(已经是好友）
{
	char* acc1 = new char[buf.length() + 1];
	char* acc2 = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", acc1, acc2);
	string Acc1 = acc1, Acc2 = acc2;
	delete[] acc1;
	delete[] acc2;
	delete[] Buffer;

	//判断两人是否已经是好友
	string Member1 = Acc1 + "|" + Acc2;
	string Member2 = Acc2 + "|" + Acc1;
	string SQL = "CMember = '" + Member1 + "'or CMember = '" + Member2 + "'";
	D.SelectData(2, SQL);
	if (!D.Cluster1.empty())
	{
		return "7#N";
	}

	string data;
	string Member = Acc1 + "|" + Acc2;

	D.SelectData(2, "/0");
	size_t n = D.Cluster1.size() - 1;
	int MaxCNum = D.Cluster1[n].CNum;

	data = to_string(MaxCNum + 1) + ",'/0', " + to_string(1) + ", '" + Member + "', " + "'/0'";
	D.AddData(2, data);

	D.SelectData(1, "Account=" + Acc2);
	string UNum = to_string(D.User1[0].Account);
	string UName = D.User1[0].UName;
	
	return "7#Y#" + UNum + "#" + UName;
}

string UserManage::ChangeData(string buf)//buf:用户账号|要修改的属性名|修改后的内容;return : 8#Success(成功完成）
{
	char* acc = new char[buf.length() + 1];
	char* col = new char[buf.length() + 1];
	char* cdata = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%s", acc, col, cdata);
	string Acc = acc, Col = col, CData = cdata;
	delete[] acc;
	delete[] col;
	delete[] cdata;
	delete[] Buffer;

	if (Col == "a")
	{
		D.ChangeData(1, "UName = '" + CData + "'", "Account = " + Acc);
	}
	else
	{
		D.ChangeData(1, "Password = '" + CData + "'", "Account = " + Acc);
	}
	

	return "8#Success";
}

string UserManage::SelecteAccount(string buf)//buf:用户账号(string);return : 12#Success(成功完成）
{
	D.DeleteData(1, "Account = " + buf);//删除该用户
	//删除该用户的所有好友关系
	string SQL = "CMember LIKE '%" + buf + "%' AND COwner = 1";
	D.DeleteData(2, SQL);
	//更新所在群组的群成员:		Member1|Member2|Member3|
	string SQL1 = "CMember LIKE '%" + buf + "%' AND COwner != 1";
	D.SelectData(2, SQL1);
	size_t n = D.Cluster1.size();
	for (size_t i = 0; i < n; i++)
	{
		string CNum = to_string(D.Cluster1[i].CNum);
		string COwner = to_string(D.Cluster1[i].COwner);
		if (COwner == buf)
		{
			D.DeleteData(2, "CNum=" + CNum);
		}
		string CMember = D.Cluster1[i].CMember;
		string NMember = "";
		char* CM = new char[CMember.length() + 1];
		strcpy(CM, CMember.c_str());
		char* Member;
		Member = strtok(CM, "|");//字符串拆分strtok()
		while (Member != NULL)
		{
			string Man = Member;
			if (Man != buf)
			{
				NMember += Man + "|";
			}
			Member = strtok(NULL, "|");
		}
		D.ChangeData(2, "CMember = '" + NMember + "'", "CNum = " + CNum);
		delete[] CM;
	}
	
	return "12#Success";
}

string UserManage::SelecteGroup(string buf)//buf:发出请求的用户账号|好友账号;return : 13#好友账号#Success(成功完成）
{
	char* acc = new char[buf.length() + 1];
	char* num = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", acc, num);
	string Num = num, Acc = acc;
	delete[] acc;
	delete[] num;
	delete[] Buffer;

	string Member1 = Acc + "|" + Num;
	string Member2 = Num + "|" + Acc;

	D.DeleteData(2, "CMember='" + Member1 + "' or CMember='" + Member2 + "'");
	return "13#" + Num + "#Success";
}
