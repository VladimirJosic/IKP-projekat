#pragma once
#pragma once
#define _WIN32_WINNT 0x0501

#include "Queue.h"
#include "List.h"

#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

#define SAFE_DELETE_HANDLE(a) if(a){CloseHandle(a);}
#define DEFAULT_BUFLEN 512

char recvbuf[DEFAULT_BUFLEN];

CRITICAL_SECTION criticalSectionForPublisher;
HANDLE publisherSemafor, publisherFinishSemafor;

HANDLE t1;
DWORD thread1ID;

HANDLE FinishSignal;

uticnica* publisherSockets = NULL;


int InitializeWindowsSockets()
{
    WSADATA wsaData;
    // Initialize windows sockets library for this process
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup failed with error: %d\n", WSAGetLastError());
        return -1;
    }
    return 0;
}

void InitAllCriticalSections() {
    InitializeCriticalSection(&criticalSectionForPublisher);
}

void DeleteAllCriticalSections() {
    DeleteCriticalSection(&criticalSectionForPublisher);
}


void CreateSemaphores() {
    FinishSignal = CreateSemaphore(0, 0, 1, NULL);
    publisherSemafor = CreateSemaphore(0, 0, 1, NULL);
    publisherFinishSemafor = CreateSemaphore(0, 0, 1, NULL);
}

void DeleteAllThreadsAndSemaphores() {
    SAFE_DELETE_HANDLE(t1);
    SAFE_DELETE_HANDLE(FinishSignal);
    SAFE_DELETE_HANDLE(publisherSemafor);
    SAFE_DELETE_HANDLE(publisherFinishSemafor);
}

SOCKET InitializeListenSocket(const char* port) {

    SOCKET listenSocket = INVALID_SOCKET;

    struct addrinfo* resultingAddress = NULL;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4 address
    hints.ai_socktype = SOCK_STREAM; // Provide reliable data streaming
    hints.ai_protocol = IPPROTO_TCP; // Use TCP protocol
    hints.ai_flags = AI_PASSIVE;     // 


    int iResult = getaddrinfo(NULL, port, &hints, &resultingAddress);
    if (iResult != 0)
    {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        WSACleanup();
        return 1;
    }

    iResult = bind(listenSocket, resultingAddress->ai_addr, (int)resultingAddress->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(resultingAddress);
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(resultingAddress);

    //stavi u neblokirjauci rezim
    unsigned long mode = 1;
    iResult = ioctlsocket(listenSocket, FIONBIO, &mode);
    if (iResult == -1) {
        printf("ioctlsocket failed with error: %ld\n", iResult);
        return INVALID_SOCKET;
    }
    return listenSocket;
}

DWORD WINAPI Thread1(LPVOID param) {
    SOCKET listenSocketPublisher = *(SOCKET*)param;
    SOCKET acceptedSocketPublisher = INVALID_SOCKET;
    unsigned long mode = 1;

    if (InitializeWindowsSockets() == -1)
    {
        return 1;
    }

    fd_set readfds;
    struct timeval timeVal;
    timeVal.tv_sec = 1;
    timeVal.tv_usec = 0;

    while (WaitForSingleObject(FinishSignal, 500) == WAIT_TIMEOUT) {

        FD_ZERO(&readfds);
        FD_SET(listenSocketPublisher, &readfds);

        int selectResult = select(0, &readfds, NULL, NULL, &timeVal);
        if (selectResult == SOCKET_ERROR) {

            printf("LISTEN SOCKET FAILED WITH ERROR: %d\n", WSAGetLastError());
            closesocket(listenSocketPublisher);
            WSACleanup();
            return 1;

        }
        else if (selectResult == 0) {
            continue;
        }
        else {
            acceptedSocketPublisher = accept(listenSocketPublisher, NULL, NULL);
            if (acceptedSocketPublisher == INVALID_SOCKET) {

                if (WSAGetLastError() == WSAECONNRESET) {
                    printf("accept failed, beacuse timeout for client request has expired.\n");
                }

            }
            else {
                if (ioctlsocket(acceptedSocketPublisher, FIONBIO, &mode) != 0) {
                    printf("ioctlsocket failed with error.");
                    continue;
                }
                else
                {
                    printf("New publisher accepted.\n");
                
                    EnterCriticalSection(&criticalSectionForPublisher);
                    AddSocketToList(&publisherSockets, acceptedSocketPublisher);
                    printf("New publisher socket added.\n");
                    LeaveCriticalSection(&criticalSectionForPublisher);
                    ReleaseSemaphore(publisherSemafor, 1, NULL);
                }
                
            }

        }

    }

    ReleaseSemaphore(publisherFinishSemafor, 1, NULL);
    
    closesocket(listenSocketPublisher);
    WSACleanup();

    return 0;
}


void CreateThreadsEngine1(SOCKET* listenSocketPublisher) {
    t1 = CreateThread(NULL, 0, &Thread1, listenSocketPublisher, 0, &thread1ID);
}