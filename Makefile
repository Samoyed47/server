
LIB = -lsqlite3 -lpthread 
CFLAG = -g -Wall 
CC = g++ 
run : server 
	./server
server : main.o server.o 
	g++ -o server main.o server.o $(LIB) 
main.o : main.c 
	g++ -c $(CFLAG) main.c 

server.o : server.c 
	g++ -c $(CFLAG) server.c
clean : 
	rm -rf *.o server 
