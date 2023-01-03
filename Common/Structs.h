#pragma once
#include <WinSock2.h>
#define max_topic 15

typedef struct uticnica {
	SOCKET acceptedSocket;
	struct uticnica *next;
}UTICNICA;

typedef struct poruka {
	char topic[max_topic];
	char text[256];
} PORUKA;

typedef struct queue {
	struct node *front;
	struct node *back;
} QUEUE;

typedef struct node {
	struct poruka data;
	struct node *next;
} NODE;

typedef struct subscribers {
	char topic[max_topic];
	UTICNICA * acceptedSocketsForTopic;
	struct subscribers *next;
}subscribers;