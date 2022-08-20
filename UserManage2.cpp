#include "UserManage.h"


string UserManage::Register(string buf) //buf:nickname,password,5
{
	string NickName;
	string PassWord;

	char Buffer[48];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,n", &NickName, &PassWord);
/*TODO SQL : 向User表中添加一整行data*/
	string data = to_string(MaxAccount) + "#" + NickName + "#" + PassWord + "#" + to_string(0) + "#" + to_string(0) + "#" + to_string(0) + "#" + to_string(0);
	MaxAccount++;

	return to_string(MaxAccount - 1);

}
string UserManage::Logging(string buf)//buf:账号，密码
{
	//字符串分割
	string Acc;
	string PWord;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s", &Acc, &PWord);

	//在User表中寻找该账号对应的密码
/*TODO SQL : 在User表找Account = Acc的行，得到Password（string）*/
	string Password;
	if () //如果搜索结果不为空
	{
		if (Password == PWord)
		{
			return string("Success");
		}
		else
		{
			return string("Failed");
		}
	}
	else
	{
		return string("Not Exist");
	}
}

string UserManage::LogSuccess(string buf)//buf:账号，密码
{
		//字符串分割
	string Acc;
	string PWord;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s", &Acc, &PWord);

	//数据库操作
/*TODO SQL : 在User表中搜索Account = Acc，得到Account(int), UName(string), Portrait（int)*/
	int Account, Portrait;
	string UName;
	string Data1 = to_string(Account) + ',' + UName + ',' + to_string(Portrait);


	//好友账号（同账号昵称头像一起作为返回值）QUE:多个好友账号怎样返回？（好友1|好友2|好友3|）？
/*TODO SQL : 在Cluster表中搜索CMember LIKE %Acc% 且 COwner == 1,返回CMember[ACC1|ACC2]字符串数组形式vector<string> Friend*/
	vector<string> Friend;
	string AllFriend = "";
	for (int i = 0; i < Friend.size(); i++)
	{
		string F, A;
		char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
		strcpy(Buffer, Friend[i].c_str());
		sscanf(Buffer, "%s,%s", &A, &F);
		AllFriend += F + "|";
	}
	//群聊编号（同账号昵称头像一起作为返回值）QUE:多个群聊怎样返回？（群1|群2|群3|）？
/*TODO SQL : 在Cluster表中搜索CMember LIKE% Acc% 且 COwner != 1, 返回CNum数组形式vector<int> Group*/
	vector<int> Group;
	string AllGroup = "";
	for (int i = 0; i < Group.size(); i++)
	{
		AllFriend += to_string(Group[i]) + "|";
	}

	return Data1 + AllFriend + AllGroup;
	//群组聊天信息（每找到一条应该发送的信息就直接发送给当前用户）
/*TODO SQL : 在User表中搜索Account == Acc, 返回OffLineTime, Socket*/
	string OffLineTime;
	int Socket;
/*TODO SQL : 在Cluster表中搜索CMember LIKE% Acc% ，返回MsgRecord（vector<string> MsgRecord)*/
	vector<string> MsgRecord;
	for (int i = 0; i < MsgRecord.size(); i++)
	{
		string msg = MsgRecord[i];
		//字符串拆分
		string Time, Acc, Msg;
		char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
		strcpy(Buffer, msg.c_str());
		sscanf(Buffer, "%s,%s,%s", &Time, &Acc, &Msg);
		
		if (Time > OffLineTime)	//如果消息时间晚于上次下线时间   （字符串比较是逐位比较）
		{
			SendMsg(msg, Socket);
		}
	}
}

string UserManage::Receive(string buf)//buf:群组编号，用户账号，发送时间，消息内容
{
		//字符串分割
	string CNum, Acc, Time, Msg;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,%s,%s", &CNum, &Acc, &Time, &Msg);
	//群发消息
	string SMsg = Time + "#" + Acc + "#" + Msg;//群组聊天记录的格式
/*TODO SQL : 在Cluster表中搜索CNum = CNum,得到MsgRecord(string)*/
	string MsgRecord;
	string RMsg = SMsg + "|" + MsgRecord;
/*TODO SQL : 在Cluster表中搜索CNum = CNum,更新MsgRecord为RMsg*/
/*TODO SQL : 在Cluster表中搜索CNum = CNum,得到CMember(string,|隔开)*/
	string CMember;
		//得到每个群组成员的账号
	char* CM = new char[CMember.length() + 1];
	strcpy(CM, CMember.c_str());	//CMember(string)->CM(char*)
	char* User;
	User = strtok(CM, "|");//字符串拆分strtok()
	while (User != NULL)
	{
		Acc = atoi(User);
		//对每个群组成员的账号Acc：
/*TODO SQL : 在User表中搜索Account = Acc，得到LogStatus（int)*/
		int LogStatus;
		if (LogStatus == 1)
		{
			//在User表中搜索Account=Acc，得到Socket（int)
/*TODO SQL : 在User表中搜索Account = Acc，得到Socket（int)*/
			int Socket;
			SendMsg(SMsg, Socket);
		}
		User = strtok(NULL, "|");
	}

	return string();
}

