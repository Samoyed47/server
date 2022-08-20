#include "UserManage.h"


string UserManage::Register(string buf) //buf:nickname,password,5
{
	string NickName;
	string PassWord;

	char Buffer[48];	//将传入的string转为char *以使用sscanf函数
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,n", &NickName, &PassWord);
/*TODO SQL : 向User表中添加一整行即data，以字符串的形式、逗号隔开的方式表示每个属性值*/
	string data = to_string(MaxAccount) + "," + NickName + "," + PassWord + "," + " " + "," + to_string(0) + "," + to_string(0) + "," + to_string(0);//头像这里用了一个空格，socket和下线时间此时设为0？
	bool Ok = AddData('User', data);//调用函数，增加一整行
	MaxAccount++;

	if (Ok) {
		return to_string(MaxAccount - 1);
	}

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
	if (sqlite3_exec() == SQLITE_OK) //如果搜索结果不为空
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
/*TODO SQL : 在User表中搜索Account = Acc，得到Account, UName, Portrait（string)*/
	string Data1 = to_string(Account) + ',' + UName + ',' + to_string(Portrait);


	//好友账号（同账号昵称头像一起作为返回值）QUE:多个好友账号怎样返回？（好友1|好友2|好友3|）？
/*TODO SQL : 在Cluster表中搜索CMember LIKE %Acc% 且 COwner == 1,返回CMember[ACC1|ACC2]字符串数组形式vector<string> Friend*/
	vector<string> Friend;
	string AllFriend = "";
	for (int i = 0; i < Friend.size(); i++)
	{
		AllFriend += Friend[i] + "|";
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
		
		if (Time > OffLineTime)	//如果消息时间晚于上次下线时间   （时间比较怎么做？？？）
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
	string SMsg = Time + "," + Acc + "," + Msg;//群组聊天记录的格式
/*TODO SQL : 在Cluster表中搜索CNum = CNum,得到MsgRecord(string)*/
	string RMsg = MsgRecord + "|" + SMsg;
/*TODO SQL : 在Cluster表中搜索CNum = CNum,更新MsgRecord为RMsg*/
/*TODO SQL : 在Cluster表中搜索CNum = CNum,得到CMember(string,|隔开),这个搜索可能会有多行结果，只取一行就行*/
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
		if (LogStatus == 1)
		{
			//在User表中搜索Account=Acc，得到Socket（int)
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
