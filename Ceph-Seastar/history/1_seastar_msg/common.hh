#define PACKET_SIZE 1024
char send_packet[PACKET_SIZE];
char recv_packet[PACKET_SIZE];
char _send_packet[PACKET_SIZE];
char _recv_packet[PACKET_SIZE];

std::atomic<int> send_size;
std::atomic<int> recv_size;


void task1(char msg[])
{
    while(1) {
        while(send_size!=0) sleep(1);
        memcpy(send_packet, msg, 5);
        send_size=5;
    }
}