string UserManage::SendMsg(string buf, int confd)//单发消息，buf:发送时间，发信人账号，消息内容；confd:收信人socket
{
	write(confd, buf, sizeof(buf));
	return string();
}

string UserManage::SearchUser(string buf)//buf:用户账号
{
	int account = atoi(buf.c_str());//被搜索用户的账号由string转为int格式
	string data;
/*TODO SQL : 在User表Account列中搜索account，得到那一行的信息:Account(int),UName(string),Portrait(int),LogStatus(int),OffLineTime(string)*/
	int Account, Portrait, LogStatus;
	string UName, OffLineTime;
	if () /*如果返回结果不为空*/
	{
		data = to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
		return data;
	}
/*TODO SQL : 在User表UName列中搜索buf，得到那一行的信息:Account(int),UName(string),Portrait(int),LogStatus(int),OffLineTime(string)*/
	else if ()/*如果返回结果不为空*/
	{
		data = to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
		return data;
	}
	else {
		return string("The User Does Not Exist!");
	}
}

string UserManage::AddFriend1(string buf)//buf:用户账号，好友账号，备注信息;该函数：发送好友请求
{
	//字符串分割
	string Acc1;
	string Acc2;
	string Msg;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,%s", &Acc1, &Acc2, &Msg);
	//找到被邀请人的信息（通过Acc2找到Socket、UName，记为confd,UName）    
/*在User表中搜索Account = Acc2,得到Socket(int)*/
	int Socket;
/*在User表中搜索Account = Acc1,得到UName(string)*/
	string UName;

	//拼接需要发送的信息,发送
	string SMsg = UName + "," + Acc1 + "," + to_string(Socket) + "," + "7";
	SendMsg(SMsg,Socket); //将好友请求以消息的形式发给被邀请人   (好友请求直接以消息的形式发送吗？？？）
	//return SMsg;
}

string UserManage::AddFriend2(string buf)//buf:accept|reject,用户账号，好友账号
{
	//字符串分割
	string Acc1;
	string Acc2;
	string A_R;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,%s", &A_R, &Acc1, &Acc2);

	//找到邀请人（根据Account得到Socket,记为confd(int))
/*在User表中搜索Account = Acc1,得到Socket(int)*/
	int Socket;
	//找到被邀请人（根据Account得到UName(string)) 
/*在User表中搜索Account = Acc2,得到UName(string)*/
	string UName;

	if (A_R == "reject")
	{
		//给邀请人发送信息
		string SMsg = "You are rejected by " + UName + Acc2;
		//SendMsg(SMsg, confd); //这个返回给邀请人的信息是直接以消息的形式发送还是以return？？？
		return SMsg;
	}
	else if (A_R == "accept")
	{
		//创建群组，即在群组表中添加一整行
		string data;
		string Member = Acc1 + "|" + Acc2;
/*Cluster表，得到CNum一列vector<int> CNum*/
		vector<int> CNum;
		vector<int>::iterator itMax = max_element(CNum.begin(), CNum.end());
		int MaxCNum = *itMax; 
		data = "" + '#' + '1' + '#' + to_string(MaxCNum+ 1) + '#' + Member + '#' + "";//群组名和聊天记录为空
/*Cluster表，新增一行，数据为data*/

			//给邀请人发送信息
		string SMsg = UName + ',' + Acc2 + " accepted your invitation.";
		//SendMsg(SMsg, confd);
		return SMsg;
	}
}

string UserManage::ChangeData(string buf)//buf:用户账号，要修改的属性名，修改后的内容
{
	//字符串分割
	string Acc;
	string Col;
	string CData;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,%s", &Acc, &Col, &CData);

/*TODO SQL : User表，在Account= Acc的行，更新Col = CData*/

	return string("Success");
}

string UserManage::SelecteAccount(string buf)//buf:用户账号，string形式
{
	buf = atoi(buf.c_str());//string->int
/*TODO SQL : User表，在Account= Acc的行，删除一整行*/
	return string("Success");
}

string UserManage::SelecteGroup(string buf)//buf:群组编号,发出请求的用户账号
{
	//字符串分割
	string CNum, Acc;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s", &CNum, &Acc);
	//查看该群群主
/*TODO SQL : Cluster表, CNum = CNum的行，得到COwner(string)*/
	string COwner;
	if (COwner == Acc || COwner == "1")//该用户是群主则可以解散该群，否则不可以
	{
/*TODO SQL : Cluster表，CNum = CNum的行，删除一整行*/
		return string("Success");
	}
	return string("Failed");
}
