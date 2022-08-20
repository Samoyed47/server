#include "UserManage.h"


string UserManage::Register(string buf) //buf:nickname,password,5
{
	string NickName;
	string PassWord;

	char Buffer[48];	//�������stringתΪchar *��ʹ��sscanf����
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,n", &NickName, &PassWord);
/*TODO SQL : ��User�������һ���м�data�����ַ�������ʽ�����Ÿ����ķ�ʽ��ʾÿ������ֵ*/
	string data = to_string(MaxAccount) + "," + NickName + "," + PassWord + "," + " " + "," + to_string(0) + "," + to_string(0) + "," + to_string(0);//ͷ����������һ���ո�socket������ʱ���ʱ��Ϊ0��
	bool Ok = AddData('User', data);//���ú���������һ����
	MaxAccount++;

	if (Ok) {
		return to_string(MaxAccount - 1);
	}

}
string UserManage::Logging(string buf)//buf:�˺ţ�����
{
	//�ַ����ָ�
	string Acc;
	string PWord;
	char Buffer[1024];	//�������stringתΪchar *��ʹ��sscanf����
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s", &Acc, &PWord);

	//��User����Ѱ�Ҹ��˺Ŷ�Ӧ������
/*TODO SQL : ��User����Account = Acc���У��õ�Password��string��*/
	if (sqlite3_exec() == SQLITE_OK) //������������Ϊ��
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

string UserManage::LogSuccess(string buf)//buf:�˺ţ�����
{
		//�ַ����ָ�
	string Acc;
	string PWord;
	char Buffer[1024];	//�������stringתΪchar *��ʹ��sscanf����
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s", &Acc, &PWord);

	//���ݿ����
/*TODO SQL : ��User��������Account = Acc���õ�Account, UName, Portrait��string)*/
	string Data1 = to_string(Account) + ',' + UName + ',' + to_string(Portrait);


	//�����˺ţ�ͬ�˺��ǳ�ͷ��һ����Ϊ����ֵ��QUE:��������˺��������أ�������1|����2|����3|����
/*TODO SQL : ��Cluster��������CMember LIKE %Acc% �� COwner == 1,����CMember[ACC1|ACC2]�ַ���������ʽvector<string> Friend*/
	vector<string> Friend;
	string AllFriend = "";
	for (int i = 0; i < Friend.size(); i++)
	{
		AllFriend += Friend[i] + "|";
	}
	//Ⱥ�ı�ţ�ͬ�˺��ǳ�ͷ��һ����Ϊ����ֵ��QUE:���Ⱥ���������أ���Ⱥ1|Ⱥ2|Ⱥ3|����
/*TODO SQL : ��Cluster��������CMember LIKE% Acc% �� COwner != 1, ����CNum������ʽvector<int> Group*/
	vector<int> Group;
	string AllGroup = "";
	for (int i = 0; i < Group.size(); i++)
	{
		AllFriend += to_string(Group[i]) + "|";
	}

	return Data1 + AllFriend + AllGroup;
	//Ⱥ��������Ϣ��ÿ�ҵ�һ��Ӧ�÷��͵���Ϣ��ֱ�ӷ��͸���ǰ�û���
/*TODO SQL : ��User��������Account == Acc, ����OffLineTime, Socket*/
/*TODO SQL : ��Cluster��������CMember LIKE% Acc% ������MsgRecord��vector<string> MsgRecord)*/
	vector<string> MsgRecord;
	for (int i = 0; i < MsgRecord.size(); i++)
	{
		string msg = MsgRecord[i];
		//�ַ������
		string Time, Acc, Msg;
		char Buffer[1024];	//�������stringתΪchar *��ʹ��sscanf����
		strcpy(Buffer, msg.c_str());
		sscanf(Buffer, "%s,%s,%s", &Time, &Acc, &Msg);
		
		if (Time > OffLineTime)	//�����Ϣʱ�������ϴ�����ʱ��   ��ʱ��Ƚ���ô����������
		{
			SendMsg(msg, Socket);
		}
	}
}

string UserManage::Receive(string buf)//buf:Ⱥ���ţ��û��˺ţ�����ʱ�䣬��Ϣ����
{
		//�ַ����ָ�
	string CNum, Acc, Time, Msg;
	char Buffer[1024];	//�������stringתΪchar *��ʹ��sscanf����
	strcpy(Buffer, buf.c_str());
	sscanf(Buffer, "%s,%s,%s,%s", &CNum, &Acc, &Time, &Msg);
	//Ⱥ����Ϣ
	string SMsg = Time + "," + Acc + "," + Msg;//Ⱥ�������¼�ĸ�ʽ
/*TODO SQL : ��Cluster��������CNum = CNum,�õ�CName,COwner,CNum,CMember(string)����ΪData0,����������ܻ��ж��н����ֻȡһ�о���*/
	string Data2 = Data0;
	Data2 += "," + SMsg;
/*TODO SQL : ��Cluster�������һ�У�����ΪData0��string, ���Ÿ������룩*/
/*TODO SQL : ��Cluster��������CNum = CNum,�õ�CMember(string,|����),����������ܻ��ж��н����ֻȡһ�о���*/
		//�õ�ÿ��Ⱥ���Ա���˺�
	char* CM = new char[CMember.length() + 1];
	strcpy(CM, CMember.c_str());	//CMember(string)->CM(char*)
	char* User;
	User = strtok(CM, "|");//�ַ������strtok()
	while (User != NULL)
	{
		Acc = atoi(User);
		//��ÿ��Ⱥ���Ա���˺�Acc��
/*TODO SQL : ��User��������Account = Acc���õ�LogStatus��int)*/
		if (LogStatus == 1)
		{
			//��User��������Account=Acc���õ�Socket��int)
			SendMsg(SMsg, Socket);
		}
		User = strtok(NULL, "|");
	}

	return string();
}

string UserManage::SendMsg(string buf, int confd)//������Ϣ��buf:����ʱ�䣬�������˺ţ���Ϣ���ݣ�confd:������socket
{
	write(confd, buf, sizeof(buf));
	return string();
}