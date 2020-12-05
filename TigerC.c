#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include <stdlib.h> 

#define MAX 80 
#define PORT 8080
#define SIZE 1040
#define SA struct sockaddr 


void func(int , char [], int);
void write_file(char file_name[], int sockfd);
void send_file(FILE *fp, int sockfd);

int main(int argc, char const *argv[])
{
	int sock = 0, valread;
	//const char *ip_address;
	struct sockaddr_in server_addr, cli;
	char command[20];
	char auth_details[100];
	char ip_address[25];
	
	fgets(auth_details, sizeof(auth_details), stdin);
	//printf("\n[+]The command is: **%s**",auth_details);
	int i=0;
	while(auth_details[i] != ' ')
	{
		//printf("%c\t",auth_details[i]);
		command[i]=auth_details[i];
		i++;
	}
	command[i]='\0';
	
	if(strcmp(command, "tconnect") !=0)
	{
		printf("\n [+]Wrong Command... \n");
		return -1;
	}
	i++;
	int j=0;
	while(auth_details[i] != ' ')
	{
		ip_address[j]=auth_details[i];
		i++;j++;
	}
	
	if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Socket Failed!");
		exit(EXIT_FAILURE);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	
	//convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, ip_address, &server_addr.sin_addr)<=0)
	{
		printf("\n Invalid Address Or Address Not supported \n");
		return -1;
	}
	
	if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("\n Connection Failed \n");
		return -1;		
	}
	int len =sizeof(auth_details);
	// function for communication with servcer
    	func(sock, auth_details,len); 
  
    	// close the socket 
    	printf("\n[+]Closing the connection...\n");
    	//close(sock); 
    	shutdown (sock, 2);
	printf("\n[+]Exiting the programs... \n");
	return 0;
		
}

void func(int sockfd, char auth_details[], int len)
{
	int auth=0;
	char buff[MAX]; 
	write(sockfd, auth_details, len);
	bzero(buff, sizeof(buff)); 
	read(sockfd, buff, sizeof(buff));
	if(strcmp(buff, "granted") == 0)
	{
		printf("[+]Auth granted...\n");
	}
	else 
	{
		printf("[+]Auth failed...\n");
		return;
	}
	char next_input[30],copy_input[30], command[30], new_file[20];
	bzero(next_input, sizeof(next_input));  
	bzero(new_file, sizeof(new_file)); 
	
	while(1)
	{ 
		printf("[+]Waiting For Command...\n");
		fgets(next_input, sizeof(next_input), stdin);
		int i=0;
		//remove spaces before command if any
		while(next_input[i]==' ')i++;
		int s=0;
		while(next_input[i]!='\n') 
		{
			copy_input[s]=next_input[i];
			s++;i++;
		}
		copy_input[s]='\n';
		strcpy(next_input, copy_input);
		
		if(next_input[0]=='e' && next_input[1]=='x' && next_input[2]=='i' && next_input[3]=='t')
		{
			write(sockfd, next_input, sizeof(next_input));
			printf("[+]Exit!...\n");
			break;
		}
		i=0;
		while(next_input[i]!=' ')
		{
			command[i] =  next_input[i];
			i++;
		}
		command[i]='\0';
		
		i++;
		int j=0;
		while(next_input[i]!='\n')
		{
			new_file[j] =  next_input[i];
			i++;j++;
		}
		new_file[j]='\0';
		char count[10]; 
		if(strcmp(command, "tget") == 0)
		{
			printf("[+]TGET command...File name is : %s\n", new_file);
			write(sockfd, next_input, sizeof(next_input));
			bzero(buff, sizeof(buff)); 
			//printf("\n[+]the file :**%s**", buff);
			write_file(new_file, sockfd);
			printf("[+]File Received...\n");
		}
		else if(strcmp(command, "tput") == 0)
		{
			printf("[+]TPUT COMMAND...File name is : %s\n", new_file);
			write(sockfd, next_input, sizeof(next_input));
			bzero(buff, sizeof(buff));
        		FILE *fp;
        		fp = fopen(new_file, "rb");
        		if (fp == NULL) 
  			{
    				perror("[-]Error in reading file.\n");
    				exit(1);
  			}
			send_file(fp, sockfd);
			fclose(fp);
			printf("[+]File Sent...\n");
		}
		else 
		{
			printf("[+]Invalid Command...\n");
			break;
		}
	}
	return;	
}

void write_file(char file_name[], int sockfd)
{
 	int n;
  	FILE *fptr;
  	char buffer[SIZE];
  	read(sockfd, buffer, sizeof(buffer));
  	if(buffer[0]=='e' && buffer[1]=='r' && buffer[2]=='r' && buffer[3]=='o' && buffer[4]=='r' && buffer[5]=='\0')
  	{
  		printf("[-]File doesn't exits...\n");
  		return;
  	}
  	fptr = fopen(file_name, "w");
  	fprintf(fptr, "%s", buffer);
  	fclose(fptr);
  	return;
}



void send_file(FILE *fp, int sockfd){
	int n;
  	char data[SIZE] = {0};
  	bzero(data, SIZE);
  	int i=0;
  	char c;
  	for (c = getc(fp); c != EOF; c = getc(fp))
  	{
  		//printf("%c->", c);
  		data[i]=c;
  		i++;
  	} 
	
	write(sockfd, data, SIZE);
	return;
}


