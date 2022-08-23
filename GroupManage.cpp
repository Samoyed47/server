#include "GroupManage.h"
void Stringsplit1(const string& str, const string& splits, vector<string>& res)
{
	if (str == "")		return;
	//���ַ���ĩβҲ����ָ����������ȡ���һ��
	string strs = str + splits;
	size_t pos = strs.find(splits);
	int step = splits.size();

	// ���Ҳ����������ַ��������������� npos
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//ȥ���ѷָ���ַ���,��ʣ�µ��ַ����н��зָ�
		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
}
//buf���û��˺�|Ⱥ�����ƣ�return��2#�˺�#Ⱥ��#Ⱥ��
string GroupManage::CreatGroup(string buf) {
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0];
	string CName = res[1];
	string sql;
	D.SelectData(2);//��ȡȺ��
	size_t n = D.Cluster1.size() - 1;
	int MaxCNum =  D.Cluster1[n].CNum;
	string data = to_string(MaxCNum+1) + ",'" + CName + "'," + Acc + ",'"+Acc+ "'," + "'/0'";
	if (!D.AddData(2, data)) {
		cout << "add data error" << endl;
	}
	return string("2#" + Acc + "#" + CName + "#" + to_string(MaxCNum + 1));
}
//buf: �û��˺�|Ⱥ��|���������˺ţ� return��3#�˺�#y/3#�˺�#n
string  GroupManage::Invite(string buf){
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc1 = res[0], Acc2 = res[2], CNum = res[1];
	//�жϸ��û��Ƿ��Ѿ���Ⱥ��
	string sql = "CNum="+CNum;
	if (D.SelectData(2, sql) && D.Cluster1[0].CMember.find(Acc2)!=string::npos) {
		return string("3#" + Acc1 + "#n");
	}
	string Member;
	if (D.Cluster1[0].CMember == "/0") {
		Member = Acc2;
	}
	else {
        Member = D.Cluster1[0].CMember + "|" + Acc2;
	}
	
	sql = "CMember='" + Member + "'";
	string where = "CNum=" + CNum;
	if (!D.ChangeData(2, sql,where)) {
		cout << "change data error" << endl;
	}
	return string("3#" + Acc1 + "#y");
}
//buf:�û��˺�|Ⱥ�ţ�return:1#�˺�#y#Ⱥ��#Ⱥ��/1#�˺�#n#��Ⱥ������   
string  GroupManage::Search(string buf){
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Search = res[1];
	string Acc = res[0];
	string sql;
	sql = "CNum=" + Search;
	string data;
	int CNum,COwner;
	string CName;
	if (!D.SelectData(2, sql))
	{
		cout << "select error" << endl;
	}
	if (D.Cluster1.size() != 0) {
		//ͨ��Ⱥ�Ų�ѯ����Ⱥ
		CNum = D.Cluster1[0].CNum;
		CName = D.Cluster1[0].CName;
		COwner = D.Cluster1[0].COwner;
		string CMember = D.Cluster1[0].CMember;
		string record = D.Cluster1[0].MsgRecord;
		//�����Ⱥ
		if (CMember == "/0") {
			CMember = Acc;
		}
		else {
            CMember = CMember + "|"+Acc;
		}
		string sql2 = "CMember='" + CMember+"'";
		string where = "CNum=" + Search;
		if (!D.ChangeData(2, sql, where)) {
			cout << "changedata error" << endl;
		}
		data = "1#" + Acc+"#y#"+to_string(CNum) + "#" + CName;
		return data;
	}
	else
	{
		if (D.SelectData(2, "CName='" + Search + "'") && !D.Cluster1.empty())
		{//ͨ��Ⱥ����ѯ�н��
			size_t n = D.Cluster1.size();
			data = "1#";
			for (size_t i = 0; i < n - 1; i++)
			{
				CNum = D.Cluster1[0].CNum;
				CName = D.Cluster1[0].CName;
				COwner = D.Cluster1[0].COwner;
				data = data+to_string(CNum) + "+" +CName +"#" + to_string(COwner)+";";
			}
			return data;
		}
		else
		{
			//û�и�Ⱥ
			return string("1#"+Acc+ "#n#"+"The Cluster Does Not Exist!");
		}
	}

}
//buf���û��˺� | Ⱥ�ţ�return��4#�˺�#y / 4#�˺�#n
string   GroupManage::Dissolve(string buf){
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0], CNum = res[1];
	//��ѯȺ
	string sql = "CNum=" + CNum;
	if (!D.SelectData(2, sql)) {
		cout << "select error" << endl;
	}
	if (D.Cluster1.empty()|| D.Cluster1[0].COwner != atoi(Acc.c_str())) {
		return string("4#" + Acc + "#n");
	}
	sql = "CNum=" + CNum;
	if (!D.DeleteData(2, sql)) {
		cout << "delect error" << endl;
	}
	return string("4#" + Acc + "#y");
}
//buf���û��˺�|Ⱥ�ţ�return��14#�˺�#Ⱥ��#Ⱥ��Ա�ǳ�       ���ǳƣ��ǳ�1|�ǳ�2��
string GroupManage::ShowMember(string buf) {
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0], CNum = res[1];
	//�õ�Ⱥ��Ա�˺�
	string sql = "CNum=" + CNum;
	if (!D.SelectData(2, sql)) {
		cout << "delete error" << endl;
	}
	vector<string>  CMemeber;
	Stringsplit1(D.Cluster1[0].CMember,"|", CMemeber);
	string CNickName="";
	for (int i = 0; i < CMemeber.size(); i++) {
		sql = "Account=" + CMemeber[i];
		if (D.SelectData(1, sql)) {
			if (!D.User1.empty()) {
				if (i == (CMemeber.size() - 1)) {
					CNickName = CNickName + D.User1[0].UName;
				}
				else {
					CNickName = CNickName + D.User1[0].UName + "|";
				}
			}
		}
		else {
			cout << "select error"<<i << endl;
		}
	}
	return string("14#" + Acc + "#" + CNum + "#" + CNickName);
}