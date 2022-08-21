#include "databases.h"
/*
CString strFilePath(_T("C:\\pic1.jpg"));
// 读取图片并开辟一块缓冲区以二进制形式存放图片
CFile file;
file.Open(strFilePath, CFile::typeBinary | CFile::modeRead);
int nFileSize = file.GetLength();
BYTE* pbytBuffer = new BYTE[nFileSize];
file.Read(pbytBuffer, nFileSize);*/

//UTF-8到GB2312的转换
/* char* U2G(const char* utf8)
{
	iconv_t cd = iconv_open("gb2312//IGNORE", "utf-8//IGNORE");   // take care of "//IGNORE", it will ignore those invalid code
	if (0 == cd)
		return NULL;
	memset(szDst, 0, iDstLen);
	char** src = &szSrc;
	char** dst = &szDst;
	if (-1 == (int)iconv(cd, src, &iSrcLen, dst, &iDstLen))
		return -1;
	iconv_close(cd);
	return 0;

	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}*/

#include <atlstr.h>
//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

void Stringsplit(const string& str, const string& splits, vector<string>& res)
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

bool DataBases::DeleteImage(int num) {
	string sql;
	string num1 = to_string(num);
	sql = "DELETE FROM Image WHERE ImageNum=";
	sql = sql + num1;
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, zSql, -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE) {
			cerr << "execution failed: " << sqlite3_errmsg(handler) << endl;
			return false;
		}
		rc = sqlite3_finalize(pStmt);
		if (rc != SQLITE_OK) {
			cerr << "destroyed_stmt failed:" << sqlite3_errmsg(handler) << endl;
			return false;
		}
		return true;
	}
}

bool DataBases::AddImage(int num, int size, const void* blobdata) {
	//插入语句
	string sql;
	string num1 = to_string(num);
	sql = "INSERT INTO Image(ImageNum,ImageData)";
	sql = sql + "VALUES(" + num1 + "?)";
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, zSql, -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		if (blobdata != NULL) {
			rc = sqlite3_bind_blob(pStmt, 1, blobdata, size, NULL);
			if (rc != SQLITE_OK) {
				cerr << "bind_blob failed:" << sqlite3_errmsg(handler) << endl;
				return false;
			}
		}
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE) {
			cerr << "execution failed: " << sqlite3_errmsg(handler) << endl;
			return false;
		}
		rc = sqlite3_finalize(pStmt);
		if (rc != SQLITE_OK) {
			cerr << "destroyed_stmt failed:" << sqlite3_errmsg(handler) << endl;
			return false;
		}
		return true;
	}
}

bool DataBases::AddData(int tablenum, string data, int size, const void* blobdata)
{
	//data  属性值1,属性值2,属性值3  若没有相关属性值请用NULL占位（头像的属性值为账号的值）
	//插入语句
	string sql;
	switch (tablenum)
	{
	case 1:
		sql = "INSERT INTO User ";
		break;
	case 2:
		sql = "INSERT INTO Cluster ";
		break;
	default:
		break;
	}
	sql = sql + "VALUES(" + data + ")";
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, G2U(zSql), -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE) {
			cerr << "execution failed: " << sqlite3_errmsg(handler) << endl;
			return false;
		}
		rc = sqlite3_finalize(pStmt);
		if (rc != SQLITE_OK) {
			cerr << "destroyed_stmt failed:" << sqlite3_errmsg(handler) << endl;
			return false;
		}
		if (blobdata != NULL) {
			vector<string> strList;
			Stringsplit(data, ",", strList);
			int num = atoi(strList[3].c_str());
			if (AddImage(num, size, blobdata))
			{
				return true;
			}
			else {
				return false;
			}
		}
		return true;
	}

}

