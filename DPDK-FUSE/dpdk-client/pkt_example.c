/*
 * The main thread that does the work, reading from
 * an input port and writing to an output port.
 */
struct message {
	char data[DATA_SIZE];
};

static __attribute__(()) void
lcore_main(void)
{
	const uint8_t nb_ports = rte_eth_dev_count();
	uint8_t port;

	for (port = 0; port < nb_ports; port++)
		if (rte_eth_dev_socket_id(port) > 0 &&
			rte_eth_dev_socket_id(port) !=
			(int)rte_socket_id())
			printf("WARNING, port %u is on remote NUMA node to "
				   "polling thread.\n\tPerformance will "
				   "not be optimal.\n", port);

	struct rte_mbuf *pkt;
	struct ether_hdr *eth_hdr;

	struct message obj;
	struct message *msg;
	int nb_rx = 0, nb_tx = 0, cnt = 0, pkt_size = 0;
	int count = 0;
	int k = 0;
	for (count = 0; count < DATA_SIZE; count++){
		obj.data[count] = (char)(97 + (k++));
		if (k == 26)
			k = 0;
	}
	time_t endtime = time(NULL) + 10;
	port = 0;
	while (time(NULL) < endtime) {
		cnt = rte_eth_rx_burst(port, 0, &pkt, 1);
		nb_rx += cnt;

		if (cnt > 0)
		{
			eth_hdr = rte_pktmbuf_mtod(pkt, struct ether_hdr *);

			rte_eth_macaddr_get(port, &eth_hdr->s_addr);

			pkt_size = sizeof(struct message) + sizeof(struct ether_hdr);
			msg = (struct message *) (rte_pktmbuf_mtod(pkt, char*)) + sizeof(struct ether_hdr);
			rte_pktmbuf_free(pkt);
		}

		msg = &obj;
		pkt = rte_pktmbuf_alloc(mbuf_pool);
		pkt_size = sizeof(struct message) + sizeof(struct ether_hdr);
		pkt->data_len = pkt_size;
		pkt->pkt_len = pkt_size;
		eth_hdr = rte_pktmbuf_mtod(pkt, struct ether_hdr *);
		rte_eth_macaddr_get(port, &eth_hdr->d_addr);
		rte_eth_macaddr_get(port ^ 1, &eth_hdr->s_addr);
		eth_hdr->ether_type = htons(PTP_PROTOCOL);
		char* data;

		data = rte_pktmbuf_append(pkt, sizeof(struct message));
		if (data != NULL)
			rte_memcpy(data, msg, sizeof(struct message));

		nb_tx += rte_eth_tx_burst(port ^ 1, 0, &pkt, 1);
	}
	printf("----\nData size: %d\nPacket size: %d\nRX : %d, TX : %d\n\n", DATA_SIZE, pkt_size, nb_rx, nb_tx);
}