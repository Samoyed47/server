#include "UserManage.h"


string UserManage::Register(string buf) //buf:nickname|password; return:5#账号(string)
{
	string NickName;
	string PassWord;
	string num;
	char Buffer[48];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s", &num, &NickName, &PassWord);
/*TODO SQL : 向User表中添加一整行data*/
	string data = to_string(MaxAccount) + "#" + NickName + "#" + PassWord + "#" + to_string(0) + "#" + to_string(0) + "#" + to_string(0) + "#" + to_string(0);
	MaxAccount++;

	return string("5" + '# '+to_string(MaxAccount - 1));

}
string UserManage::Logging(string buf)//buf:账号|密码; return:9#成功/失败/不存在
{
	//字符串分割
	string Acc;
	string PWord;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s", &Acc, &PWord);

	//在User表中寻找该账号对应的密码
/*TODO SQL : 在User表找Account = Acc的行，得到Password（string）*/
	string Password;
	string ans;
	if () //如果搜索结果不为空
	{
		if (Password == PWord)
		{
			ans = "9" + '#' + string("Success");
			LogSuccess(buf);
		}
		else
		{
			ans = "9" + '#' + string("Failed");
		}
	}
	else
	{
		ans = "9" + '#' + string("Not Exist");
	}
	return ans;
}

string UserManage::LogSuccess(string buf)//buf:账号|密码; return:9#账号#昵称#头像#所有好友账号#所有群聊编号
{
		//字符串分割
	string Acc;
	string PWord;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s", &Acc, &PWord);

	//数据库操作
/*TODO SQL : 在User表中搜索Account = Acc，得到Account(int), UName(string), Portrait（int)*/
	int Account, Portrait;
	string UName;
	string Data1 = to_string(Account) + '#' + UName + '#' + to_string(Portrait);

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

	//好友账号（同账号昵称头像一起作为返回值）QUE:多个好友账号怎样返回？（好友1|好友2|好友3|）？
/*TODO SQL : 在Cluster表中搜索CMember LIKE %Acc% 且 COwner == 1,返回CMember[ACC1|ACC2]字符串数组形式vector<string> Friend*/
	vector<string> Friend;
	string AllFriend = "";
	for (int i = 0; i < Friend.size(); i++)
	{
		string F, A;
		char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
		strcpy(Buffer, Friend[i].c_str());
		sscanf(Buffer, "%s|%s", &A, &F);
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

	return "9" + '#' + Data1 + '#' + AllFriend + '#' +  AllGroup;
	
}

string UserManage::Receive(string buf)//buf:群号或好友账号|用户账号|发送时间|消息内容; return:10#Done(表示收到消息且已发出）
{
		//字符串分割
	string Num, Acc, Time, Msg;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s|%s|%s", &Num, &Acc, &Time, &Msg);

	//得到群组编号
	string CNum;
	if (buf < "40000000")//buf是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
/*TODO SQL : Cluster表, CMember = Member1 or CMember = Member2 的行，得到CNum(int)*/
		int Num;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}

	//群发消息
	string SMsg = Time + "," + Acc + "," + Msg;//群组聊天记录的格式
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
			SendMsg("10" + '#' + SMsg, Socket);
		}
		User = strtok(NULL, "|");
	}

	return "10" + '#' + string("Done");
}

string UserManage::SendMsg(string buf, int confd)//单发消息，buf:发送时间，发信人账号，消息内容；confd:收信人socket;;;return: 11#Done(收信人已收到)
{
	write(confd, buf, sizeof(buf));
	return "11" + '#' + string("Done");
}

string UserManage::SearchUser(string buf)//buf:用户账号; return : 6#账号#昵称#头像#登录状态#上次下线时间 / 该用户不存在
{
	int account = atoi(buf.c_str());//被搜索用户的账号由string转为int格式
	string data;
/*TODO SQL : 在User表Account列中搜索account，得到那一行的信息:Account(int),UName(string),Portrait(int),LogStatus(int),OffLineTime(string)*/
	int Account, Portrait, LogStatus;
	string UName, OffLineTime;
	if () /*如果返回结果不为空*/
	{
		data = "6" + '#' + to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
		return data;
	}
/*TODO SQL : 在User表UName列中搜索buf，得到那一行的信息:Account(int),UName(string),Portrait(int),LogStatus(int),OffLineTime(string)*/
	else if ()/*如果返回结果不为空*/
	{
		data = "6" + '#' + to_string(Account) + "#" + UName + to_string(Portrait) + "#" + to_string(LogStatus) + OffLineTime;
		return data;
	}
	else {
		return "6" + '#' + string("The User Does Not Exist!");
	}
}

