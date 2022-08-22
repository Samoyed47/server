#include"server.h" 
static int DEBUG = 1 ; 
int listenfd , connectfd ; 
struct sockaddr_in client, server ; 

int send_q_head, send_q_tail ; 
struct send_property send_queue[MAX_SEND_QUEUE_SIZE] ; 
//lock for the queue 
static pthread_mutex_t queue_lock ; 

struct client_property client_prop[MAX_CLIENT_COUNT] ; 

// 初始化服务器 
// initialize data structures :memssage queue  client_prop->client_fd 
// multithread lock 
// network( socket)   
void init_server()
{
	if(DEBUG) printf("init_server\n") ; 
	send_q_head = send_q_tail = 0 ;
	memset(send_queue,  0, sizeof(send_queue ) ) ; 
	memset(client_prop, 0, sizeof(client_prop) ) ; 
	for(int i=0; i < MAX_CLIENT_COUNT; i++) 
		client_prop[i].client_fd = -1; 
	
	pthread_mutex_init(&queue_lock, NULL) ;  
	init_socket() ;  
}

void init_socket()
{
	if(DEBUG) printf("init_sock\n") ;  
	if( (listenfd = socket(AF_INET,SOCK_STREAM,0) ) <0)
		handle_error("socket") ; 
	int opt = SO_REUSEADDR ; 
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt) ) ; 
	bzero( &server, sizeof(server) ) ; 
	server.sin_family	   = AF_INET ; 
	server.sin_port		   = htons(LISTEN_PORT) ; 
	server.sin_addr.s_addr = htonl(INADDR_ANY) ; 
	if (bind(listenfd, (struct sockaddr *)(&server) , sizeof(server)) <0 )
		handle_error("bind") ; 
	if (listen(listenfd ,MAX_CLIENT_COUNT) < 0) 
		handle_error("listen") ; 
}
 /* 监听线程
  *
  * */
void listen_thread_function(void *arg) 
{
	printf("in the listen thread function\n" ) ; 
	int sin_size = sizeof(struct sockaddr_in) ; 
	while(1)
	{

		if((connectfd=accept(listenfd,(struct sockaddr *)&client,
						&sin_size)) < 0) 
			handle_error("accept ()") ; 

		printf("connection from %s\n" , inet_ntoa(client.sin_addr) ) ; 
		add_client(connectfd , client) ; 
	}
} 
 /* 一个新的客户端连接到服务器
  * connectfd 链接的用户的文件描述符
  * client	  用户套接字 */
void add_client(int connectfd, struct sockaddr_in addr ) 
{
	int idx = 0 ; 
	for(idx=0; idx <MAX_CLIENT_COUNT; idx++)
	{
		if(client_prop[idx].client_fd == -1)
			break ;
	}
	assert(client_prop[idx].client_fd == -1) ; 
	
	client_prop[idx].client_fd = connectfd ; 	
	client_prop[idx].addr	   = addr ; 
	pthread_t tid ; 
	pthread_create(&tid, NULL,client_thread_function, &client_prop[idx]) ; 
	
	printf("create new thread for connect %s\n", inet_ntoa(addr.sin_addr));
}
 /* 接受用户消息的线程
  * *arg  客户端套接字指针
  */
void client_thread_function(void *arg)
{
	struct client_property *prop = (struct client_property *) arg ; 

	if(DEBUG){
		char we[] = "welcome!" ; 
	   	write(prop->client_fd, we, strlen(we) +1) ; 
	} 	
	char buf[BUFFER_SIZE] ; 
	int numbytes = 0 ;  
	while(1)
	{
		printf("recv ......\n") ; 
		numbytes = recv(prop->client_fd, buf, BUFFER_SIZE, 0) ;
		if( numbytes <=0)
		{
			printf("user %s is offline.\n",inet_ntoa(prop->addr.sin_addr));
			break ;
		} 
		buf[numbytes] = '\0' ; 
		handle_client_message(prop , buf) ; 
		
		printf(" OK\n" ) ; 
	}
	remove_client(prop) ; 
	pthread_exit(NULL) ; 
}
void remove_client(struct client_property *prop)
{
	prop->client_fd = -1 ; 

}
 /* 处理客户端发来的消息
  * prop 发送信息的客户端套接字，
  * message 客户端发送来的信息 */
