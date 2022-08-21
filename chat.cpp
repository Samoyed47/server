#include<iostream>
#include<stdlib>
#include<string>
#include<errno.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<pthread.h>

void *threadFun(void *arg)
{
	int confd = *(int*)arg;
	char buf[1024];
	
	while(1)
	{		
		memset(&buf,0,sizeof(buf));
		if(recv(confd,buf,sizeof(buf),0) == 0)
		{
			pthread_exit(NULL);
		}
		
		string ans;//返回客户端的字符串
		string n,Buf;//前缀，传入函数的参数
		sscanf(&buf,"%s|%s",&n,&Buf);
		
		if(n >= "1" && n <= "4")
		{
			//建立群组对象
		}
		else if(n >= "5" && n <= "13")
		{
			//建立用户对象
			UserManage U;
		}
		
		switch(n){
			case "5":
				ans = U.Register(Buf);
				break;
			case "6":
				ans = U.SearchUser(Buf);
				break;
			case "7":
				if(Buf[0] == 'a' || Buf[0] == 'r')//如果是accept/reject开头
				{
					ans = U.AddFriend2(Buf);
				}
				else
				{
					ans = U.AddFriend1(Buf);
				}
				break;
			case "8":
				ans = U.ChangeData(Buf);
				break;
			case "9":
				ans = U.Logging(Buf);
				break;
			case "10":
				string d1,d2,d3,d4,d5;
				sscanf(&Buf,"%s|%s|%s|%s|%s",&d1,&d2,&d3,&d4,&d5);
				string RBuf = d4 + "|" + d1 + "|" + "T" + "|" + d5;
				ans = U.Receive(RBuf);
				break;
			case "11":
				
				break;
			case "12":
				ans = U.SelecteAccount(Buf);
				break;
			case "13":
				ans = U.SelecteGroup(Buf);
				break;
		}
		
		write(confd,ans,strlen(ans));//将ans返回给客户端
	}
}


int main()
{
	pthread_t tid;
	int clientfd[100];
	memset(&clientfd,0,sizeof(clientfd));
	int size = 50;//可以连接的客户端数量，需要比clientfd的长度小
	char buf[1024] = {0};
	struct sockaddr_in myaddr;
	memset(&myaddr,0,sizeof(myaddr));

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(8899);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int lisfd = socket(AF_INET,SOCK_STREAM,0);

	if(lisfd == -1)
	{
		perror("socket");
		return -1;
	}

	int vel = 1;
	setsockopt(lisfd,SOL_SOCKET,SO_REUSEADDR,(void *)&vel,sizeof(vel));

	if(bind(lisfd,(struct sockaddr *)&myaddr,sizeof(myaddr)) == -1)
	{
		perror("bind");
		return -1;
	}
	
	if(listen(lisfd,100) == -1)
	{
		perror("listen");
		return -1;
	}

	while(1)
	{
		
		int confd = accept(lisfd,NULL,NULL);

		if(confd == -1)
		{
			perror("accept");
			return -1;
		}

		int i = 0;
		for(i = 0; i < size; i++)
		{
			if(clientfd[i] == 0)
			{
				clientfd[i] = confd;
				//pthread_t tid;
				pthread_create(&tid,NULL,threadFun,&confd);
				break;
			}
		}

	}
}