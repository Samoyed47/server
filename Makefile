
LIB = -lsqlite3 -lpthread 
CFLAG = -g -Wall 
CC = gcc
run : server 
	./server

server : main.o server.o 
	$(CC) -o server main.o server.o $(LIB) 

main.o : main.c 
	$(CC) -c $(CFLAG) main.c  

server.o : server.c 
	$(CC) -c $(CFLAG) server.c

# databases.o :databases.cpp
# 	g++ -c $(CFLAG) databases.cpp 
# usermanage.o : UserManage.cpp
# 	g++ -c $(CFLAG) UserManage.cpp 


clean : 
	rm -rf *.o server 
