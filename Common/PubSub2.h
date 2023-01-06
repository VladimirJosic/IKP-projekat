#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Connection.h"

#pragma comment(lib, "ws2_32.lib")

#define SAFE_DELETE_HANDLE(a) if(a){CloseHandle(a);}
#define DEFAULT_BUFLEN 512

#define PUBSUB_PORT2 "27017"

char recvbuf[DEFAULT_BUFLEN];

