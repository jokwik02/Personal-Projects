#include "node.h"
#include "pdu.h"
#include <errno.h>
#include "Hashtable/hashtable.h"
#include "Hash/hash.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <unistd.h>

#define BUFFER_SIZE 25600

volatile sig_atomic_t shutdown_req = 0;
void signal_handler(int signal);

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		perror("To few arguments");
		return 0;
	}
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	state state = {
		.next = q1,
		.A = 0,
		.B = 0,
		.C = 0,
		.D = 0,
		.tracker_ip = argv[1],
		.tracker_port = atoi(argv[2])};

	while (state.next)
	{
		state.next(&state);
	}

	printf("	Shutting down node\n");
	return 0;
}

void signal_handler(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
	{
		shutdown_req = 1;
	}
}

void transfer_data(state *state, int start, int end, bool to_succ)
{
	// transfer all data
	int numb_keys;
	char **keys = ht_get_keys(state->ht, start, end, &numb_keys);

	for (int i = 0; i < numb_keys; i++)
	{
		keys[i];
		uint8_t key1[13] = {0};
		memcpy(key1, keys[i], 12);
		value_t *lookup = (value_t *)ht_lookup(state->ht, (char *)key1);

		uint8_t ssn_response[12];
		for (int j = 0; j < 12; j++)
		{
			ssn_response[j] = keys[i][j];
		}

		uint8_t name_length = strlen(lookup->name);
		uint8_t email_length = strlen(lookup->email);

		// Calculate total buffer size
		size_t total_size = 1 + SSN_LENGTH + 1 + name_length + 1 + email_length;

		// Dynamically allocate the buffer
		uint8_t *buffer = (uint8_t *)malloc(total_size);
		if (!buffer)
		{
			perror("Failed to allocate memory for PDU");
			return;
		}

		// Fill the buffer with PDU fields
		buffer[0] = VAL_INSERT;
		memcpy(buffer + 1, ssn_response, SSN_LENGTH);
		buffer[1 + SSN_LENGTH] = name_length;
		memcpy(buffer + 2 + SSN_LENGTH, lookup->name, name_length);
		buffer[2 + SSN_LENGTH + name_length] = email_length;
		memcpy(buffer + 3 + SSN_LENGTH + name_length, lookup->email, email_length);

		if (state->hash_end != 255 || to_succ)
		{
			// send to successor
			ssize_t bytes_sent = send(state->B, buffer, total_size, 0);
			if (bytes_sent < 0)
			{
				perror("Failed to send VAL_LOOKUP_RESPONSE");
				return;
			}
			printf("	Transfer %s to successor.\n", key1);
		}
		else
		{
			ssize_t bytes_sent = send(state->D, buffer, total_size, 0);
			if (bytes_sent < 0)
			{
				perror("Failed to send VAL_LOOKUP_RESPONSE");
				return;
			}
			printf("	Transfer %s to predecessor.\n", key1);
		}
		state->ht = ht_remove(state->ht, (char *)key1);
		free(buffer);
	}
	free_keys(keys, numb_keys);
	printf("	Transfer complete\n");
	return;
}


void q1(state *state)
{
	printf("[Q1]\n");

	state->A = socket(AF_INET, SOCK_DGRAM, 0);
	if (state->A == -1)
	{
		perror("Socket error");
		state->next = NULL;
		return;
	}
	state->tracker_addr.sin_family = AF_INET;
	state->tracker_addr.sin_port = htons(state->tracker_port);

	int res = inet_pton(AF_INET, state->tracker_ip, &state->tracker_addr.sin_addr);
	if (res != 1)
	{
		fprintf(stderr, "	Invalid tracker IP address or other error!\n");
		state->next = NULL;
		return;
	}

	// Bind socket to a specific port
	struct sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = INADDR_ANY;
	local_addr.sin_port = htons(0);

	if (bind(state->A, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
	{
		perror("Failed to bind socket");
		state->next = NULL;
		return;
	}

	// Print bound port for debugging
	struct sockaddr_in bound_addr;
	socklen_t addr_len = sizeof(bound_addr);
	if (getsockname(state->A, (struct sockaddr *)&bound_addr, &addr_len) == 0)
	{
		printf("	Socket bound to port %d\n", ntohs(bound_addr.sin_port));
	}
	else
	{
		perror("	getsockname failed");
	}

	// Send STUN_LOOKUP to tracker
	const struct STUN_LOOKUP_PDU message = {
		.type = STUN_LOOKUP};

	ssize_t bytes_sent = sendto(state->A, &message, sizeof(message), 0,
								(struct sockaddr *)&state->tracker_addr, sizeof(state->tracker_addr));
	if (bytes_sent < 0)
	{
		perror("Failed to send STUN_LOOKUP to tracker");
		state->next = NULL;
		return;
	}
	printf("	Node started, sending STUN_LOOKUP to tracker\n");
	state->C = socket(AF_INET, SOCK_STREAM, 0);

	state->next = q2;
}

void q2(state *state)
{
	printf("[Q2]\n");

	uint8_t buffer[5];
	socklen_t len = sizeof(state->tracker_addr);

	ssize_t bytes_received = recvfrom(state->A, buffer, 5, MSG_PEEK,
									  (struct sockaddr *)&state->tracker_addr, &len);
	if (bytes_received < 0)
	{
		perror("Failed to receive data");
		state->next = NULL;
		return;
	}

	
	state->next = q3;
}

void q3(state *state)
{
	printf("[Q3]\n");

	uint8_t buffer1[5];
	socklen_t len = sizeof(state->tracker_addr);
	ssize_t bytes_received = recvfrom(state->A, buffer1, sizeof(buffer1), 0,
									  (struct sockaddr *)&state->tracker_addr, &len);
	if (bytes_received < 0)
	{
		perror("Recv error");
		state->next = NULL;
		return;
	}

	// extract type and adress from the buffer
	uint8_t type = buffer1[0];
	uint32_t address = (buffer1[1] << 24) | (buffer1[2] << 16) | (buffer1[3] << 8) | buffer1[4];

	// save address to state
	state->address = address;
	address = ntohl(address);

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &address, ip_str, sizeof(ip_str));

	const struct NET_GET_NODE_PDU message = {
		.type = NET_GET_NODE};

	sendto(state->A, &message, sizeof(message), 0, (struct sockaddr *)&state->tracker_addr, sizeof(state->tracker_addr));

	uint8_t buffer[7];

	bytes_received = recvfrom(state->A, buffer, 7, 0,
							  (struct sockaddr *)&state->tracker_addr, sizeof(state->tracker_addr));

	struct NET_GET_NODE_RESPONSE_PDU response = {
		.type = buffer[0],
		.address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4],
		.port = (buffer[5] << 8) | buffer[6]};

	if (response.address == 0 && response.port == 0)
	{
		printf("	I am the first node to join the network\n");
		state->next = q4;
	}
	else
	{
		state->predecessor_ip = ntohl(response.address);
		state->predecessor_port = response.port;
		state->next = q7;
	}
}

