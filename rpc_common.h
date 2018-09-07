#ifndef __RPC_COMMON__
#define __RPC_COMMON__

#define MAX_RECV_SEND_BUFF_SIZE 32
#define serialize_int(a,b,c)    (memcpy(a,b,c))
#define deserialize_int(a,b,c)  (memcpy(a,b,c))

#define SERVER_PORT 2000
#define SERVER_IP "127.0.0.1"

#endif /* __RPC_COMMON__ */
