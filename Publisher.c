#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT 27016

int InitializeWindowsSockets();

int __cdecl main(void)
{
    // socket used to communicate with server
    SOCKET connectSocket = INVALID_SOCKET;
    // variable used to store function return value
    int iResult;
    char topic[16];
    char messageToSend[256];

    if(InitializeWindowsSockets() == -1)
    {
		return 1;
    } 

    if (Connect())
    {
        return 1;
    }

    do{
            
        printf("Unesi topic za slanje: ");
        fgets(topic, sizeof(topic), stdin);
        printf("Unesi poruku za slanje: ");
        fgets(messageToSend, sizeof(messageToSend), stdin);
        strtok(messageToSend, "\n");

        if(strcmp("exit", messageToSend) == 0)
        {
            printf("aaaaaa");
            break;
        }

        iResult = Publish((void*)topic, (void*)messageToSend);
        if(iResult == -1)
            break;

        printf("Bytes Sent: %ld\n", iResult);

    }while(1);

    closesocket(connectSocket);
    WSACleanup();

    getch();
    
    return 0;

}




int InitializeWindowsSockets()
{
    WSADATA wsaData;
	// Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        getch();
        return -1;
    }
	return 0;
}