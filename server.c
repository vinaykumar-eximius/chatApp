#include"header.h"
CLI_DETAIL *list_head = NULL;
unsigned int id = 0;
int fd_server = -1;
//#if 0
void signal_handler(int sig){
	printf("Caught termination request signal %d\n", sig);	
	signal(SIGINT, SIG_DFL);
	closeAll(list_head);
}
//#endif

int main()
{
	int socket_bind;
	int len, flag = 1;
	int  fd_dest = -1;
	struct sockaddr_in server, client;
	pthread_t recvt;
	CLI_DETAIL *tmp = NULL;
//#if 0
	signal(SIGINT,signal_handler);
//#endif
	memset( &server, 0, sizeof(server));
	do{
		fd_server = socket( AF_INET, SOCK_STREAM, 0 );
		if( fd_server == -1 )
		{
			printf("Error in socket creation");
			flag = 0;
			break;
		}
		printf("Socket created successfully.\n");

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(PORT);

		socket_bind = bind( fd_server, (struct sockaddr*)&server, sizeof(server));
		if(socket_bind == -1)
		{
			perror("Error in binding");
			flag = 0;
			break;
		}
		printf("bind() success.\n");
#if 0
		int yes = 1;
		if (setsockopt(socket_bind, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
			perror("setsockopt(SO_REUSEADDR) failed");
#endif			
		socket_bind = listen( fd_server, 10);
		if(socket_bind == -1)
		{
			perror("Error in listening");
			flag = 0;
			break;
		}
		printf("listen() success.\n");

		len = sizeof(client);
		while(flag){
			memset( &client, 0, sizeof(server));
			fd_dest = -1;
			printf("Ready to accept new connection\n");
			fd_dest = accept( fd_server, (struct sockaddr*)&client, &len);
			if(fd_dest == -1)
			{
				perror("Error in socket creation for client");
				flag = 0;
				break;
			}
			tmp = (CLI_DETAIL *)malloc(sizeof(CLI_DETAIL));
			memset( tmp, 0, sizeof(CLI_DETAIL));
			tmp->id = id++;
			tmp->next = list_head;
			list_head = tmp;

			memcpy( &tmp->client, &client, sizeof(client));
			tmp->fd_self = fd_dest;
			pthread_create( &recvt, NULL, ConnectReq, tmp);
			//To Do : thread wait till request complete
			printf("Success to connect %s with server.\n", tmp->name);
		}

	}while(flag);

	if(fd_server > 0 ){
		close(fd_server);
		printf("Server Closed Successfully\n");
	}
	return 0;
}
