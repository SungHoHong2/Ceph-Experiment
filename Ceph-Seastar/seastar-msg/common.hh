#define PACKET_SIZE 1024
char send_packet[PACKET_SIZE];
char recv_packet[PACKET_SIZE];
char _send_packet[PACKET_SIZE];
char _recv_packet[PACKET_SIZE];

std::atomic<int> send_size;
std::atomic<int> recv_size;

int total_number_of_packets = -1;
void task1(char msg[])
{
    while(1 && (total_number_of_packets!=0)) {
//        while(send_size!=0) sleep(1);
//        memcpy(send_packet, msg, 5);
//        send_size=5;
//        total_number_of_packets--;
    }
}
