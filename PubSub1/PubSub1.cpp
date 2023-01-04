#include "../Common/PubSub.h"
#include <conio.h>

#define PUBSUB_PORT1 "27016"

int main(void)
{
    SOCKET listenSocketPublisher = INVALID_SOCKET;

    int iResult;

    char recvbuf[DEFAULT_BUFLEN];

    if (InitializeWindowsSockets() == -1)
    {
        return 1;
    }

    listenSocketPublisher = InitializeListenSocket(PUBSUB_PORT1);
    if (listenSocketPublisher == SOCKET_ERROR || listenSocketPublisher == INVALID_SOCKET)
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

    InitAllCriticalSections();
    CreateSemaphores();
    CreateThreadsEngine1(&listenSocketPublisher);

    if (!t1) {
        ReleaseSemaphore(FinishSignal, 1, NULL);
    }

    while (1) {

        if (_kbhit()) {
            char c = _getch();
            if (c == 'q') {
                ReleaseSemaphore(FinishSignal, 1, NULL);
                break;
            }
        }
    }

    if (t1) {
        WaitForSingleObject(t1, INFINITE);
    }

    DeleteAllCriticalSections();
    DeleteAllThreadsAndSemaphores();



    ZatvoriSveSocketeZaListu(publisherSockets);

    deleteList(&publisherSockets);

    closesocket(listenSocketPublisher);
    listenSocketPublisher = INVALID_SOCKET;
    WSACleanup();

    return 0;

}
