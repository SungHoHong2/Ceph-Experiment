#define PACKET_SIZE 1024
char send_packet[PACKET_SIZE];
char recv_packet[PACKET_SIZE];
char _send_packet[PACKET_SIZE];
char _recv_packet[PACKET_SIZE];

std::atomic<int> send_size;
std::atomic<int> recv_size;
