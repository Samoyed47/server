#include "databases.h"
#pragma comment(lib,"sqlite3.lib")
int main()
{
	DataBases d;
	//databases�г���ͼ����صĺ���������������һ��������Ϊ�������ı�1��ʾUser��2��ʾCluster
	if (!d.SelectData(1)) {//������where�������ѯȫ���ĳ�Ա    
	                       //����where����ѯ:d.SelectData(1,"Account=10000002")
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
	//Account����С�������򣬲�ѯ�������һ�е�Account��������Account������
	//Account����ѡ����NULL�������Զ�������ͷ��ı��ӦͬAcount��ͬ���������Զ�����
	//��������ΪTEXT�����ݴ��뵫û��ֵʱ����Ҫʹ��NULL����ʹ��"/0"
	if (!d.AddData(1, "null,'С��','wsxl444',10000003,0,null,'/0'"))
	//���������壺 ���ʶ��    ���������
	{
		cout << "error" << endl;
	}
	if (!d.ChangeData(1, "Password='wsxqpia'", "Account=10000003"))
	//���������壺 ���ʶ��  �޸����Ϊ��������=����ֵ   where���
	{
		cout << "error" << endl;
	}
	if (!d.DeleteData(1, "Account=10000003")) {
	//���������壺  ���ʶ��  where���
		cout << "error" << endl;
	}
	system("pause");
}