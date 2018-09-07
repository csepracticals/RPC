#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rpc_common.h"
#include <memory.h>

static char server_recv_ser_buffer[MAX_RECV_SEND_BUFF_SIZE];
static char server_send_ser_buffer[MAX_RECV_SEND_BUFF_SIZE];

int
multiply(int a , int b){

    return a * b;
}

int
server_stub_unmarshal(char *recv_client_data, int recv_client_data_size,
                      char *empty_buffer, int empty_buffer_size){

    /*Unmarshalling of Arguments is done here.
     * Reconstruct the Arguments*/

    int a , b;
    deserialize_int(&a, recv_client_data, sizeof(int));   
    deserialize_int(&b, recv_client_data + sizeof(int), sizeof(int));

    /*Step 6 : Call the Actual RPC*/

    int res = multiply(a, b);

    /*Step 7 :  Serialize the RPC result*/
    serialize_int(empty_buffer, (char *)&res, sizeof(int));

    /*Return the size of result we have serialize_intd*/
    return sizeof(int);
}

int
rpc_server_process_msg(char *recv_client_data, int recv_client_data_size, 
                       char *empty_buffer, int empty_buffer_size){


   /*Step 5 : Unmarshalling of Arguments*/

   int reply_msg_size = server_stub_unmarshal(recv_client_data, recv_client_data_size, 
                        empty_buffer, empty_buffer_size);

   return reply_msg_size; 
}

int
main(int argc, char **argv){

	int sock_udp_fd = 0, len, addr_len, opt = 1,
        reply_msg_size = 0;
         
	struct sockaddr_in server_addr,
                       client_addr;

	 if ((sock_udp_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP )) == -1)
        {

                printf("socket creation failed\n");
                exit(1);
        }

     server_addr.sin_family = AF_INET;
     server_addr.sin_port = htons(SERVER_PORT);
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

	printf("Server ready to service rpc calls\n");
READ:
    memset(server_recv_ser_buffer, 0 , MAX_RECV_SEND_BUFF_SIZE);

    /*Step 4 : Recoeve the Data from client in local buffer*/

	len = recvfrom(sock_udp_fd, server_recv_ser_buffer, MAX_RECV_SEND_BUFF_SIZE, 0,(struct sockaddr *)&client_addr, &addr_len);
    printf("No of bytes recvd from client = %d\n", len);

    /*prepare the buffer to store the reply msg to be sent to client*/
    memset(server_send_ser_buffer, 0, MAX_RECV_SEND_BUFF_SIZE);

	reply_msg_size = rpc_server_process_msg(server_recv_ser_buffer, len, /*Serialized Data which came from client, length of data */
                                            server_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE); /*Empty buffer, Maximum capacity of the buffer*/

    /*Step 8 : Send the serialize_intd result data back to client*/
	len = sendto(sock_udp_fd, server_send_ser_buffer, reply_msg_size, 0,
			(struct sockaddr *)&client_addr, sizeof(struct sockaddr));

	printf("rpc server replied with %d bytes msg\n", len);
	goto READ;
	return 0;
}
