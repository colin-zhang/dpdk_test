//
// Created by colin on 18-5-6.
//
#include "main/main.h"
#include "port/dpdk.h"
#include "util/define.h"

#define PKT_READ_SIZE 256
int cap_slave(__attribute__((unused)) void *ptr_data)
{
    struct rte_mbuf *pkts[PKT_READ_SIZE];
    uint8_t port_id = 0;
    uint16_t rx_pkts = 0;

    int core_id = rte_lcore_id();
    int ring_num = core_id % GetRte()->CapPortNum();
    uint16_t c = 0;
    int j = 0;

    for (;;) {
        if (Unlikely(CheckStoping())) {
            break;
        }
        for (j = 0; j < GetRte()->port_num; j++) {
            port_id = GetRte()->GetPort(j)->PortId();
            rx_pkts = rte_eth_rx_burst(port_id, ring_num, pkts, PKT_READ_SIZE);
            if (rx_pkts > 0) {
                for (c = 0; c < (rx_pkts & ((~(uint16_t)0x3))); c += 4) {
                    rte_pktmbuf_free(pkts[c]);
                    rte_pktmbuf_free(pkts[c + 1]);
                    rte_pktmbuf_free(pkts[c + 2]);
                    rte_pktmbuf_free(pkts[c + 3]);
                }
                switch (rx_pkts & 0x3) {
                    case 3:
                        rte_pktmbuf_free(pkts[c++]);
                    case 2:
                        rte_pktmbuf_free(pkts[c++]);
                    case 1:
                        rte_pktmbuf_free(pkts[c++]);
                }
            }
        }
    }
    return 0;
}
