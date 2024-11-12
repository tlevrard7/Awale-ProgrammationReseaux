
enum PACKET_IDS {
    PACKET_CONNECTION = 1
};

struct {
    char name[256];
} typedef ConnectionPacket;

ConnectionPacket buffer_to_ConnectionPacket(char *buffer);
ConnectionPacket ConnectionPacket_to_buffer(char *buffer, ConnectionPacket packet);