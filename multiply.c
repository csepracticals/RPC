#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "rpc_common.h"
#include "De_Serialization/serialize.h"

void
rpc_send_recv (ser_buff_t *client_send_ser_buffer,
        ser_buff_t *client_recv_ser_buffer){
}


/* client_stub_marshal()*/
ser_buff_t *
multiply_client_stub_marshal(int a, int b){

    ser_buff_t *client_send_ser_buffer = NULL;
    init_serialized_buffer_of_defined_size(&client_send_ser_buffer, MAX_RECV_SEND_BUFF_SIZE);

    /*Serialize the first Argument*/
    serialize_data(client_send_ser_buffer, (char *)&a, sizeof(int));
    /*Serialize the second Argument*/
    serialize_data(client_send_ser_buffer, (char *)&b, sizeof(int));

    /*Serialize buffer looks like this
     * 
     * +---------+---------+
     * |         |         |
     * |   Arg1  |   Arg2  |
     * +---------+---------+
     * <----4----><----4--->
     * */
    return client_send_ser_buffer;
}


int
multiply_rpc(int a , int b){

    /*Step 2 : Serialize/Marshal the arguments*/
    /*Signature :  ser_buff_t* (client_stub_marshal) <Arg1, Arg2, . . . ,> */
    ser_buff_t *client_send_ser_buffer = multiply_client_stub_marshal(a, b);
    ser_buff_t *client_recv_ser_buffer = NULL;

    /*Prepare for recieving the data from Server*/
    init_serialized_buffer_of_defined_size(&client_recv_ser_buffer, 
                        MAX_RECV_SEND_BUFF_SIZE);

    /*Step 3 : Send the serialized data to the server,
     * and wait for the reply*/

    /*Fn that will work for all RPCs
     * Signature : void (rpc_send_recv) <ser_buff_t *, ser_buff_t *> */
    
     rpc_send_recv(client_send_ser_buffer, client_recv_ser_buffer);
     /*After sending the data to the server, client should free the 
      * memory hold by the client_send_ser_buffer*/
     free_serialize_buffer(client_send_ser_buffer);
     client_send_ser_buffer = NULL;

    /*Step 4 , 5, 6 , 7, 8 are executed on Server side*/
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
