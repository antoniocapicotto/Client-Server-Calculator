//libraries for sockets
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket close
#define NO_ERROR 0
#endif
//standard libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"
#define PROTOPORT 27015 //default protocol port number
#define BUFFERSIZE 512	//default buffer size
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
int main(int argc, char *argv[])
{
//initializes winsock
#ifdef WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (result != NO_ERROR)
	{
		errorhandler("Error at WSAStartup().\n");
		system("pause");
		return -1;
	}
#endif
	//creation of the client socket
	int socketclient; //client socket descriptor
	socketclient = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serveraddress;
	memset(&serveraddress, 0, sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	if (socketclient < 0)
	{
		errorhandler("socket creation failed.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
		return -1;
	}
	if (argc > 1)
	{
		serveraddress.sin_port = htons(atoi(argv[2]));
		serveraddress.sin_addr.s_addr = inet_addr(argv[1]);
	}
	else
	{
		//1.client reads the server address and port number from the command line
		char choose[BUFFERSIZE];
		char ind[32];
		int port;
		do
		{
			printf("Want to use an ip address and port? 1 for yes, 0 for no: ");
			fflush(stdin);
			fgets(choose, sizeof(choose), stdin);
		} while (!(choose[0] == '0' || choose[0] == '1') || !(choose[1] == '\n'));

		if (choose[0] == '1')
		{
			printf("Enter address:");
			fflush(stdin);
			fgets(ind, sizeof(ind), stdin);
			fflush(stdin);
			printf("Insert port:");
			fflush(stdin);
			scanf("%d", &port);
			getchar();
			serveraddress.sin_addr.s_addr = inet_addr(ind);
			serveraddress.sin_port = htons(port);
		}
		else
		{
			serveraddress.sin_addr.s_addr = inet_addr("127.0.0.1");
			serveraddress.sin_port = htons(27015);
		}
	}
	//client requests connection to the server
	if (connect(socketclient, (struct sockaddr *)&serveraddress,
				(int)sizeof(serveraddress)) < 0)
	{
		errorhandler("failed to connect.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
		return -1;
	}
	//3.Once the connection is established, the client reads the operation to be performed
	char str[BUFFERSIZE];
	struct msgStruct msg;
	int stop, flag0, flag1, flag2, i, j, l, k;

	msg.operation = ' ';
	while (msg.operation != '=')
	{
		msg.operation = '=';
		do
		{
			//6.The client reads the next operation to be performed from the standard input.
			flag0 = 0;
			flag1 = 0;
			flag2 = 0;
			stop = 0;
			printf("Insert operation symbol +, -, *, / and the 2 numbers. ex: '+ 23 45[Enter]' \n");
			printf("Enter = symbol to close the program \n");
			printf("Please enter operation : ");

			fflush(stdin);
			fgets(str, sizeof(str), stdin);
			printf("\n");
			if (str[0] != '=')
			{
				if (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/')
				{
					msg.operation = str[0];
					i = 1;
					while (str[i] == 32)
					{
						i++;
					}
					j = i;
					while (str[j] != 32)
					{
						j++;
					}
					k = 0;
					while (i < j)
					{
						msg.value1[k] = str[i];
						k++;
						i++;
					}
					msg.value1[k] = '\0';

					l = 0;
					flag1 = 0;
					while (l < k && flag1 == 0)
					{
						if (!(msg.value1[l] == '0' || msg.value1[l] == '1' || msg.value1[l] == '2' || msg.value1[l] == '3' || msg.value1[l] == '4' || msg.value1[l] == '5' || msg.value1[l] == '6' || msg.value1[l] == '7' || msg.value1[l] == '8' || msg.value1[l] == '9'))
						{
							flag1 = 1;
							printf("WRITTEN REQUEST BAD: PLEASE CHECK AND CORRECT YOUR REQUEST\n");
							printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n\n");
						}
						l++;
					}
					if (flag1 == 0)
					{
						while (str[i] == 32)
						{
							i++;
						}
						k = 0;
						j = strlen(str);
						while (i < j)
						{
							msg.value2[k] = str[i];
							k++;
							i++;
						}
						msg.value2[k] = '\0';
						l = 0;
						flag2 = 0;
						while (l < k && flag2 == 0)
						{
							if (!(msg.value2[l] == '0' || msg.value2[l] == '1' || msg.value2[l] == '2' || msg.value2[l] == '3' || msg.value2[l] == '4' || msg.value2[l] == '5' || msg.value2[l] == '6' || msg.value2[l] == '7' || msg.value2[l] == '8' || msg.value2[l] == '9' || msg.value2[l] == '\n'))
							{
								flag2 = 1;
								printf("WRITTEN REQUEST BAD: PLEASE CHECK AND CORRECT YOUR REQUEST\n");
								printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n\n");
							}
							l++;
						}
					}
				}
				else
				{
					flag0 = 1;
					printf("WRITTEN REQUEST BAD: PLEASE CHECK AND CORRECT YOUR REQUEST\n");
					printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ \n\n");
				}
			}
			else
			{
				stop = 1;
			}
		} while (flag0 == 1 || flag1 == 1 || flag2 == 1);

		//client sends what is read from the keyboard to the server
		int bytes = (int)sizeof(struct msgStruct);
		if (send(socketclient, (struct msgStruct *)&msg, bytes, 0) < 0)
		{
			errorhandler("send() data failed.\n");
			closesocket(socketclient);
			clearwinsock();
			system("pause");
			return -1;
		}
		//5.The client reads the response sent by the server and displays it on standard output.
		if (stop != 1)
		{
			char resultvalue[BUFFERSIZE];
			int iResult = recv(socketclient, (char *)&resultvalue,
							   BUFFERSIZE - 1, 0);
			if (iResult <= 0)
			{
				errorhandler("recv() string failed.\n");
				closesocket(socketclient);
				clearwinsock();
				system("pause");
				return -1;
			}
			else
			{
				if (msg.operation != '=')
				{
					printf("\n");
					printf("Result : %s\n\n", resultvalue);
				}
			}
		}
	}
	//7.If instead of an operation the '=' character is inserted
	/*
	 * the client closes the connection with the server and ends its process here;
	 * otherwise,
	 * go back to step 3.
	 */
	msg.operation = '=';
	int bytes = (int)sizeof(struct msgStruct);
	if (send(socketclient, (struct msgStruct *)&msg, bytes, 0) < 0)
	{
		errorhandler("send() data failed.\n");
		closesocket(socketclient);
		clearwinsock();
		system("pause");
		return -1;
	}
	//close connection
	printf("Close connection.\n");
	closesocket(socketclient);
	clearwinsock();
	system("pause");
	return 0;
}