void q4(state *state)
{
	printf("[Q4]\n");

	state->ht = ht_create(&free_value);
	state->hash_start = 0;
	state->hash_end = 255;

	state->next = q6;
}
void q5(state *state)
{
	printf("[Q5]\n");
	state->B = socket(AF_INET, SOCK_STREAM, 0);
	state->C = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	uint8_t buffer[14];

	ssize_t len = recvfrom(state->A, buffer, 14, 0,
						   (struct sockaddr *)&client_addr, &client_len);

	if (len < 0)
	{
		perror("Receive failed");
		return;
	}

	uint8_t type = buffer[0];

	uint32_t src_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t src_port = (buffer[5] << 8) | buffer[6];

	src_address = ntohl(src_address);
	src_port = ntohs(src_port);

	char ip_str[INET_ADDRSTRLEN];
	// Convert the IP to string
	inet_ntop(AF_INET, &src_address, ip_str, sizeof(ip_str));
	printf("	Received Type: %u, IP: %s, Port: %d\n", type, ip_str, src_port);

	struct sockaddr_in C_addr;
	socklen_t C_addr_len = sizeof(C_addr);

	C_addr.sin_family = AF_INET;
	C_addr.sin_addr.s_addr = ntohl(state->address);
	C_addr.sin_port = 0;

	if (bind(state->C, (struct sockaddr *)&C_addr, sizeof(C_addr)) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}

	if (getsockname(state->C, (struct sockaddr *)&C_addr, &C_addr_len) < 0)
	{
		perror("	getsockname failed");
		printf("	Error code: %d, Message: %s\n", errno, strerror(errno));
		return 1;
	}

	int succ_max = state->hash_end;
	state->hash_end = ((state->hash_end - state->hash_start) / 2) + state->hash_start;
	int succ_min = state->hash_end + 1;

	struct NET_JOIN_RESPONSE_PDU message2 =
		{
			.type = 4,
			.next_address = C_addr.sin_addr.s_addr,
			.next_port = C_addr.sin_port,
			.range_end = succ_max,
			.range_start = succ_min};

	printf("	Updated hash range to: [%d, %d]\n", state->hash_start, state->hash_end);
	struct sockaddr_in succ_addr;
	succ_addr.sin_family = AF_INET;
	succ_addr.sin_port = src_port;
	succ_addr.sin_addr.s_addr = src_address;
	if (connect(state->B, (struct sockaddr *)&succ_addr, sizeof(succ_addr)) < 0)
	{
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}
	printf("	Connected to %s:%d\n", ip_str, src_port);
	int flags = fcntl(state->B, F_GETFL, 0);
	fcntl(state->B, F_SETFL, flags | O_NONBLOCK);
	ssize_t bytes_sent = send(state->B, &message2, 9, 0);
	if (bytes_sent < 0)
	{
		perror("Send failed");
		exit(EXIT_FAILURE);
	}

	if (listen(state->C, 1000) < 0)
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

	if ((state->D = accept(state->C, (struct sockaddr *)&C_addr, (socklen_t *)&C_addr_len)) < 0)
	{
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}
	printf("	Accept succesfull\n");
	flags = fcntl(state->D, F_GETFL, 0);
	fcntl(state->D, F_SETFL, flags | O_NONBLOCK);

	transfer_data(state, succ_min, succ_max, true);
	state->next = q6;
}

