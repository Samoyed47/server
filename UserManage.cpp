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

	D.SelectData(1, "\0");
	size_t n = D.User1.size() - 1;
	int MaxAccount = D.User1[n].Account;

	string data = to_string(MaxAccount+1) + "," + NickName + "," + PassWord + "," + "NULL" + "," + to_string(0) + "," + "NULL" + "," + "\0";
	D.AddData(1, data);

	return string("5#" + to_string(MaxAccount - 1));
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

	string SQL = "CMember LIKE %" + Acc + "%";
	D.SelectData(2, SQL);
	vector<string> MsgRecord;
	//MsgRecord.clear();
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		char* S = new char[D.Cluster1[i].MsgRecord.length() + 1];
		char* Buffer = new char[D.Cluster1[i].MsgRecord.length() + 1];
		strcpy(Buffer, D.Cluster1[i].MsgRecord.c_str()); //D.Cluster1[i].MsgRecord:'Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3'
		sscanf(Buffer, "%*[']%[^']", S);
		string s = S;
		MsgRecord.push_back(s);
	}
	for (size_t i = 0; i < MsgRecord.size(); i++)//MsgRecord[i]:Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3
	{
		if (i >= MsgRecord.size() || i < 0) break;
		char* MR = new char[MsgRecord[i].length() + 1];
		strcpy(MR, MsgRecord[i].c_str());
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
			if (Time > OffLineTime)	//如果消息时间晚于上次下线时间   （字符串比较是逐位比较）
			{
				string SMsg = AMsg;
				SendMsg("10#" + SMsg, Socket);
			}
			AMsg = strtok(NULL, "|");
		}
	}

	string SQL1 = "CMember LIKE %" + Acc + "% AND COwner = 1";
	D.SelectData(2, SQL1);

	string AllFriend = "";
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
		if (A == Acc)
		{
			AllFriend += F + "|";
		}
		else
		{
			AllFriend += A + "|";
		}
	}

	string SQL2 = "CMember like %" + Acc + "% and COwner <> 1";
	D.SelectData(2, SQL2);

	string AllGroup = "";
	for (size_t i = 0; i < D.Cluster1.size(); i++)
	{
		if (i >= D.Cluster1.size() || i < 0) break;
		AllFriend += to_string(D.Cluster1[i].CNum) + "|";
	}

	return "9#" + Data1 + "#" + AllFriend + "#" + AllGroup;
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

	string CNum;
	if (buf < "40000000")//buf是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
		string SQL = "CMember = " + Member1 + "or CMember = " + Member2;
		D.SelectData(2, SQL);
		int Num = D.Cluster1[0].CNum;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}
	string SMsg = "|" + Time + "," + Acc + "," + Msg + "'";

	D.SelectData(2, "CNum = " + CNum);
	string MsgRecord = D.Cluster1[0].MsgRecord;//'Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3'
	char* M = new char[MsgRecord.length() + 1];
	char* Ms = new char[MsgRecord.length() + 1];
	strcpy(M, MsgRecord.c_str());
	sscanf(M, "%s'", Ms);//MsgRecord:'Time1,Acount1,Msg1|Time2,Account2,Msg2|Time3,Account3,Msg3
	MsgRecord = Ms;
	string RMsg = MsgRecord + SMsg;

	D.ChangeData(2, "MsgRecord = " + RMsg, "CNum = " + CNum);

	D.SelectData(2, "CNum = " + CNum);
	string CMember = D.Cluster1[0].CMember;
	char* CM = new char[CMember.length() + 1];
	strcpy(CM, CMember.c_str());
	char* User;
	User = strtok(CM, "|");//字符串拆分strtok()
	while (User != NULL)
	{
		Acc = atoi(User);
		D.SelectData(1, "Account = " + Acc);
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

string UserManage::SendMsg(string buf, int confd)//单发消息，buf:发送时间，发信人账号，消息内容；confd:收信人socket;;;return: 11#Done(收信人已收到)
{
	//write(confd, buf, sizeof(buf));
	return "11#Done";
}

string UserManage::SearchUser(string buf)//buf:用户账号; return : 6#账号#昵称#头像#登录状态#上次下线时间 / 该用户不存在
{
	string data;
	D.SelectData(1, "Account = " + buf);
	int Account, Portrait, LogStatus;
	string UName, OffLineTime;
	if (!D.User1.empty())
	{
		Account = D.User1[0].Account;
		Portrait = D.User1[0].Potrait;
		LogStatus = D.User1[0].LogStatus;
		UName = D.User1[0].UName;
		OffLineTime = D.User1[0].OffLineTime;
		data = "6#" + to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
		return data;
	}
	else
	{
		D.SelectData(1, "UName = " + buf);
		if (!D.User1.empty())
		{
			Account = D.User1[0].Account;
			Portrait = D.User1[0].Potrait;
			LogStatus = D.User1[0].LogStatus;
			UName = D.User1[0].UName;
			OffLineTime = D.User1[0].OffLineTime;
			data = "6#"+ to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
			return data;
		}
		else
		{
			return "6#The User Does Not Exist!";
		}
	}
}

string UserManage::AddFriend1(string buf)//buf:用户账号|好友账号|备注信息;该函数：发送好友请求；return : 7#sended（已发送）/You are already friends(已经是好友）
{
	char* acc1 = new char[buf.length() + 1];
	char* acc2 = new char[buf.length() + 1];
	char* msg = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%s", acc1, acc2, msg);
	string Acc1 = acc1, Acc2 = acc2, Msg = msg;

	//判断两人是否已经是好友
	string Member1 = Acc1 + "|" + Acc2;
	string Member2 = Acc2 + "|" + Acc1;
	string SQL = "CMember = " + Member1 + "or CMember = " + Member2;
	D.SelectData(2, SQL);
	if (!D.Cluster1.empty())
	{
		return "7#You are already friends";
	}

	string data;
	string Member = Acc1 + "|" + Acc2;

	D.SelectData(2, "\0");
	size_t n = D.Cluster1.size() - 1;
	int MaxCNum = D.Cluster1[n].CNum;

	data = "\0" + ',' + '1' + ',' + to_string(MaxCNum + 1) + ',' + Member + ',' + "\0"; //?????
	D.AddData(2, data);

	D.SelectData(1, "Account = " + Acc2);
	int Socket = D.User1[0].Socket;
	D.SelectData(1, "Account = " + Acc1);
	string UName = D.User1[0].UName;

	string SMsg = "11#"+ UName + "," + Acc1 + "," + to_string(Socket);
	SendMsg(SMsg, Socket); //将好友请求以消息的形式发给被邀请人  

	return "7#sended";
}

string UserManage::AddFriend2(string buf)//buf:accept/reject|用户账号|好友账号；return : 7#accept/reject
{
	char* acc1 = new char[buf.length() + 1];
	char* acc2 = new char[buf.length() + 1];
	char* ar = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%[^|]|%s", ar, acc1, acc2);
	string Acc1 = acc1, Acc2 = acc2, A_R = ar;

	D.SelectData(1, "Account = " + Acc1);
	int Socket = D.User1[0].Socket;
	D.SelectData(1, "Account = " + Acc2);
	string UName = D.User1[0].UName;

	if (A_R == "reject")
	{
		D.DeleteData(2, "CMember = " + Acc1 + "|" + Acc2);

		string SMsg = "7#You are rejected by " + UName + Acc2;
		SendMsg(SMsg, Socket);
		return "7#reject";
	}
	else if (A_R == "accept")
	{
		string SMsg = "7#" + UName + "," + Acc2 + " accepted your invitation.";
		SendMsg(SMsg, Socket);
		return "7#accept";
	}
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

	D.ChangeData(1, Col + " = " + CData, "Account = " + Acc);

	return "8#Success";
}

string UserManage::SelecteAccount(string buf)//buf:用户账号(string);return : 12#Success(成功完成）
{
	D.DeleteData(1, "Account = " + buf);
	return "12#Success";
}

string UserManage::SelecteGroup(string buf)//buf:发出请求的用户账号|群号或好友账号;return : 13#Success(成功完成）/Failed（操作无效）
{
	char* acc = new char[buf.length() + 1];
	char* num = new char[buf.length() + 1];
	char* Buffer = new char[buf.length() + 1];
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%[^|]|%s", acc, num);
	string Num = num, Acc = acc;

	string CNum;
	if (buf < "40000000")//buf是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
		string SQL = "CMember = " + Member1 + "or CMember = " + Member2;
		D.SelectData(2, SQL);
		int Num = D.Cluster1[0].CNum;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}

	D.SelectData(2, "CNum = " + CNum);
	int COwner = D.Cluster1[0].COwner;
	if (COwner == atoi(Acc.c_str()) || COwner == 1)//该用户是群主则可以解散该群，否则不可以
	{
		D.DeleteData(2, "CNum = " + CNum);
		return "13#Success";
	}
	return "13#Failed";
}
