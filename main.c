#include "server.h" 
#include "thread.h" 

int main()
{
	init_server() ; 

	pthread_t tid ; 
	/* create  jyy 封装的，创建无参数线程函数
	 * bug pthread_create 失败 */
	create(listen_thread_function) ; 
	create(send_thread_function) ; 
	/* pthread_create(&tid, NULL, listen_thread_function, NULL) ; */ 
	/* pthread_create(&tid, NULL , send_thread_function , NULL) ; */ 

	return 0 ; 
} 
