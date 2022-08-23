#include "GroupManage.h"
void Stringsplit1(const string& str, const string& splits, vector<string>& res)
{
	if (str == "")		return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	string strs = str + splits;
	size_t pos = strs.find(splits);
	int step = splits.size();

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + step, strs.size());
		pos = strs.find(splits);
	}
}
//buf：用户账号|群聊名称；return：2#账号#群名#群号
string GroupManage::CreatGroup(string buf) {
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0];
	string CName = res[1];
	string sql;
	D.SelectData(2);//获取群号
	size_t n = D.Cluster1.size() - 1;
	int MaxCNum =  D.Cluster1[n].CNum;
	string data = to_string(MaxCNum+1) + ",'" + CName + "'," + Acc + ",'"+Acc+ "'," + "'/0'";
	if (!D.AddData(2, data)) {
		cout << "add data error" << endl;
	}
	return string("2#" + Acc + "#" + CName + "#" + to_string(MaxCNum + 1));
}
//buf: 用户账号|群号|被邀请人账号； return：3#账号#y/3#账号#n
string  GroupManage::Invite(string buf){
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc1 = res[0], Acc2 = res[2], CNum = res[1];
	//判断该用户是否已经在群中
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
//buf:用户账号|群号；return:1#账号#y#群号#群名/1#账号#n#该群不存在   
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
		//通过群号查询到了群
		CNum = D.Cluster1[0].CNum;
		CName = D.Cluster1[0].CName;
		COwner = D.Cluster1[0].COwner;
		string CMember = D.Cluster1[0].CMember;
		string record = D.Cluster1[0].MsgRecord;
		//加入该群
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
		{//通过群名查询有结果
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
			//没有该群
			return string("1#"+Acc+ "#n#"+"The Cluster Does Not Exist!");
		}
	}

}
//buf：用户账号 | 群号；return：4#账号#y / 4#账号#n
string   GroupManage::Dissolve(string buf){
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0], CNum = res[1];
	//查询群
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
//buf：用户账号|群号；return：14#账号#群号#群成员昵称       （昵称：昵称1|昵称2）
string GroupManage::ShowMember(string buf) {
	vector<string> res;
	Stringsplit1(buf, "|", res);
	string Acc = res[0], CNum = res[1];
	//得到群成员账号
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