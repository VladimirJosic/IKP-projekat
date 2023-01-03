#include "Common/Publisher.h"

int __cdecl main(void)
{
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
    
    return 0;
}