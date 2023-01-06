#include "../Common/PubSub2.h"
#include <conio.h>


int main(void)
{
	SOCKET listenSocket = INVALID_SOCKET;

	int iResult;

	if (InitializeWindowsSockets() == -1)
	{
		return 1;
	}

	listenSocket = InitializeListenSocket(PUBSUB_PORT2);
	if (listenSocket == SOCKET_ERROR || listenSocket == INVALID_SOCKET)
	{
		return 1;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR)
	{
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	printf("Server initialized, waiting for clients.\n");

	while (true)
	{
		printf("Jos sam ziv\n");
		Sleep(2000);
	}

	closesocket(listenSocket);
	listenSocket = INVALID_SOCKET;
	WSACleanup();

	return 0;
}