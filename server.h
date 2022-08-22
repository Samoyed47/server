
#ifndef _SERVER_H
#define _SERVER_H 

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <arpa/inet.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <pthread.h> 
#include <assert.h> 

#define JOIN_GROUP		1 
#define CREATE_GROUP	2
#define INVITE_USER		3
#define DISSOLVE_GROUP  4
#define REGISTER_USER	5
#define SEARCH_USER		6
#define ADD_USER		7 
#define CHANGE_INFO		8 
#define LOGIN_USER		9 
#define RECV_MSG		10 
#define SEND_MSG		11 
#define LOGOUT_USER		12
#define DELETE_USER_GROUP 13
#define LISTEN_PORT 8899				// 监听端口 
#define MAX_CLIENT_COUNT 16 			// 最大连接数 
#define MAX_SEND_QUEUE_SIZE 256			//请求处理队列长度 
#define BUFFER_SIZE		2048			//消息缓冲区长度
#define handle_error(msg) \
	do{ perror(msg) ; exit(EXIT_FAILURE) ;} while(0) 

 /* 打印记录 */
#define ASNI_NODE		"\33[0m"
#define ASNI_FG_BLUE	"\33[1;34m"
#define ASNI_FMT( str, fmt) fmt str ASNI_NODE
#define _Log(...)\
	do { \
		printf(__VA_ARGS__); \
	} while(0) 
/* 输出：	[当前源文件：行数：所在函数：传入字符串] */
#define Log(format, ...)\
	_Log(ASNI_FMT("[%s:%d %s] " format, ASNI_FG_BLUE) "\n",\
			__FILE__, __LINE__, __func__, ## __VA_ARGS__) 

// storing the clients' property 
struct client_property{
	struct sockaddr_in	addr ; 
	int client_fd ;			//initial value -1 
	
	// describes the user 
	int is_online ; 
	char useraccount ; 
} ; 
struct send_property{
	char *message ; 
	int  useraccount ; 
} ;
// 初始化服务器 
void init_server() ;  
void init_socket() ;


void *listen_thread_function(void *arg) ; 
void *send_thread_function  (void *arg) ; 

void add_client(int connectfd, struct sockaddr_in addr) ; 
void *client_thread_function(void *arg) ; 
void remove_client(struct client_property *prop) ; 
void handle_client_message(struct client_property *prop, const char *message);
void join_group( const char *msg) ; 
void create_group ( const char *msg) ; 
void invite_user  (const char *msg) ; 
void dissolve_group(const char *msg) ; 
void register_user (const char *msg) ; 
void search_user   (const char *msg) ; 
void add_user	   (const char *msg) ; 
void change_info   (const char *msg) ; 
void login_user	   (const char *msg) ; 
void recv_msg	   (const char *msg) ; 
void send_msg	   (const char *msg) ; 
void logout_user   (const char *msg) ; 
void delete_user_group (const char *msg) ; 


int get_user_fd(int useraccount) ; 

void save_offline_message(struct send_property msg) ; 

#endif // _SERVER_H
