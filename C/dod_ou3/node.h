#ifndef NODE_H
#define NODE_H
#define _POSIX_C_SOURCE 200112L
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>

struct state;

typedef void state_func(struct state *s);
void free_value(void *value);

typedef struct value
{
	uint8_t *name;
	uint8_t *email;
} value_t;

enum socket{
	A,
	B,
	C,
	D
};
typedef struct state
{
	state_func *next;
	int A, B, C, D;
	enum socket sock_rec_from;
	char *tracker_ip;
	uint16_t tracker_port;
	uint32_t address;
	uint32_t successor_ip;
	uint16_t successor_port;
	uint32_t predecessor_ip;
	uint16_t predecessor_port;
	struct sockaddr_in tracker_addr;
	int hash_start, hash_end;
	struct ht *ht;
} state;

state_func q1, q2, q3, q4, q5, q6, q7, q8, q9, q10, q11, q12, q13, q14, q15, q16, q17, q18, end_state;

value_t *create_value(char *name, int name_length, char *email, uint32_t email_length);

#endif