void handle_client_message(struct client_property *prop, const char *message)
{
	if(DEBUG)printf("handling message from client %s,message is \n%s\n",
					inet_ntoa(prop->addr.sin_addr), message) ; 
	int typeint ; 
	char *msg ; 
	sscanf(message , "%d|%s", &typeint, msg ) ; 
	if(typeint ==JOIN_GROUP ){
		join_group( msg) ; 	
	} else if( typeint == CREATE_GROUP){
		create_group(msg) ; 
	} else if( typeint == INVITE_USER) {
		invite_user(msg) ; 
	} else if(typeint == DISSOLVE_GROUP) {
		dissolve_group(msg) ; 
	} else if(typeint == REGISTER_USER) {
		register_user(msg) ; 
	} else if(typeint == SEARCH_USER) {
		search_user(msg) ; 
	} else if(typeint == ADD_USER) {
		add_user(msg);
	} else if(typeint == CHANGE_INFO) {
		change_info(msg) ; 
	} else if(typeint == LOGIN_USER) {
		login_user(msg) ; 
	} else if (typeint == RECV_MSG) {
		recv_msg(msg) ;
	} else if(typeint == SEND_MSG) {
		send_msg(msg) ;
	} else if(typeint == LOGOUT_USER){
		logout_user(msg) ;
	} else if(typeint == DELETE_USER_GROUP){
		delete_user_group(msg) ;
	} else {
		printf("sorry ,I do't know\n") ; 
	} 	
	
}
void join_group(const char * msg)
{
}	
void create_group(const char * msg) 
{
}

void invite_user(const char * msg) 
{
}	
void dissolve_group(const char * msg) 
{
}	
void register_user(const char * msg) 
{
}	
void search_user(const char * msg) 
{
}	
void add_user(const char * msg)
{
}
void change_info(const char * msg) 
{
}	
void login_user(const char * msg) 
{
}	
void recv_msg(const char * msg) 
{
}
void send_msg(const char * msg) 
{
}
void logout_user(const char * msg) 
{
}
void delete_user_group(const char * msg) 
{
}

/* 保存离线信息
 * */
void save_offline_message(struct send_property msg)
{

}
 /* 从消息队列 send_queue[]中取出消息，发送或者存储消息
  * */
void send_thread_function (void *arg) 
{
	struct send_property msg_tmp ; 
	char   send_buffer[BUFFER_SIZE] ; 
	while(1)
	{
		pthread_mutex_lock(&queue_lock) ; 
		if(send_q_head != send_q_tail)
		{
			msg_tmp = send_queue[send_q_head] ; 
			send_q_head = (send_q_head + 1) % MAX_SEND_QUEUE_SIZE ; 
			int fd = get_user_fd(msg_tmp.useraccount); 
			
			if( fd != -1 ) {
				if(DEBUG) printf("sending message to online user %d\b",msg_tmp.useraccount) ; 
				memset(send_buffer, 0 , sizeof(send_buffer) ) ; 

				write(fd, send_buffer, sizeof(send_buffer) ) ; 
			} else{
				save_offline_message(msg_tmp) ; 
			}
			pthread_mutex_unlock(&queue_lock) ;
		}else {
			pthread_mutex_unlock(&queue_lock) ; 
			sleep(1) ;
		} 
	}


}	
 /* 获取用户文件描述符 */
 /* return client_fd
  * return -1 if error  */ 
int get_user_fd(int useraccount) 
{
	int idx=0; 
	for(idx = 0; idx <MAX_CLIENT_COUNT; idx++)
	{
		if(useraccount == client_prop[idx].useraccount )
			return client_prop[idx].client_fd ; 
	} 
	return -1 ; 
}

