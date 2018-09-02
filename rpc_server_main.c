#include "rpc_spec.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "serialize.h"

extern ser_buff_t * rpc_server_process_msg (ser_buff_t *b);
extern void rpc_server_load_rpcs();

int
main(int argc, char **argv){

	int sock_udp_fd = 0, len, addr_len, opt = 1;
	ser_buff_t *recv_b = NULL, *out_b = NULL;

	init_serialized_buffer_of_defined_size(&recv_b, SERVER_RECV_BUFF_MAX_SIZE);

	struct sockaddr_in server_addr,
                           client_addr;

	 if ((sock_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP )) == -1)
        {

                printf("socket creation failed\n");
                exit(1);
        }

	server_addr.sin_family = AF_INET;
        server_addr.sin_port = SERVER_PORT;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        addr_len = sizeof(struct sockaddr);

	if (setsockopt(sock_udp_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(sock_udp_fd, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt))<0)
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

        if (bind(sock_udp_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
        {
            printf("socket bind failed\n");
            exit(1);
        }

	rpc_server_load_rpcs();
	printf("Server ready to service rpc calls\n");
READ:
	len = recvfrom(sock_udp_fd, (char *)recv_b->b, SERVER_RECV_BUFF_MAX_SIZE, 0,(struct sockaddr *)&client_addr, &addr_len);

	out_b = rpc_server_process_msg(recv_b);

	len = sendto(sock_udp_fd, (char *)(out_b->b), get_serialize_buffer_size(out_b), 0,
			(struct sockaddr *)&client_addr, sizeof(struct sockaddr));

	printf("rpc server replied with %d bytes msg\n", len);
	free_serialize_buffer(out_b);
	reset_serialize_buffer(recv_b);
	goto READ;

	return 0;
}