void q6(state *state)
{
	int entries = get_num_entries(state->ht);
	printf("[Q6] (%d entries stored)\n", entries);
	if (shutdown_req)
	{
		state->next = q10;
		return;
	}
	struct NET_ALIVE_PDU alive_msg = {
		.type = NET_ALIVE};

	// send NET_ALIVE to tracker
	ssize_t bytes_sent = sendto(state->A, &alive_msg, sizeof(alive_msg), 0,
								(struct sockaddr *)&state->tracker_addr, sizeof(state->tracker_addr));
	if (bytes_sent < 0)
	{
		perror("Failed to send NET_ALIVE");
		state->next = NULL;
		return;
	}
	// printf("Sent NET_ALIVE to tracker\n");

	// poll for incoming responses
	struct pollfd poll_fd[3];
	poll_fd[0].fd = state->A;
	poll_fd[0].events = POLLIN;

	poll_fd[1].fd = state->D;
	poll_fd[1].events = POLLIN;

	poll_fd[2].fd = state->B;
	poll_fd[2].events = POLLIN;

	int poll_res = poll(poll_fd, 3, 5000);
	// printf("pol_res: %d\n", poll_res);
	if (poll_res > 0 && (poll_fd[0].revents & POLLIN))
	{
		state->sock_rec_from = A;
		// we got message
		struct sockaddr_in from_addr;
		socklen_t from_len = sizeof(from_addr);
		uint8_t buffer[BUFFER_SIZE];

		ssize_t bytes_received = recvfrom(state->A, buffer, sizeof(buffer), MSG_PEEK,
										  (struct sockaddr *)&from_addr, &from_len);
		if (bytes_received < 0)
		{
			perror("Failed to receive response");
			state->next = NULL;
			return;
		}

		char sender_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &from_addr.sin_addr, sender_ip, sizeof(sender_ip));
		printf("	Received %ld bytes from %s:%d\n", bytes_received, sender_ip, ntohs(from_addr.sin_port));

		// Handle response
		// printf("	Response type: %d\n", buffer[0]);

		switch (buffer[0])
		{
		case VAL_INSERT:
			printf("	Received VAL_INSERT\n");
			state->next = q9;
			break;

		case VAL_REMOVE:
			printf("	Received VAL_REMOVE\n");
			state->next = q9;
			break;

		case VAL_LOOKUP:
			printf("	Received VAL_LOOKUP\n");
			state->next = q9;
			break;

		case NET_JOIN:
			printf("	Received NET_JOIN\n");
			state->next = q12;
			break;

		case NET_NEW_RANGE:
			printf("	Received NET_NEW_RANGE\n");
			state->next = q15;
			break;

		case NET_LEAVING:
			printf("	Received NET_LEAVING\n");
			state->next = q16;
			break;

		case NET_CLOSE_CONNECTION:
			printf("	Received NET_CLOSE_CONNECTION\n");
			state->next = q17;
			break;

		default:
			printf("	Unknown response type: %d\n", buffer[0]);
			state->next = NULL;
			break;
		}
	}
	else if (poll_res > 0 && (poll_fd[1].revents & POLLIN))
	{
		state->sock_rec_from = D;

		uint8_t buffer[BUFFER_SIZE];

		ssize_t bytes_received = recv(state->D, buffer, sizeof(buffer), MSG_PEEK);
		printf("BYTES RECV: %d\n", bytes_received);
		printf("	D: Response type: %d\n", buffer[0]);
		switch (buffer[0])
		{
		case NET_JOIN:
			printf("	Received NET_JOIN\n");
			state->next = q12;
			break;
		case NET_CLOSE_CONNECTION:
			printf("	Received NET_CLOSE_CONNECTION\n");
			state->next = q17;
			break;
		case NET_NEW_RANGE:
			printf("	Received NET_NEW_RANGE\n");
			state->next = q15;
			break;
		case VAL_INSERT:
			printf("	Received VAL_INSERT\n");
			state->next = q9;
			break;

		case VAL_REMOVE:
			printf("	Received VAL_REMOVE\n");
			state->next = q9;
			break;

		case VAL_LOOKUP:
			printf("	Received VAL_LOOKUP\n");
			state->next = q9;
			break;
		default:
			// clear
			recv(state->D, buffer, 1, 0);
			break;
		}
	}
	else if (poll_res > 0 && (poll_fd[2].revents & POLLIN))
	{

		state->sock_rec_from = B;

		uint8_t buffer[BUFFER_SIZE];

		ssize_t bytes_received = recv(state->B, buffer, sizeof(buffer), MSG_PEEK);
		printf("BYTES RECV: %d\n", bytes_received);
		printf("	B: Response type: %d\n", buffer[0]);
		switch (buffer[0])
		{
		case NET_JOIN:
			printf("	Received NET_JOIN\n");
			state->next = q12;
			break;
		case NET_CLOSE_CONNECTION:
			printf("Received NET_CLOSE_CONNECTION\n");
			state->next = q17;
			break;
		case NET_LEAVING:
			printf("	Received NET_LEAVING\n");
			state->next = q16;
			break;
		case NET_NEW_RANGE:
			printf("	Received NET_NEW_RANGE\n");
			state->next = q15;
			break;
		case VAL_INSERT:
			printf("	Received VAL_INSERT\n");
			state->next = q9;
			break;

		case VAL_REMOVE:
			printf("	Received VAL_REMOVE\n");
			state->next = q9;
			break;

		case VAL_LOOKUP:
			printf("	Received VAL_LOOKUP\n");
			state->next = q9;
			break;
		default:
			// clear
			recv(state->B, buffer, 1, 0);
			break;
		}
	}
	else
	{
		// printf("No response or timeout\n");
	}
}

