#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

#define PORT 5555

#define NAME_SIZE 32
#define MSG_BUFF 512
#define BUFF_SIZE 128


typedef struct client_data{
	char data[MSG_BUFF];         //User name#pass
	unsigned int flag;      //0:reg, 1:list, 2:send, 3:close
}MSG_DATA ;

typedef struct client_list{
	int id;
	int fd_server;
	int fd_client;
	int fd_user;
	char name[NAME_SIZE];
	char pass[NAME_SIZE];
	char other_name[NAME_SIZE];
	struct sockaddr_in client;
	struct client_data *c_data;
	struct client_list *next;
}CLI_DETAIL;

typedef struct online_list{
	char name[NAME_SIZE];         //User name#pass
	int open_fd;      //0:reg, 1:list, 2:send, 3:close
	int o_val;
}ONLINE_LIST;

void *ChatThread(void *data);
void *ConnectReq(void *data);
void *RecvThread(void *data);
void closeAll(CLI_DETAIL *list);
