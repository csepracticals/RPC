#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory.h>
#include "rpc_common.h"
#include "De_Serialization/serialize.h"

int
multiply(int a , int b){

    return a * b;
}

int
multiply_server_stub_unmarshal(ser_buff_t *server_recv_ser_buffer){

    int a , b;
    de_serialize_data((char *)&a, server_recv_ser_buffer, sizeof(int));   
    de_serialize_data((char *)&b, server_recv_ser_buffer, sizeof(int));

    /*Step 6 : Call the Actual RPC and return its result*/
    return multiply(a, b);
}

void
multiply_server_stub_marshal(int res, ser_buff_t *server_send_ser_buffer){

    serialize_data(server_send_ser_buffer, (char *)&res, sizeof(int));
}

void
rpc_server_process_msg(ser_buff_t *server_recv_ser_buffer, 
                       ser_buff_t *server_send_ser_buffer){

   /*  Step 5 */
   /*  Signature : <rpc return type> server_stub_unmarshal (ser_buff_t *ser_data)
    *  Unmarshalling of Arguments*/
    /* Unmarshalling of Arguments is done here. Reconstruct the Arguments*/

   int res = multiply_server_stub_unmarshal(server_recv_ser_buffer); 

   /*Step 7 : Now we have got the RPC result, time to serialize/Marshall the result*/
   multiply_server_stub_marshal(res, server_send_ser_buffer);
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

    /*Prepare Server Memory buffers to send and Recieve serialized Data*/

    ser_buff_t *server_recv_ser_buffer = NULL,
               *server_send_ser_buffer = NULL;

    init_serialized_buffer_of_defined_size(&server_recv_ser_buffer, 
                                MAX_RECV_SEND_BUFF_SIZE);

    init_serialized_buffer_of_defined_size(&server_send_ser_buffer, 
                                MAX_RECV_SEND_BUFF_SIZE);

	printf("Server ready to service rpc calls\n");
READ:
    reset_serialize_buffer(server_recv_ser_buffer);

    /*Step 4 : Recieve the Data from client in local buffer*/

	len = recvfrom(sock_udp_fd, server_recv_ser_buffer->b, 
                   get_serialize_buffer_length(server_recv_ser_buffer),
                   0,(struct sockaddr *)&client_addr, &addr_len);
    printf("No of bytes recvd from client = %d\n", len);

    /*prepare the buffer to store the reply msg to be sent to client*/
    reset_serialize_buffer(server_send_ser_buffer);

	rpc_server_process_msg(server_recv_ser_buffer, /*Serialized Data which came from client*/
                           server_send_ser_buffer); /*Empty serialized buffer*/

    /*Step 8 : Send the serialized result data back to client*/
	len = sendto(sock_udp_fd, server_send_ser_buffer->b, 
                 get_serialize_buffer_data_size(server_send_ser_buffer),
                 0, (struct sockaddr *)&client_addr, sizeof(struct sockaddr));

	printf("rpc server replied with %d bytes msg\n", len);
	goto READ;
	return 0;
}