void q7(state *state)
{
	printf("[Q7]\n");
	state->B = socket(AF_INET, SOCK_STREAM, 0);
	state->C = socket(AF_INET, SOCK_STREAM, 0);

	int recieve;
	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &state->predecessor_ip, ip_str, INET_ADDRSTRLEN);
	printf("	Received IP: %s, Port: %d\n", ip_str, state->predecessor_port);

	struct sockaddr_in predecessor_addr, my_addr, receive_addr;
	socklen_t addr_len = sizeof(my_addr);
	socklen_t r_addr_len = sizeof(receive_addr);

	predecessor_addr.sin_family = AF_INET;
	predecessor_addr.sin_port = htons(state->predecessor_port);
	int res1 = inet_pton(AF_INET, ip_str, &predecessor_addr.sin_addr);
	// printf("	predecessor adress network byte order:%d\n ", predecessor_addr.sin_addr.s_addr);
	// printf("	state adress: %d\n", ntohl(state->address));

	receive_addr.sin_family = AF_INET;
	receive_addr.sin_addr.s_addr = ntohl(state->address);
	receive_addr.sin_port = 0;

	if (bind(state->C, (struct sockaddr *)&receive_addr, sizeof(receive_addr)) < 0)
	{
		perror("Bind failed");
		exit(EXIT_FAILURE);
	}
	// printf("	Bind success\n");
	getsockname(state->C, (struct sockaddr *)&receive_addr, &r_addr_len);

	const struct NET_JOIN_PDU message = {
		.src_address = receive_addr.sin_addr.s_addr,
		.src_port = receive_addr.sin_port,
		.type = NET_JOIN,
		.max_span = 0};
	// printf("	%d\n", receive_addr.sin_addr.s_addr);
	// printf("	%d\n", message.src_port);
	printf("	I am not the first node, sending NET_JOIN\n");

	int res3 = sendto(state->A, &message, 14, 0, (struct sockaddr *)&predecessor_addr, sizeof(predecessor_addr));
	// printf("	res3: %d\n", res3);
	// printf("	size: %ld\n", sizeof(message));
	if (listen(state->C, 1000) < 0)
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

	if ((state->D = accept(state->C, (struct sockaddr *)&receive_addr, (socklen_t *)&r_addr_len)) < 0)
	{
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}
	printf("	Accepted new predecessor\n");

	state->next = q8;
}
void q8(state *state)
{
	printf("[Q8]\n");
	uint8_t buffer[9];

	ssize_t bytes_received = recv(state->D, buffer, 9, 0);
	if (bytes_received < 0)
	{
		perror("Receive failed");
	}
	else if (bytes_received == 0)
	{
		printf("	Client disconnected\n");
	}
	/*for (int i = 0; i < 9; i++)
	{
		printf("	%d\n", buffer[i]);
	}*/
	uint8_t type = buffer[0];

	uint32_t succ_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t succ_port = (buffer[5] << 8) | buffer[6];

	succ_address = ntohl(succ_address);
	succ_port = ntohs(succ_port);

	char ip_str2[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &succ_address, ip_str2, sizeof(ip_str2));
	printf("	Received Type: %u, IP: %s, Port: %d\n", type, ip_str2, succ_port);

	struct sockaddr_in server_addr;
	socklen_t s_addr_len = sizeof(server_addr);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = succ_port;
	server_addr.sin_addr.s_addr = succ_address;
	if (connect(state->B, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}
	if (printf("	Connected to %s:%d\n", ip_str2, succ_port) < 0)
	{
		perror("	printf error lol");
		exit(EXIT_FAILURE);
	}
	int flags = fcntl(state->B, F_GETFL, 0);
	fcntl(state->B, F_SETFL, flags | O_NONBLOCK);
	state->ht = ht_create(&free_value);
	state->hash_start = buffer[7];
	state->hash_end = buffer[8];
	if (printf("	Hash range: [%d , %d]\n", state->hash_start, state->hash_end) < 0)
	{
		perror("	printf error lol");
		exit(EXIT_FAILURE);
	}
	fflush(stdout);
	state->next = q6;
	flags = fcntl(state->D, F_GETFL, 0);
	fcntl(state->D, F_SETFL, flags | O_NONBLOCK);
}
void q9(state *state)
{
	printf("[Q9]\n");
	ssize_t bytes_received = 0;
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	uint8_t buffer[BUFFER_SIZE];
	// read from udp
	if (state->sock_rec_from == A)
	{

		bytes_received = recvfrom(state->A, buffer, sizeof(buffer), 0,
								  (struct sockaddr *)&from_addr, &from_len);
	}
	else if (state->sock_rec_from == D)
	{
		bytes_received = recv(state->D, buffer, sizeof(buffer), MSG_PEEK);
	}
	else if (state->sock_rec_from == B)
	{
		bytes_received = recv(state->B, buffer, sizeof(buffer), MSG_PEEK);
	}
	if (bytes_received < 0)
	{
		perror("Failed to receive response");
		state->next = NULL;
		return;
	}

	switch (state->sock_rec_from)
	{
	case A:
		// printf("	A\n");
		break;

	case B:
		// printf("	B\n");
		break;

	case C:
		// printf("	C\n");
		break;

	case D:
		// printf("	D\n");
		break;

	default:
		break;
	}

	char sender_ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &from_addr.sin_addr, sender_ip, sizeof(sender_ip));
	// printf("	Received %ld bytes from %s:%d\n", bytes_received, sender_ip, ntohs(from_addr.sin_port));

	int index = 0;
	// check the type
	uint8_t type = buffer[index];
	index++;
	// get the ssn
	char ssn[13] = {0};

	for (int i = 0; i < 12; i++)
	{
		ssn[i] = buffer[index++];
	}
	// printf("	SSN: %s\n", ssn);

	// check if we are in the range
	uint8_t key1[13] = {0};
	memcpy(key1, ssn, 12);
	if (state->hash_start <= hash_ssn(key1) && state->hash_end >= hash_ssn(key1))
	{
		if (type == VAL_INSERT)
		{
			// in valid range add data to table
			value_t *value1;
			// get name lenght
			uint8_t name_length = buffer[index];
			// printf("	Index: %d\n", index);
			index++;
			// printf("	Lenght of name: %d\n", name_length);
			char *name = malloc(name_length + 1);
			for (int i = 0; i < name_length; i++)
			{
				name[i] = buffer[index];
				index++;
			}
			name[name_length] = '\0';
			// printf("	name: %s\n", name);
			uint8_t email_lenght = buffer[index];
			index++;
			char *email = malloc(email_lenght + 1);
			for (int i = 0; i < email_lenght; i++)
			{
				email[i] = buffer[index];
				index++;
			}
			email[email_lenght] = '\0';
			// printf("	email: %s\n", email);

			// print buffer
			/*for (int i = 0; i < bytes_received; i++)
			{
				printf("%d ", buffer[i]);
			}*/
			// create the value
			value1 = create_value(name, name_length, email, email_lenght);

			printf("	Inserting ssn Entry { ssn: \"%s\", name: \"%s\", email:\"%s\" } \n", key1, name, email);
			state->ht = ht_insert(state->ht, (char *)key1, (void *)value1);
			// printf("	Value inserted\n");
			free(name);
			free(email);

			int insert_length = 15 + email_lenght + name_length;
			if (state->sock_rec_from == B)
			{
				recv(state->B, buffer, insert_length, 0);
			}
			else if (state->sock_rec_from == D)
			{
				recv(state->D, buffer, insert_length, 0);
			}

			state->next = q6;
		}
		else if (type == VAL_LOOKUP)
		{
			// printf("	SSN in lookup: %s\n", ssn);

			// get the dest address
			uint32_t sender_address = (buffer[13] << 24) | (buffer[14] << 16) | (buffer[15] << 8) | buffer[16];
			uint16_t sender_port = (buffer[17] << 8) | buffer[18];

			sender_address = ntohl(sender_address);
			sender_port = ntohs(sender_port);

			// perform the lookup

			int entries = get_num_entries(state->ht);
			// printf("	NUMB ENTRIES: %d\n", entries);
			value_t *lookup = (value_t *)ht_lookup(state->ht, (char *)key1);
			// printf("	Second Lookup key: %s, hash: %d\n", key1, hash_ssn(key1));

			struct sockaddr_in response_address;
			socklen_t response_len = sizeof(response_address);

			char ip_str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &sender_address, ip_str, INET_ADDRSTRLEN);
			// printf("	Received IP: %s, Port: %d\n", ip_str, htons(sender_port));

			response_address.sin_family = AF_INET;
			response_address.sin_port = sender_port;
			int res1 = inet_pton(AF_INET, ip_str, &response_address.sin_addr);

			if (lookup != NULL)
			{
				printf("	Value found (ssn: %s).\n", key1);
				// printf("HEJ\n");
			}
			if (lookup == NULL)
			{
				// printf("	I am null\n");
				printf("	No matching entry found for SSN.\n");

				uint8_t empty_response[17] = {0};
				empty_response[0] = VAL_LOOKUP_RESPONSE;
				ssize_t bytes_sent = sendto(state->A, empty_response, 17, 0,
											(struct sockaddr *)&response_address, sizeof(response_address));
				if (bytes_sent < 0)
				{
					perror("Failed to send VAL_LOOKUP_RESPONSE");
					return;
				}
				// clear buffer
				if (state->sock_rec_from == B)
				{
					recv(state->B, buffer, bytes_received, 0);
				}
				else if (state->sock_rec_from == D)
				{
					recv(state->D, buffer, bytes_received, 0);
				}

				state->next = q6;
				return;
			}

			uint8_t ssn_response[12];
			for (int i = 0; i < 12; i++)
			{
				ssn_response[i] = ssn[i];
			}
			// send back to client

			// Construct the response PDU
			uint8_t buffer[100];
			uint8_t namelen = strlen(lookup->name);
			uint8_t maillen = strlen(lookup->email);

			buffer[0] = VAL_LOOKUP_RESPONSE;
			memcpy(buffer + 1, ssn, 12);
			buffer[13] = namelen;
			memcpy(buffer + 14, lookup->name, namelen);
			buffer[14 + namelen] = maillen;
			memcpy(buffer + 15 + namelen, lookup->email, maillen);

			// Calculate total PDU size
			uint8_t len = 15 + namelen + maillen;

			// Send the response PDU
			ssize_t bytes_sent = sendto(state->A, buffer, len, 0,
										(struct sockaddr *)&response_address, sizeof(response_address));
			if (bytes_sent < 0)
			{
				perror("Failed to send VAL_LOOKUP_RESPONSE");
				return;
			}
			// Log the sent message details
			// printf("	Successfully sent VAL_LOOKUP_RESPONSE:\n");
			// printf("	Type: %d\n", buffer[0]);
			// printf(" 	SSN: %.12s\n", buffer + 1);
			// printf(" 	Name Length: %d\n", buffer[13]);
			// printf("	Name: %.*s\n", buffer[13], buffer + 14);
			// printf("	Email Length: %d\n", buffer[14 + namelen]);
			// printf("	Email: %.*s\n", buffer[14 + namelen], buffer + 15 + namelen);

			if (state->sock_rec_from == B)
			{
				recv(state->B, buffer, bytes_received, 0);
			}
			else if (state->sock_rec_from == D)
			{
				recv(state->D, buffer, bytes_received, 0);
			}

			state->next = q6;
		}
		else if (type == VAL_REMOVE)
		{
			int entries = get_num_entries(state->ht);
			// printf("	NUMB ENTRIES: %d\n", entries);
			state->ht = ht_remove(state->ht, (char *)key1);
			if ((entries - 1) == get_num_entries(state->ht))
			{
				printf("	Removing ssn: %s\n", key1);
			}

			// printf("	NUMB ENTRIES: %d\n", entries);
			if (state->sock_rec_from == B)
			{
				recv(state->B, buffer, bytes_received, 0);
			}
			else if (state->sock_rec_from == D)
			{
				recv(state->D, buffer, bytes_received, 0);
			}
			state->next = q6;
		}
		else
		{
			state->next = q6;
		}
	}
	else
	{
		send(state->B, &buffer, bytes_received, 0);
		printf("	Not in my range, forwarding!\n");
		// we are not in range, forward
		if (state->sock_rec_from == B)
		{
			recv(state->B, buffer, bytes_received, 0);
		}
		else if (state->sock_rec_from == D)
		{
			recv(state->D, buffer, bytes_received, 0);
		}

		state->next = q6;
	}
}

