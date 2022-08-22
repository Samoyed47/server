#include "server.h" 
/* #include "thread.h" */ 

int main()
{
	init_server() ; 

	pthread_t tid ; 

	pthread_create(&tid, NULL, listen_thread_function,NULL) ; 
	pthread_create(&tid, NULL ,send_thread_function , NULL) ; 

	/* (listen_thread_function)(NULL) ; */
	
	while(1)
	{
	}

	return 0 ; 
} 
