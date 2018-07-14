00000000: A0 36 9F 83 AB BC 00 1B 21 A6 D4 D4 08 00 45 00 | .6......!.....E.
00000010: 01 49 7C 60 00 00 80 11 C9 37 0A DA 6E 59 0A DA | .I|`.....7..nY..
00000020: 6F FF 44 5C 44 5C 01 35 DF 27 7B 22 6E 61 6D 65 | o.D\D\.5.'{"name
00000030: 73 70 61 63 65 73 22 3A 20 5B 31 33 37 30 34 36 | spaces": [137046
00000040: 36 31 39 33 2C 20 32 39 33 31 32 37 36 38 39 36 | 6193, 2931276896
00000050: 2C 20 31 33 32 34 30 31 33 30 39 31 2C 20 31 33 | , 1324013091, 13
00000060: 35 32 37 38 34 34 35 32 2C 20 31 34 33 33 32 37 | 52784452, 143327
00000070: 32 31 36 36 2C 20 32 32 30 31 36 38 30 30 31 36 | 2166, 2201680016
00000080: 2C 20 31 33 35 39 38 30 30 32 37 30 2C 20 31 34 | , 1359800270, 14
00000090: 37 31 30 37 33 36 34 39 2C 20 31 39 37 34 38 36 | 71073649, 197486
000000A0: 31 33 39 32 2C 20 32 32 33 38 33 34 32 39 32 38 | 1392, 2238342928
000000B0: 2C 20 31 33 37 32 38 31 35 32 32 31 2C 20 33 30 | , 1372815221, 30
000000C0: 37 33 32 34 31 37 36 30 2C 20 37 36 32 35 31 36 | 73241760, 762516
000000D0: 39 35 35 2C 20 31 32 39 34 33 35 36 37 36 34 2C | 955, 1294356764,
000000E0: 20 31 35 32 30 34 39 31 39 39 35 5D 2C 20 22 64 |  1520491995], "d
000000F0: 69 73 70 6C 61 79 6E 61 6D 65 22 3A 20 22 22 2C | isplayname": "",
00000100: 20 22 76 65 72 73 69 6F 6E 22 3A 20 5B 32 2C 20 |  "version": [2,
00000110: 30 5D 2C 20 22 68 6F 73 74 5F 69 6E 74 22 3A 20 | 0], "host_int":
00000120: 32 30 39 37 36 33 35 34 38 32 33 39 38 34 38 32 | 2097635482398482
00000130: 38 34 35 36 30 36 31 34 37 35 30 34 32 32 37 38 | 8456061475042278
00000140: 33 35 34 37 35 37 32 2C 20 22 70 6F 72 74 22 3A | 3547572, "port":
00000150: 20 31 37 35 30 30 7D 68 65 6C 6C 6F 68 65 6C 6C |  17500}hellohell
00000160: 6F 68 65 6C 6C 6F 68 65 6C 6C 6F 68 65 6C 6C 6F | ohellohellohello
00000170: 68 65 6C 6C 6F 68 65 6C 6C 6F 68 65 6C 6C 6F 00 | hellohellohello.

dump mbuf at 0x7f751006c740, phys=4d986c7c0, buf_len=2176
  pkt_len=1367, ol_flags=180, nb_segs=1, in_port=0
  segment at 0x7f751006c740, data=0x7f751006c840, data_len=1367
  Dump data at [0x7f751006c840], len=1024

86 = 
209

```c++

/* dump a mbuf on console */
void
rte_pktmbuf_dump(FILE *f, const struct rte_mbuf *m, unsigned dump_len)
{
	unsigned int len;
	unsigned nb_segs;

	__rte_mbuf_sanity_check(m, 1);

	fprintf(f, "dump mbuf at %p, phys=%"PRIx64", buf_len=%u\n",
	       m, (uint64_t)m->buf_physaddr, (unsigned)m->buf_len);
	fprintf(f, "  pkt_len=%"PRIu32", ol_flags=%"PRIx64", nb_segs=%u, "
	       "in_port=%u\n", m->pkt_len, m->ol_flags,
	       (unsigned)m->nb_segs, (unsigned)m->port);
	nb_segs = m->nb_segs;

	while (m && nb_segs != 0) {
		__rte_mbuf_sanity_check(m, 0);

		fprintf(f, "  segment at %p, data=%p, data_len=%u\n",
			m, rte_pktmbuf_mtod(m, void *), (unsigned)m->data_len);
		len = dump_len;
		if (len > m->data_len)
			len = m->data_len;
		if (len != 0)
			rte_hexdump(f, NULL, rte_pktmbuf_mtod(m, void *), len);
		dump_len -= len;
		m = m->next;
		nb_segs --;
	}
}

```