void q10(state *state)
{
	printf("[Q10]\n");
	int flags = fcntl(state->B, F_GETFL, 0);
	flags &= ~O_NONBLOCK;
	fcntl(state->B, F_SETFL, flags);

	flags = fcntl(state->D, F_GETFL, 0);
	flags &= ~O_NONBLOCK;
	fcntl(state->D, F_SETFL, flags);
	if (state->D == 0)

	{
		state->next = NULL;
		printf("	I am the last node\n");
		ht_destroy(state->ht);
	}
	else
	{
		state->next = q11;
	}
}
void q11(state *state)
{
	printf("[Q11]\n");
	struct NET_NEW_RANGE_PDU message = {
		.type = NET_NEW_RANGE,
		.range_start = state->hash_start,
		.range_end = state->hash_end};
	uint8_t buffer[BUFFER_SIZE];

	printf("	Sending NET_NEW_RANGE\n");
	if (state->hash_end != 255)
	{
		send(state->B, &message, 3, 0);
		recv(state->B, buffer, 1, 0);
	}
	else
	{
		send(state->D, &message, 3, 0);
		recv(state->D, buffer, 1, 0);
	}

	state->next = q18;
}
void q12(state *state)
{
	printf("[Q12]\n");
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	uint8_t buffer[BUFFER_SIZE];
	if (state->sock_rec_from == A)
	{
		ssize_t bytes_received = recvfrom(state->A, buffer, sizeof(buffer), MSG_PEEK,
										  (struct sockaddr *)&from_addr, &from_len);
	}
	else if (state->sock_rec_from == D)
	{
		recv(state->D, buffer, sizeof(buffer), MSG_PEEK);
	}
	if (state->D == 0)
	{
		printf("	I am alone in the network\n");
		state->next = q5;
		return;
	}
	uint8_t type = buffer[0];

	uint32_t src_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t src_port = (buffer[5] << 8) | buffer[6];
	uint8_t max_span = buffer[7];
	uint32_t max_address = (buffer[8] << 24) | (buffer[9] << 16) | (buffer[10] << 8) | buffer[11];
	;
	uint16_t max_port = (buffer[12] << 8) | buffer[13];

	src_address = ntohl(src_address);
	src_port = ntohs(src_port);

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &src_address, ip_str, sizeof(ip_str));

	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getsockname(state->C, (struct sockaddr *)&addr, &addr_len);

	if (max_address == addr.sin_addr.s_addr && max_port == addr.sin_port)
	{
		printf("	I am the node with the maximim span (%d)\n", max_span);
		state->next = q13;
		return;
	}

	state->next = q14;
}
void q13(state *state)
{
	printf("[Q13]\n");
	const struct NET_CLOSE_CONNECTION_PDU message = {
		.type = NET_CLOSE_CONNECTION};

	uint8_t buffer[14];

	recv(state->D, buffer, sizeof(buffer), 0);

	uint8_t type = buffer[0];

	uint32_t src_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t src_port = (buffer[5] << 8) | buffer[6];

	src_address = ntohl(src_address);
	src_port = ntohs(src_port);

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &src_address, ip_str, sizeof(ip_str));

	struct sockaddr_in old_succ_addr;
	socklen_t old_succ_addr_len = sizeof(old_succ_addr);

	if (getpeername(state->B, (struct sockaddr *)&old_succ_addr, &old_succ_addr_len) < 0)
	{
		perror("getsockname failed");
		printf("Error code: %d, Message: %s\n", errno, strerror(errno));
		return 1;
	}

	int succ_max = state->hash_end;
	state->hash_end = ((state->hash_end - state->hash_start) / 2) + state->hash_start;
	int succ_min = state->hash_end + 1;

	struct NET_JOIN_RESPONSE_PDU message2 =
		{
			.type = 4,
			.next_address = old_succ_addr.sin_addr.s_addr,
			.next_port = old_succ_addr.sin_port,
			.range_end = succ_max,
			.range_start = succ_min};
	printf("	Sending NET_CLOSE_CONNECTION to successor\n");
	send(state->B, &message, 1, 0);
	close(state->B);
	state->B = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in succ_addr;
	succ_addr.sin_family = AF_INET;
	succ_addr.sin_port = src_port;
	succ_addr.sin_addr.s_addr = src_address;

	if (connect(state->B, (struct sockaddr *)&succ_addr, sizeof(succ_addr)) < 0)
	{
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}
	printf("	Connected to new successor %s:%d\n", ip_str, src_port);

	ssize_t bytes_sent = send(state->B, &message2, 9, 0);
	if (bytes_sent < 0)
	{
		perror("Send failed");
		exit(EXIT_FAILURE);
	}

	// transfer upper half to successor
	transfer_data(state, succ_min, succ_max, true);

	printf("	New hash-range is (%d,%d),\n", state->hash_start, state->hash_end);
	state->next = q6;
}
void q14(state *state)
{
	printf("[Q14]\n");
	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);
	uint8_t buffer[BUFFER_SIZE];

	if (state->sock_rec_from == A)
	{
		ssize_t bytes_received = recvfrom(state->A, buffer, sizeof(buffer), 0,
										  (struct sockaddr *)&from_addr, &from_len);
	}
	else if (state->sock_rec_from == D)
	{
		recv(state->D, buffer, sizeof(buffer), 0);
	}
	uint8_t type = buffer[0];

	uint32_t src_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t src_port = (buffer[5] << 8) | buffer[6];
	uint8_t max_span = buffer[7];
	uint32_t max_address = (buffer[8] << 24) | (buffer[9] << 16) | (buffer[10] << 8) | buffer[11];
	uint16_t max_port = (buffer[12] << 8) | buffer[13];

	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getsockname(state->C, (struct sockaddr *)&addr, &addr_len);

	int current_span = state->hash_end - state->hash_start;

	if (max_span < current_span)
	{
		max_span = current_span;
		max_address = addr.sin_addr.s_addr;
		max_port = addr.sin_port;
	}

	const struct NET_JOIN_PDU message = {
		.src_address = htonl(src_address),
		.src_port = htons(src_port),
		.type = NET_JOIN,
		.max_span = max_span,
		.max_address = max_address,
		.max_port = max_port};

	printf("	Forwarding to successor\n");
	send(state->B, &message, 14, 0);
	state->next = q6;
}
void q15(state *state)
{
	printf("[Q15]\n");
	uint8_t buffer[BUFFER_SIZE];
	if (state->sock_rec_from == B)
	{
		recv(state->B, buffer, sizeof(buffer), 0);
	}
	else if (state->sock_rec_from == D)
	{
		recv(state->D, buffer, sizeof(buffer), 0);
	}
	printf("	Current range is: (%d,%d)\n", state->hash_start, state->hash_end);
	if (buffer[1] < state->hash_start)
	{
		state->hash_start = buffer[1];
	}
	else
	{
		state->hash_end = buffer[2];
	}
	const struct NET_NEW_RANGE_RESPONSE_PDU message = {
		.type = NET_NEW_RANGE_RESPONSE};
	if (state->sock_rec_from == B)
	{
		send(state->B, &message, 1, 0);
	}
	else if (state->sock_rec_from == D)
	{
		send(state->D, &message, 1, 0);
	}
	printf("	New range is: (%d, %d)\n", state->hash_start, state->hash_end);

	state->next = q6;
}
void q16(state *state)
{
	printf("[Q16]\n");
	uint8_t buffer[BUFFER_SIZE];

	ssize_t bytes_received = recv(state->B, buffer, sizeof(buffer), 0);

	uint8_t type = buffer[0];

	uint32_t src_address = (buffer[1] << 24) | (buffer[2] << 16) | (buffer[3] << 8) | buffer[4];

	uint16_t src_port = (buffer[5] << 8) | buffer[6];

	src_address = ntohl(src_address);
	src_port = ntohs(src_port);

	char ip_str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &src_address, ip_str, sizeof(ip_str));

	close(state->B);

	if ((state->hash_end == 255) && (state->hash_start == 0))
	{
		printf("	I am the last node\n");
		close(state->C);
		state->C = 0;
		state->B = 0;
		state->next = q6;
		return;
	}
	printf("	Disconnected from successor\n");
	state->B = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in succ_addr;
	succ_addr.sin_family = AF_INET;
	succ_addr.sin_port = src_port;
	succ_addr.sin_addr.s_addr = src_address;

	if (connect(state->B, (struct sockaddr *)&succ_addr, sizeof(succ_addr)) < 0)
	{
		perror("Connection failed");
		exit(EXIT_FAILURE);
	}
	printf("	Connected to new successor %s:%d\n", ip_str, src_port);
	int flags = fcntl(state->B, F_GETFL, 0);
	fcntl(state->B, F_SETFL, flags | O_NONBLOCK);
	state->next = q6;
}
void q17(state *state)
{
	printf("[Q17]\n");
	uint8_t buffer[BUFFER_SIZE];

	ssize_t bytes_received = recv(state->D, buffer, sizeof(buffer), 0);
	close(state->D);
	printf("	Disconnected from predecessor\n");
	state->D = 0;
	if ((state->hash_end == 255) && (state->hash_start == 0))
	{
		state->next = q6;
		return;
	}
	if (listen(state->C, 1000) < 0)
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in receive_addr;
	socklen_t r_addr_len = sizeof(receive_addr);

	if ((state->D = accept(state->C, (struct sockaddr *)&receive_addr, (socklen_t *)&r_addr_len)) < 0)
	{
		perror("Accept failed");
		exit(EXIT_FAILURE);
	}
	printf("	Accepted new predecessor\n");

	int flags = fcntl(state->D, F_GETFL, 0);
	fcntl(state->D, F_SETFL, flags | O_NONBLOCK);
	state->next = q6;
}
void q18(state *state)
{
	printf("[Q18]\n");
	printf("	Transferring all entries\n");
	// transfer everything
	transfer_data(state, state->hash_start, state->hash_end, false);

	const struct NET_CLOSE_CONNECTION_PDU message = {
		.type = NET_CLOSE_CONNECTION};

	struct sockaddr_in succ_addr;
	socklen_t succ_addr_len = sizeof(succ_addr);

	if (getpeername(state->B, (struct sockaddr *)&succ_addr, &succ_addr_len) < 0)
	{
		perror("getpeername failed");
		exit(EXIT_FAILURE);
	}
	struct NET_LEAVING_PDU message2 = {
		.type = NET_LEAVING,
		.new_address = succ_addr.sin_addr.s_addr,
		.new_port = succ_addr.sin_port};

	printf("	Sending NET_LEAVING to predecessor\n");
	send(state->B, &message, 1, 0);
	send(state->D, &message2, 7, 0);
	close(state->B);
	close(state->A);
	close(state->C);
	close(state->D);
	ht_destroy(state->ht);
	state->next = NULL;
}
void end_state(state *state)
{
	printf("Im in end_state\n");
	state->next = NULL;
}

void free_value(void *value)
{
	value_t *val = (value_t *)value;
	free(val->name);
	free(val->email);
	free(val);
}

value_t *create_value(char *name, int name_length, char *email, uint32_t email_length)
{
	value_t *value = calloc(1, sizeof(value_t));
	if (value == NULL)
	{
		perror("calloc:");
		exit(EXIT_FAILURE);
	}

	if ((value->name = calloc(name_length, sizeof(uint8_t))) == NULL)
	{
		perror("calloc:");
		exit(EXIT_FAILURE);
	}

	memcpy(value->name, name, name_length);

	if ((value->email = calloc(email_length, sizeof(uint8_t))) == NULL)
	{
		perror("calloc:");
		exit(EXIT_FAILURE);
	}

	memcpy(value->email, email, email_length);

	return value;
}
