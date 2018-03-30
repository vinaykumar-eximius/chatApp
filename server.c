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
	int len;
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
			break;
		}

		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_port = htons(PORT);

		socket_bind = bind( fd_server, (struct sockaddr*)&server, sizeof(server));
		if(socket_bind == -1)
		{
			perror("Error in binding");
			break;
		}
#if 0
		int yes = 1;
		if (setsockopt(socket_bind, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0)
			perror("setsockopt(SO_REUSEADDR) failed");
#endif			
		printf("Bind Done\n");
		socket_bind = listen( fd_server, 10);
		printf("Listen Done\n");
		if(socket_bind == -1)
		{
			perror("Error in listening");
			break;
		}

		len = sizeof(client);
		while(1){
			memset( &client, 0, sizeof(server));
			fd_dest = -1;
			printf("Ready to accept new connection\n");
			fd_dest = accept( fd_server, (struct sockaddr*)&client, &len);
			if(fd_dest == -1)
			{
				printf("Error in temporary socket creation");
				break;
			}
			tmp = (CLI_DETAIL *)malloc(sizeof(CLI_DETAIL));
			memset( tmp, 0, sizeof(CLI_DETAIL));
			tmp->id = id++;
			tmp->next = list_head;
			list_head = tmp;

			memcpy( &tmp->client, &client, sizeof(client));
			tmp->fd_client = fd_dest;
			tmp->fd_server = fd_server;
			//printf("fd_dest: %d  tmp->fd_client:%d  tmp:%p\n", fd_dest, tmp->fd_client, tmp);
			pthread_create( &recvt, NULL, ConnectReq, tmp);
		}

	}while(0);

	if(fd_server > 0 ){
		close(fd_server);
		printf("Server Closed Successfully\n");
	}
	return 0;
}