string UserManage::AddFriend1(string buf)//buf:用户账号|好友账号|备注信息;该函数：发送好友请求；return : 7#sended（已发送）
{
	//字符串分割
	string Acc1;
	string Acc2;
	string Msg;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s|%s", &Acc1, &Acc2, &Msg);

		//建立两人群组
	//创建群组，即在群组表中添加一整行
	string data;
	string Member = Acc1 + "|" + Acc2;
/*TODO SQL : Cluster表，得到CNum一列vector<int> CNum*/
	vector<int> CNum;
	vector<int>::iterator itMax = max_element(CNum.begin(), CNum.end());
	int MaxCNum = *itMax;
	data = "" + '#' + '1' + '#' + to_string(MaxCNum + 1) + '#' + Member + '#' + "";//群组名和聊天记录为空
/*TODO SQL : Cluster表，新增一行，数据为data*/

	//找到被邀请人的信息（通过Acc2找到Socket、UName，记为confd,UName）    
/*TODO SQL : 在User表中搜索Account = Acc2,得到Socket(int)*/
	int Socket;
/*TODO SQL : 在User表中搜索Account = Acc1,得到UName(string)*/
	string UName;

	//拼接需要发送的信息,发送
	string SMsg = "11" + '#' + UName + "," + Acc1 + "," + to_string(Socket);
	SendMsg(SMsg,Socket); //将好友请求以消息的形式发给被邀请人  
	//return SMsg;
	return "7" + '#' + string("sended");
}

string UserManage::AddFriend2(string buf)//buf:accept/reject|用户账号|好友账号；return : 7#accept/reject
{
	//字符串分割
	string Acc1;
	string Acc2;
	string A_R;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s|%s", &A_R, &Acc1, &Acc2);

	//找到邀请人（根据Account得到Socket,记为confd(int))
/*TODO SQL : 在User表中搜索Account = Acc1,得到Socket(int)*/
	int Socket;
	//找到被邀请人（根据Account得到UName(string)) 
/*TODO SQL : 在User表中搜索Account = Acc2,得到UName(string)*/
	string UName;

	if (A_R == "reject")
	{
/*TODO SQL : Cluster表，搜索CMember = Acc1|Acc2,删除该行*/
		//给邀请人发送信息
		string SMsg = "7" + '#' + string("You are rejected by ") + UName + Acc2;
		SendMsg(SMsg, Socket); 
		return "7" + '#' + string("reject");
	}
	else if (A_R == "accept")
	{
			//给邀请人发送信息
		string SMsg = "7" + '#' + UName + ',' + Acc2 + " accepted your invitation.";
		SendMsg(SMsg, Socket);
		return "7" + '#' + string("accept");
	}
}

string UserManage::ChangeData(string buf)//buf:用户账号|要修改的属性名|修改后的内容;return : 8#Success(成功完成）
{
	//字符串分割
	string Acc;
	string Col;
	string CData;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s|%s", &Acc, &Col, &CData);

	int acc = atoi(Acc.c_str());
/*TODO SQL : User表，在Account= acc的行，更新Col = CData*/

	return "8" + '#' + string("Success");
}

string UserManage::SelecteAccount(string buf)//buf:用户账号(string);return : 12#Success(成功完成）
{
	buf = atoi(buf.c_str());//string->int
/*TODO SQL : User表，在Account= Acc的行，删除一整行*/
	return "12" + '#' + string("Success");
}

string UserManage::SelecteGroup(string buf)//buf:群号或好友账号|发出请求的用户账号;return : 13#Success(成功完成）/Failed（操作无效）
{
	//字符串分割
	string Num, Acc;
	char Buffer[1024];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s|%s", &Num, &Acc);

	//得到群组编号
	string CNum;
	if (buf < "40000000")//buf是好友账号
	{
		string Member1 = Acc + "|" + Num;
		string Member2 = Num + "|" + Acc;
/*TODO SQL : Cluster表, CMember = Member1 or CMember = Member2 的行，得到CNum(int)*/
		int Num;
		CNum = to_string(Num);
	}
	else
	{
		CNum = Num;
	}

	//查看该群群主
/*TODO SQL : Cluster表, CNum = CNum的行，得到COwner(string)*/
	string COwner;
	if (COwner == Acc || COwner == "1")//该用户是群主则可以解散该群，否则不可以
	{
/*TODO SQL : Cluster表，CNum = CNum的行，删除一整行*/
		return "13" + '#' + string("Success");
	}
	return "13" + '#' + string("Failed");
}
