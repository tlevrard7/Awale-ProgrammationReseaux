#ifdef linux

#include <sys/types.h>

#endif

enum PACKET_IDS {
    PACKET_CONNECTION = 1,
    PACKET_CHAT = 2,
};

typedef struct ConnectionPacket {
    char name[256];
} ConnectionPacket;

size_t serialize_ConnectionPacket(ConnectionPacket *packet, char *buffer);
ConnectionPacket deserialize_ConnectionPacket(char *buffer, size_t n);

typedef struct ChatPacket {
    char sender;
    char message[256];
} ChatPacket;

size_t serialize_ChatPacket(ChatPacket *packet, char *buffer);
ChatPacket deserialize_ChatPacket(char *buffer, size_t n);