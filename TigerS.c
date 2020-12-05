#include<stdio.h>
#include<unistd.h>
#include<stdlib.h> 
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<string.h>

#define MAX 100 
#define PORT 8080
#define SIZE 1040
#define SA struct sockaddr 

void write_file(char file_name[], int sockfd);
void send_file(FILE *fp, int sockfd, int exists);
int check_username_password(char entered_username[], char entered_password[]);

void func(int sockfd)
{
	char buff[MAX]; 
	char command[20];
	char auth_details[100];
	char conn_details[100];
	char ip_address[25];
    	int n, auth;
    	char username[20], password[20];
    	
    	//read client's commands 
    	read(sockfd, buff, sizeof(buff));

        int i=0;
	while(buff[i] != ' ')
	{

		command[i]=buff[i];
		i++;
	}
	command[i]='\0';
	
	if(strcmp(command, "tconnect") !=0)
	{
		printf("\n [+]Wrong Command... \n");
		exit;
	}
	i++;
	int j=0;
	while(buff[i] != ' ')
	{
		ip_address[j]=buff[i];
		i++;j++;
	}
	ip_address[j] ='\0';
	j=0;
	i++;
	while(buff[i] != ' ')
	{
		username[j]=buff[i];
		i++;j++;
	}
	i++;
	username[j]='\0';
	j=0;
	while(buff[i] != '\n')
	{
		password[j]=buff[i];
		i++;j++;
	}
	password[j]='\0';	
	//printf("\n the data is : **%s**",buff);
	
	//Auth user
	auth = check_username_password(username, password);
	
	if(auth == 1)
	{
		printf("\n[+]Auth Granted to: %s",username);
		write(sockfd, "granted", strlen("granted"));
		//read clients's next commands
		while(1)
		{
			bzero(buff, sizeof(buff));  
    			read(sockfd, buff, sizeof(buff));
    			char next_input[30], next_command[30], file_name[30];
    			i=0;
    			if(buff[0]=='e' && buff[1]=='x' && buff[2]=='i' && buff[3]=='t')
			{
				printf("[+]Exit!...\n");
				break;
			}
			while(buff[i]!=' ')
			{
				next_command[i] =  buff[i];
				i++;
			}
			next_command[i]='\0';
			//printf("the command is : **%s**\n", next_command);
			i++;
			j=0;
			if(strcmp(command, "exit\n") == 0)
			{
				printf("[+]Exit....\n");
				break;
			}
			while(buff[i]!='\n')
			{
				file_name[j] =  buff[i];
				i++;j++;
			}
			file_name[j]='\0';
			if(strcmp(next_command, "tget") == 0)
			{
				printf("\n[+]TGET command...File name is : %s\n", file_name);
				FILE *fp;
				if( access( file_name, F_OK ) != -1 ) 
				{
					// file exists
					//printf("[+]In Access File exists...");
    					fp = fopen(file_name, "r");
    					if (fp == NULL) 
  					{
    						perror("[-]Error in reading file.");
    						exit(1);
  					}
  					send_file(fp, sockfd, 1);
					fclose(fp);
					printf("\n[+]File Sent...");
				} 
				else 
				{
    					// file doesn't exist
    					send_file(fp, sockfd, -1);
    					perror("\n[-]");
    					//exit(1);
				}		
			}
			else if(strcmp(next_command, "tput") == 0)
			{
				printf("[+]TPUT COMMAND...File name is : %s\n", file_name);
				write_file(file_name,sockfd);
				printf("\n[+]File Received...\n");
			}
			else 
			{
				printf("[+]Invalid Command...\n");
				return;
			}
		}
	}
	else 
	{
		printf("\n[+]Auth Failed for: %s\n",username);
		write(sockfd, "failed", strlen("failed"));
		return;
	}
		
  	printf("[+]Closing the connection...\n");
  	return;
}


int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread, conn,len;
	struct sockaddr_in address, cli;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[1024] = {0};
	char message[100] = "";
	
	//creating socket file descriptor
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0))== 0)
	{
		perror("Socket Failed!");
		exit(EXIT_FAILURE);		
	}
	
	//forcefully attach socket to port 8080
	if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );
	
	//forcefully attaching socket to port port 8080
	if(bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
	{
		perror("Bind Failed!");
		exit(EXIT_FAILURE);
	}	
	
	if(listen(server_fd, 100) <0)
	{
		perror("Listen!");
		exit(EXIT_FAILURE);	
	}
	printf("[+]Listening....\n");
	len = sizeof(cli); 
	// Accept the data packet from client and verification
	//conn = accept(server_fd, (SA*)&cli, &len);
    	 
	while(conn = accept(server_fd, (SA*)&cli, &len))
	{
		int pid = 0;
		if((pid = fork()) == 0)
		{
			func(conn); 
			printf("\n[+]Client Dissconnected....");
			exit(0);
		}
	}
	
	close(server_fd); 
	return 0;
}	

void write_file(char file_name[], int sockfd){
 	int n;
  	FILE *fp;
  	char buffer[SIZE];

  	fp = fopen(file_name, "w");
  	if (fp == NULL) 
  	{
    		perror("[-]Error in reading file...\n");
    		exit(1);
  	}
  	read(sockfd, buffer, sizeof(buffer));
  	//printf("\n the data is : %s", buffer);
  	fprintf(fp, "%s", buffer);
  	fclose(fp);
  	return;
}

void send_file(FILE *fp, int sockfd, int exists){
	int n;
  	char data[SIZE] = {0};
  	bzero(data, SIZE);
  	if(exists==-1)
  	{
  		data[0]='e';
  		data[1]='r';
  		data[2]='r';
  		data[3]='o';
  		data[4]='r';
  		data[5]='\0';
  		
  	}
  	else 
  	{
  		int i=0; 
  		char c;
  		for (c = getc(fp); c != EOF; c = getc(fp))
  		{
  			//printf("%c->", c);
  			data[i]=c;
  			i++;
  		} 
  	}
        write(sockfd, data, SIZE);		
  	//printf("\n[+]File Sent...exit send_file, data is : %s", data);
  	return;
}


int check_username_password(char entered_username[], char entered_password[])
{
	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
    	FILE * fp_auth;
    	fp_auth = fopen("users.txt", "r");
    	//printf("\n[+]in auth username : **%s** password : **%s**",entered_username,entered_password);
    	if (fp_auth == NULL)
        	exit(EXIT_FAILURE);
	while ((read = getline(&line, &len, fp_auth)) != -1) 
	{
        	char *user = strtok(line, " ");
        	char *password = strtok (NULL, "\n");
        	
        	//printf("\n[+]in while auth username : **%s** password : **%s**",user,password);
       
    		int resUser = strcmp(entered_username, user);
 		
    		int resPassword = strcmp(entered_password, password);
    
    		if(resUser == 0 && resPassword == 0)
    		{
    			printf("\n[+]User Authentication Successfull...\n");
    			fclose(fp_auth);
    			return 1;
    		}
    	}
    	printf("\n[+]User Authentication Failed...\n");
    	fclose(fp_auth);	
    	return -1;
}

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