bool DataBases::ChangeData(int tablenum, string data, string where, int size, const void* blobdata)
{

	//data 传入内容为 属性名1=属性值1，属性名2=属性值2，若要修改头像属性值仍为账号的值
	//where 传入where语句语句做筛选  例如 Account=10000000 或  Account IN  <select语句>
	//UPDATA语句
	string sql;
	switch (tablenum)
	{
	case 1:
		sql = "UPDATE User SET ";
		break;
	case 2:
		sql = "UPDATE Cluster SET ";
		break;
	default:
		break;
	}
	sql = sql + data + " WHERE " + where;
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, G2U(zSql), -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE) {
			cerr << "execution failed: " << sqlite3_errmsg(handler) << endl;
			return false;
		}
		rc = sqlite3_finalize(pStmt);
		if (rc != SQLITE_OK) {
			cerr << "destroyed_stmt failed:" << sqlite3_errmsg(handler) << endl;
			return false;
		}
		if (blobdata != NULL) {
			vector<string> strList;
			Stringsplit(data, ",", strList);
			for (int i = 0; i < strList.size(); i++) {
				if (strList[i][0] == 'P' && strList[i][1] == 'o') {
					vector<string> numList;
					Stringsplit(strList[i], "=", numList);
					int num = atoi(numList[1].c_str());
					if (!DeleteImage(num))
						return false;
					if (!AddImage(num, size, blobdata))
						return false;
					break;
				}
			}
		}
		return true;
	}
}

bool DataBases::DeleteData(int tablenum, string where) {
	//where 传入where语句语句做筛选  例如 Account=10000000 或  Account IN  <select语句>
	//UPDATA语句
	string sql;
	switch (tablenum)
	{
	case 1:
		sql = "DELETE FROM User";
		break;
	case 2:
		sql = "DELETE FROM Cluster";
		break;
	default:
		break;
	}
	sql = sql + " WHERE " + where;
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, G2U(zSql), -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE) {
			cerr << "execution failed: " << sqlite3_errmsg(handler) << endl;
			return false;
		}
		rc = sqlite3_finalize(pStmt);
		if (rc != SQLITE_OK) {
			cerr << "destroyed_stmt failed:" << sqlite3_errmsg(handler) << endl;
			return false;
		}
		return true;
	}
}

bool DataBases::SelectData(int tablenum, string where) {
	string sql;
	switch (tablenum)
	{
	case 1:
		sql = "SELECT * FROM User";
		break;
	case 2:
		sql = "SELECT * FROM Cluster";
		break;
	default:
		break;
	}
	if (where != "/0") {
		sql = sql + " WHERE " + where;
	}
	const char* zSql = sql.c_str();
	int rc = sqlite3_prepare_v2(handler, G2U(zSql), -1, &pStmt, NULL);
	int row = 0;
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		while (sqlite3_step(pStmt) == SQLITE_ROW)
		{
			row++;
		}
		sqlite3_finalize(pStmt);
	}
	rc = sqlite3_prepare_v2(handler, zSql, -1, &pStmt, NULL);
	if (rc != SQLITE_OK) {
		cerr << "prepare failed: " << sqlite3_errmsg(handler) << endl;
		return false;
	}
	else {
		if (tablenum == 1) {
			User1.clear();
			vector<User> User2(row);
			User u;
			int i=0;
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				u.Account = sqlite3_column_int(pStmt, 0);
				u.UName = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 1))));
				u.Password = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 2))));
				u.Potrait = sqlite3_column_int(pStmt, 3);
				u.LogStatus = sqlite3_column_int(pStmt, 4);
				u.Socket = sqlite3_column_int(pStmt, 5);
				u.OffLineTime = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 6))));
				User2[i] = u;
				i++;
			}
			User1 = User2;
		}
		else if (tablenum == 2)
		{
			Cluster1.clear();
			vector<Cluster> Cluster2;
			Cluster c;
			int i = 0;
			while (sqlite3_step(pStmt) == SQLITE_ROW) {
				c.CNum = sqlite3_column_int(pStmt, 0);
				c.CName = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 1))));
				c.COwner = sqlite3_column_int(pStmt, 2);
				c.CMember = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 3))));
				c.MsgRecord = std::string(U2G(reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 4))));
				Cluster2[i] = c;
				i++;
			}
			Cluster1 = Cluster2;
		}
		sqlite3_finalize(pStmt);
		return true;
	}
}