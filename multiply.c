#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "rpc_common.h"

#define MAX_RECV_SEND_BUFF_SIZE 32

static char client_recv_ser_buffer[MAX_RECV_SEND_BUFF_SIZE];
static char client_send_ser_buffer[MAX_RECV_SEND_BUFF_SIZE];

int
rpc_send_recv (char *ser_data, int ser_data_size, char *server_reply_msg){

    struct sockaddr_in dest;
    int sockfd = 0, rc = 0, recv_size = 0;
    int addr_len;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(SERVER_PORT);
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP);
    dest.sin_addr = *((struct in_addr *)host->h_addr);
    addr_len = sizeof(struct sockaddr);

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
        printf("socket creation failed\n");
        return -1;
    }

    rc = sendto(sockfd, ser_data, ser_data_size, 0,
            (struct sockaddr *)&dest, sizeof(struct sockaddr));

    printf("%s() : %d bytes sent\n", __FUNCTION__, rc);

    recv_size = recvfrom(sockfd, server_reply_msg, MAX_RECV_SEND_BUFF_SIZE, 0,
            (struct sockaddr *)&dest, &addr_len);

    printf("%s() : %d bytes recieved\n", __FUNCTION__, recv_size);

    return recv_size;
}

/* client_stub_marshal()*/
char *
client_stub_marshal(int a, int b, int *size){

    memset(client_send_ser_buffer, 0, 32);

    /*Serialize the first Argument*/
    serialize_int(client_send_ser_buffer, (char *)&a, sizeof(int));
    /*Serialize the second Argument*/
    serialize_int(client_send_ser_buffer + sizeof(int), (char *)&b, sizeof(int));

    *size = sizeof(int) * 2;

    /*Serialize buffer looks like this 
     *
     *  +---------+---------+
     *  |         |         |
     *  |   Arg1  |   Arg2  |
     *  +---------+---------+
     *  <----4----><----4--->
     * */
    return client_send_ser_buffer;
}

int client_stub_unmarshal(char *ser_data, int size){

    /*Reconstruct the result obtained from Server*/
    int res = 0;
    deserialize_int(&res, ser_data, sizeof(int));
    return res;
}

int
multiply_rpc(int a , int b){

    /*Step 2 : Serialize/Marshal the arguments*/
    int size = 0;

    /*Signature : char * () <Arg1, Arg2, . . . , int *> */
    char *ser_data = client_stub_marshal(a,b, &size);

    /*Prepare for ecieving the data from Server*/
    memset(client_recv_ser_buffer, 0, 32);

    /*Step 3 : Send the serialized data to the server,
     * and wait for the reply*/

    /*Fn that will work for all RPCs
     * Signature : int () <char *, int size, char *> */
    int recv_size = 
        rpc_send_recv(ser_data, size, client_recv_ser_buffer);

    /*Step 4 , 5, 6 , 7, 8 are executed on Server side*/

    /*Step 9 : Unmarshal the serialized data (result) recvd from Server,
     * and reconsctruct the RPC return type
     * Signature : <rpc return type> () <char *, int>
     * */
    
    int res = client_stub_unmarshal(client_recv_ser_buffer, recv_size);
    return res;
}

int
main(int argc, char **argv){

    int a, b;
    printf("Enter Ist number : ");
    scanf("%d", &a);
    printf("Enter IInd number : ");
    scanf("%d", &b);

    /*Step 1 : Invoke the rpc*/
    int res = multiply_rpc(a, b);

    /*Step 7 : Print the RPC result*/
    printf("res = %d\n", res);
    return 0; 
}
