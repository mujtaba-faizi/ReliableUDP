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

int s_port = atoi(argv[3]);  //portnumber
int sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock == -1)   //error in socket
{
printf("Socket not created\n"); 
exit(0); 
}
else
printf("Socket created successfully\n");

struct sockaddr_in s_ip; 
s_ip.sin_family = AF_INET;   //Internet Protocol v4 addresses
s_ip.sin_port = htons(s_port);   //port
inet_pton(AF_INET, argv[2], &s_ip.sin_addr);   //user given ipaddress of server

FILE *f; 
f = fopen(argv[1], "rb");   //open file
char sendBuff[500];
int seq_Num = 0;    //initial squencenumber
char Buff[1];   //packet=sequence number+data
bool acks[5];   //acks (received or not)
char window[5][500];   //windowsize=5
socklen_t ip_len = sizeof(s_ip);

int u=0;
for (u=0;u<5;u++){   //initializing to no acks received(yet)
acks[u]=false;
}

while(!feof(f))   //till end of file
{
	sendBuff[0] = seq_Num;   //for sending sequence numbers to server
	fread(&sendBuff[1], sizeof(char), sizeof(sendBuff) - 1, f);   //reading file data bytes
	sendto(sock, sendBuff,sizeof(sendBuff), 0, (struct sockaddr *)&s_ip, ip_len);   //sending packets(sequence numbers + data bytes)
	strcpy(window[seq_Num], sendBuff);   //saving the bytes in their respective cells(i.e. acc. to their packet sequence numbers)
	if(recvfrom(sock, Buff, sizeof(Buff), 0, (struct sockaddr *)&s_ip, &ip_len) > 0)
	{
		acks[(int) Buff[0]] = true;     //if ack received
		memset(&Buff, 0, sizeof(Buff));
	}
	memset(&sendBuff, 0, sizeof(sendBuff));
	seq_Num++;   //next packet

	if(seq_Num == 5)  //for last packet in window
	{
		int i = 0;
		seq_Num = 0;
		for(i = 0; i < 5; i++)   //for all packets sent to server
		{
			if(acks[i] == false)   //no ack received
			{
				sleep(2);   //wait for 2 seconds
				if(recvfrom(sock, Buff, sizeof(Buff), 0, (struct sockaddr *)&s_ip, &ip_len) > 0)//if ack received after 2 sec
					{
						acks[(int) Buff[0]] = true;
						memset(&Buff, 0, sizeof(Buff));
					}
				if(acks[i] == false) //is ack still not received after 2 sec
				{
					sendto(sock, window[i], sizeof(window[i]), 0, (struct sockaddr *)&s_ip, ip_len);  //resend packet
					sleep(2);   //wait again for the ack 
					if(recvfrom(sock, Buff, sizeof(Buff), 0, (struct sockaddr *)&s_ip, &ip_len) > 0)
						{
							acks[(int) Buff[0]] = true;
							memset(&Buff, 0, sizeof(Buff));
						}	
				}
			}
			acks[i] = false;
		}
		for(i = 0; i < 5; i++)
		{
			memset(&window[i], 0, sizeof(window[i]));
		}
	}
}
printf("\nFile sent successfully\n");
close(sock);    //close socket  
}
