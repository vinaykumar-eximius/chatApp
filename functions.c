#include"header.h"
extern CLI_DETAIL *list_head;
extern int fd_server;
void *RecvThread(void *data){
	CLI_DETAIL *tdata = (CLI_DETAIL *)data;
	MSG_DATA *c_data;
	void *buff = malloc(sizeof(MSG_DATA));

	if(buff == NULL){
		printf("Error to allocating memory %s %d\n", __func__, __LINE__);
		return ;
	}
	printf("Reciving messages from [%s]\n",tdata->name);
	while(1){
		if((recv(tdata->fd_self, buff, sizeof(MSG_DATA), 0)) > 0){
			printf("Recived Msg from :%s\n", tdata->name );
			c_data = (MSG_DATA*)buff;
			printf("%s\n", c_data->data);
			if(c_data->flag == 3){ //close chat
				printf("Close request recived from: %s with %s\n", tdata->name, tdata->other_name);
				break;			
			}


			if(( send( tdata->fd_user, c_data->data, strlen(c_data->data),0)) > 0){
				printf("Message being sent from [%s] to [%s]\n", tdata->name, tdata->other_name);
			}
			else {
				perror("Failure Sending Message");
				printf("Send fail from [%s] to [%s]\n", tdata->name, tdata->other_name);
				printf("file: %s line: %d\n", __FILE__, __LINE__);
				break;
			}
		}
		else{
			perror("Error while receiving  message");
			printf("Recive fail from [%s]\n", tdata->name);
			printf("file: %s line: %d\n", __FILE__, __LINE__);
			break;
		}
	}

	if(buff != NULL)
		free(buff);
}

void *ChatThread(void *detail){
	CLI_DETAIL *tdata = (CLI_DETAIL *)detail;
	MSG_DATA *c_data;
	void *buff = malloc(sizeof(MSG_DATA));
	if(buff == NULL){
		printf("Error to allocating memory %s %d\n", __func__, __LINE__);
		return ;
	}
	int len;
	pthread_attr_t tattr;
	pthread_t tid;

	printf("[%s] ready to chat with [%s]\n", tdata->name, tdata->other_name);

	//pthread_attr_init()(&tattr);
	//pthread_attr_setdetachstate()( &tattr, PTHREAD_CREATE_DETACHED);
	//pthread_create( &tid, &tattr, RecvThread, cl);
	pthread_create( &tid, NULL, RecvThread, tdata);

	while(1){
		if((recv(tdata->fd_user, buff, sizeof(MSG_DATA), 0)) > 0){
			printf("Recived Msg from :%s\n", tdata->other_name );
			c_data = (MSG_DATA*)buff;
			printf("%s\n", c_data->data);

			if(c_data->flag == 3){ //close chat
				printf("Close request recived from: %s with %s\n", tdata->other_name, tdata->name);
				break;			
			}

			if(( send( tdata->fd_self, c_data->data, strlen(c_data->data),0)) > 0){
				printf("Message being sent from [%s] to [%s]\n", tdata->other_name, tdata->name);
			}
			else {
				perror("Failure Sending Message");
				printf("Send fail from [%s] to [%s]\n", tdata->other_name, tdata->name);
				printf("file: %s line: %d\n", __FILE__, __LINE__);
				break;
			}
		}
		else{
			perror("Error while receiving  message");
			printf("Recive fail from [%s]\n", tdata->other_name);
			printf("file: %s line: %d\n", __FILE__, __LINE__);
			break;
		}
	}
	if(buff != NULL)
		free(buff);
}


void *ConnectReq(void *req_cli)
{

	CLI_DETAIL *req_detail = (CLI_DETAIL *)req_cli;
	MSG_DATA *c_data;
	ONLINE_LIST list_on[15] = {0};
	char *token;
	pthread_t rec;
	pthread_attr_t tattr;
	void *buff = malloc(sizeof(MSG_DATA));
	if(buff == NULL){
		printf("Error to allocating memory %s %d\n", __func__, __LINE__);
		return ;
	}
	int len;
	int i;

	printf("New connection request arrived at %s.\n", __func__);
	while(1){
		printf("Proccessing on request\n");
		if((len = recv(req_detail->fd_self, buff, sizeof(MSG_DATA), 0)) > 0) {
			printf("Packet received for registration Size :%d\n", len );
			c_data = (MSG_DATA*)buff;
			printf("Registration data: %s flag: %d\n", c_data->data, c_data->flag);
		}
		else{
			perror("Error in recv()");
			printf("file: %s line: %d\n", __FILE__, __LINE__);
			break;
		}
		if(c_data->flag == 0){
			token = strtok(c_data->data, "#");
			printf("Name:%s\n", token);
			strcpy(req_detail->name, token);
			token = strtok(NULL, "\0");
			printf("Password:%s\n", token);
			strcpy(req_detail->pass, token);
		}
		else if(c_data->flag == 1){
			char str[MSG_BUFF] = {0};
			char name[NAME_SIZE] = {0};
			CLI_DETAIL *tmp = list_head;
			while(tmp != NULL){
				printf("%d  %s\n", i, tmp->name);
				sprintf(name, "%d:%s#", i, tmp->name);
				strcat(str, name);
				strcpy(list_on[i].name, name);
				list_on[i].open_fd = tmp->fd_self;
				list_on[i].o_val = i;
				tmp = tmp->next;
				i++;
			}

			printf("List to send:%s\n", str);
			strcpy(c_data->data, str);
			c_data->flag = 1;
			printf("Copied to data buffer, ready to send..\n");
			if ((send(req_detail->fd_self, (void *)c_data,  sizeof(MSG_DATA), 0)) == -1){
				perror("Failure to send user list");
				printf("Failure to send user list to %s\n",req_detail->name);
				printf("file: %s line: %d\n", __FILE__, __LINE__);
				break;
			}
			else {
				printf("Message being sent: %s\n", str);
			}

			printf("Waiting for client response\n");
			if((len = recv(req_detail->fd_self, buff, sizeof(MSG_DATA), 0)) > 0) {
				printf("Recived Success size: %d\n", len);
				c_data=(MSG_DATA*)buff;
				printf("Data: %s flag: %d\n", c_data->data, c_data->flag);
			}
			else{
				perror("Error While receiving\n");
				printf("file: %s line: %d\n", __FILE__, __LINE__);
				break;
			}
		
			int n = atoi(c_data->data);
			printf("Recived request to connect [%d] %s \n", n, list_on[n].name);
			req_detail->fd_user = list_on[n].open_fd;
			strcpy(req_detail->other_name, list_on[n].name);

			//pthread_attr_init()(&tattr);
			//pthread_attr_setdetachstate()( &tattr, PTHREAD_CREATE_DETACHED);
			//pthread_create( &rec, &tattr, recmode, req_detail);
			pthread_create( &rec, NULL, ChatThread, req_detail);
			break;
		}
		printf("Successfully added client to server.\n");
			//To Do: send reponse to thread
	}
	if(buff != NULL)
		free(buff);
	
}

void closeAll(CLI_DETAIL *list){
	printf("CloseAll Request Recived\n");
	while(list != NULL){
		if(list->fd_self){
			close(list->fd_self);
			printf("%s clossed successfully..\n", list->name);
		}
		list = list->next;
	}
#if 0	
	if(fd_server){
		close(fd_server);
		printf("Server Closed Successfully\n");
	}
#endif	
	printf("CloseAll Finished\n");
}


