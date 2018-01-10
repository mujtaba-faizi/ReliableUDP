/*
Mujtaba Shahid FAizi
Ziyab Ikram
BSCS-5A
OS Project
*/

#include<stdio.h>
#include<stdlib.h> 
#include<string.h> 
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h> 
#include<stdbool.h>

void main(int argc, char *argv[])
{

int port = atoi(argv[2]);    //portnumber
struct sockaddr_in s_ip, c_ip;
int sock = socket(AF_INET, SOCK_DGRAM, 0);

if(sock == -1)    //error in socket
{
printf("Socket not created\n");
exit(0); 
}
else
printf("Socket created successfully\n");

s_ip.sin_family = AF_INET;   //Internet Protocol v4 addresses
s_ip.sin_addr.s_addr = htonl(INADDR_ANY);  //generate a random IPaddress
s_ip.sin_port = htons(port);    //port

if(bind(sock, (struct sockaddr*)&s_ip, sizeof(s_ip))!=0)   //binding
printf("Not Binded\n");
else
printf("Binded\n");

printf("The server is listening\n");

FILE *f; 
char Buff[500];    
char sendBuff[1];
char window[5][500];   //windowsize=5
int seq_Num = 0;    //initial sequence number
bool order[5];

socklen_t ip_len = sizeof(c_ip);
f = fopen(argv[1], "wb");   //open file

while(recvfrom(sock, Buff, sizeof(Buff), 0, (struct sockaddr *)&c_ip, &ip_len) > 0) 
{	
	strcpy(window[seq_Num], Buff);   //saving the bytes in their respective cells(i.e. acc. to their packet seq/ack numbers)
	sendBuff[0] = Buff[0];    //buffering the incoming packet sequence numbers
	sendto(sock, sendBuff, sizeof(sendBuff), 0, (struct sockaddr *)&c_ip, ip_len);   //sending ack to client
	if(seq_Num != (int) Buff[0])   //if the expected packet not received
	{
		order[seq_Num]=false;    //wrong order packet, so we wont write in file yet
		memset(&Buff, 0, sizeof(Buff));
	}
	else{
		order[seq_Num]=true;   //correct order
		fwrite(&Buff[1], sizeof(char), sizeof(Buff) - 1, f); //writing data in file
		memset(&Buff, 0, sizeof(Buff));
	}
	seq_Num++;   //next packet
	if(seq_Num == 5)   
	{
		seq_Num = 0;   //start from 0 again
		int n=0;
		for(n=0;n<5;n++){
		if (order[n]==false){   //now write the bytes buffered before in order  
			fwrite(&window[n], sizeof(char), sizeof(Buff) - 1, f); //writing previously buffered data in file
		}
		}
	}
}
fclose(f);   //close file
printf("\nFile received successfully");
}
