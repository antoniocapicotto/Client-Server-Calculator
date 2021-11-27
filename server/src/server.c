/*
Primo esonero di Laboratorio - Reti di Calcolatori (INF) aa 20-21 di:
Antonio Capicotto 719315
Vito Luce 698859

*/
//libraries for sockets
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define NO_ERROR 0
#define closesocket close
#endif
//standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "protocol.h"
#define PROTOPORT 27015 //defualt protocol number
#define QLEN 5			//size of request queque
#define BUFFERSIZE 512
// function that prints the string given in input as an error message
void errorhandler(char *errorMessage)
{
	printf("%s", errorMessage);
}
// function that cleans the WSADATA structure used to initialize sockets in windows
void clearwinsock()
{
#ifdef WIN32
	WSACleanup();
#endif
}
//mathematical functions
void add(char str1[], char str2[], char result[])
{
	sprintf(result, "%.2f", atof(str1) + atof(str2));
}
void mult(char str1[], char str2[], char result[])
{
	sprintf(result, "%.2f", atof(str1) * atof(str2));
}
void sub(char str1[], char str2[], char result[])
{
	sprintf(result, "%.2f", atof(str1) - atof(str2));
}
void division(char str1[], char str2[], char result[])
{
	if (atof(str1) == 0 && atof(str2) == 0)
	{
		strcpy(result, "Impossible");
	}
	else if (atof(str2) == 0)
	{
		strcpy(result, "Infinity");
	}
	else
		sprintf(result, "%.2f", atof(str1) / atof(str2));
}
int main(int argc, char *argv[])
{

	int port;
	if (argc > 1)
	{
		port = atoi(argv[2]); // if argument specified convert argument to binary
	}
	else
	{
		port = PROTOPORT; // use default port number
	}

	if (port < 0)
	{
		printf("bad port number %s \n", argv[1]);
		return 0;
	}

//initializes winsock
#ifdef WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR)
	{
		errorhandler("Error at WSAStartup().\n");
		system("pause");
		return 0;
	}
#endif
	//creating the server socket
	int socketserver; //server socket descriptor
	socketserver = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socketserver < 0)
	{
		errorhandler("socket creation failed.\n");
		clearwinsock();
		system("pause");
		return -1;
	}
	struct sockaddr_in localaddress;
	memset(&localaddress, 0, sizeof(localaddress));
	localaddress.sin_family = AF_INET;
	if (argc > 1)
	{
		localaddress.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else
	{
		localaddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	localaddress.sin_port = htons(port);
	//bind address and port with socket descriptor
	if (bind(socketserver, (struct sockaddr *)&localaddress,
			 sizeof(localaddress)) < 0)
	{
		errorhandler("bind() failed.\n");
		clearwinsock();
		system("pause");
		return -1;
	}
	//server socket set to listen
	if (listen(socketserver, QLEN) < 0)
	{
		errorhandler("listen() failed.\n");
		closesocket(socketserver);
		clearwinsock();
		system("pause");
		return -1;
	}
	//accept a new connection
	struct sockaddr_in clientaddress; //client address structure
	int socketclient;				  //client socket descriptor
	int clientaddress_size;			  //size in bytes of the client address structure
	printf("Waiting for a client to connect...\n");
	int exit;

	while (1)
	{
		exit = 0;
		//2.Upon receiving the connection,server displays the client's address and port number on standard output
		clientaddress_size = sizeof(clientaddress);
		if ((socketclient = accept(socketserver,
								   (struct sockaddr *)&clientaddress, &clientaddress_size)) < 0)
		{
			errorhandler("accept() failed.\n");
			printf("client disconnected\n");
			printf("Waiting for a client to connect...\n");

			exit = 1;
		}
		if (exit == 0)
		{
			exit = 0;
			printf("Connection established with %s:%d\n",
				   inet_ntoa(clientaddress.sin_addr), port);
			//4.The server reads what is sent by the client, performs the requested operation and sends the result to the client
			int bytes_rcvd = 0;
			struct msgStruct msg;
			int bytes = (int)sizeof(struct msgStruct);
			do
			{
				bytes_rcvd = recv(socketclient, (struct msgStruct *)&msg, bytes,
								  0);
				if (bytes_rcvd <= 0)
				{
					errorhandler("recv() data failed.\n");
					printf("client disconnected\n");
					printf("Waiting for a client to connect...\n");

					exit = 1;
				}
				if (exit == 0)
				{
					exit = 0;

					char result[BUFFERSIZE];
					if (msg.operation != '=')
					{
						if (msg.operation == '+')
						{
							add(msg.value1, msg.value2, result);
						}
						else if (msg.operation == '-')
						{
							sub(msg.value1, msg.value2, result);
						}
						else if (msg.operation == '*')
						{
							mult(msg.value1, msg.value2, result);
						}
						else if (msg.operation == '/')
						{
							division(msg.value1, msg.value2, result);
						}
						int iResult = send(socketclient, (char *)&result,
										   BUFFERSIZE - 1, 0);
						if (iResult < 0)
						{
							errorhandler("send() result failed.\n");
							printf("client disconnected\n");
							printf("Waiting for a client to connect...\n");

							exit = 1;
						}
					}
				}

			} while (msg.operation != '=' && exit == 0);
		}
		if (exit == 0)
		{
			printf("client disconnected.\n");
		}
	}
	//8.The server never ends its process and must be able to accept a queue of up to 5 clients
	clearwinsock();
	closesocket(socketserver);
	system("pause");
	return 0;
}
