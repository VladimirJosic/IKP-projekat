#include "Common/PubSub.h"

#define PUBSUB_PORT1 "27016"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27016"

int InitializeWindowsSockets();

int  main(void) 
{
    // Socket used for listening for new clients 
    SOCKET listenSocket = INVALID_SOCKET;
    // Socket used for communication with client
    SOCKET acceptedSocket = INVALID_SOCKET;

    int iResult;

    char recvbuf[DEFAULT_BUFLEN];

    if(InitializeWindowsSockets() == -1)
    {
		return 1;
    }

    listenSocketPublisher = InitializeListenSocket(PUBSUB_PORT1);
    if (listenSocketPublisher == SOCKET_ERROR || listenSocketPublisher == INVALID_SOCKET )
    {
        return 1;
    }

    iResult = listen(listenSocketPublisher, SOMAXCONN);
    if (iResult == SOCKET_ERROR)
    {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listenSocketPublisher);
        WSACleanup();
        return 1;
    }
    printf("Server initialized, waiting for clients.\n");

    do
    {

        acceptedSocket = accept(listenSocketPublisher, NULL, NULL);
        if (acceptedSocket == INVALID_SOCKET)
        {
            printf("accept failed with error: %d\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        do
        {
            // Receive data until the client shuts down the connection
            iResult = recv(acceptedSocket, recvbuf, DEFAULT_BUFLEN, 0);
            if (iResult > 0)
            {
                printf("Message received from client: %s\n", recvbuf);
            }
            else if (iResult == 0)
            {
                // connection was closed gracefully
                printf("Connection with client closed.\n");
                closesocket(acceptedSocket);
            }
            else
            {
                // there was an error during recv
                printf("recv failed with error: %d\n", WSAGetLastError());
                closesocket(acceptedSocket);
            }

        }while (iResult > 0);
        
        //Ovde moze logika za gasenje servera

    }while (1);

    // shutdown the connection since we're done
    iResult = shutdown(acceptedSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(acceptedSocket);
        WSACleanup();
        return 1;
    }
    closesocket(listenSocketPublisher);
    closesocket(acceptedSocket);
    WSACleanup();
    return 0;
}
