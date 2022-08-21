#include "databases.h"
#pragma comment(lib,"sqlite3.lib")
int main()
{
	DataBases d;
	//databases中除与图像相关的函数，其他函数的一个参数均为所操作的表，1表示User，2表示Cluster
	if (!d.SelectData(1)) {//不输入where语句代表查询全部的成员    
	                       //输入where语句查询:d.SelectData(1,"Account=10000002")
		cout << "error" << endl;
	}
	else {
		for (int i = 0; i < d.User1.size(); i++) {
			cout << "Account:" << d.User1[i].Account;
			cout << " UName:" << d.User1[i].UName;
			cout << " Password:" << d.User1[i].Password;
			cout << " Potrait:" << d.User1[i].Potrait;
			cout << " LogStatus:" << d.User1[i].LogStatus;
			cout << " Socket:" << d.User1[i].Socket;
			cout << " OffLineTime:" << d.User1[i].OffLineTime;
			cout << endl;
		}
	}
	//Account按从小到大排序，查询到的最后一行的Account就是所有Account中最大的
	//Account可以选择填NULL，它会自动递增，头像的编号应同Acount相同，但不会自动递增
	//数据类型为TEXT的数据传入但没有值时，不要使用NULL而是使用"/0"
	if (!d.AddData(1, "null,'小李','wsxl444',10000003,0,null,'/0'"))
	//各参数含义： 表标识符    插入的数据
	{
		cout << "error" << endl;
	}
	if (!d.ChangeData(1, "Password='wsxqpia'", "Account=10000003"))
	//各参数含义： 表标识符  修改语句为：属性名=属性值   where语句
	{
		cout << "error" << endl;
	}
	if (!d.DeleteData(1, "Account=10000003")) {
	//各参数含义：  表标识符  where语句
		cout << "error" << endl;
	}
	system("pause");
}