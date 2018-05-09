#include "port/dpdk.h"

#include "main/main.h"

#include "util/pcap.h"


bool gStopPeudo = false;

void PseudoSetStop(bool stop_flag)
{
    gStopPeudo = stop_flag;
}


extern struct rte_ring *ring[2];
int pseudo_send(__attribute__((unused)) void *ptr_data)
{
    uint32_t nb_rx_enqueued, freed, num;
    RteMbufPtr mbufs[128];
    RteMemPoolPtr mbuf_pool = NULL;
    int socket_id = rte_socket_id();

    printf("socket_id = %d , rte_lcore_id = %d \n", socket_id, rte_lcore_id());

    mbuf_pool = rte_pktmbuf_pool_create("", 1024, 512, 0, RTE_MBUF_DEFAULT_BUF_SIZE, socket_id);

    rte_pktmbuf_alloc_bulk(mbuf_pool, mbufs, 128);
    num = 128;


    //PcapReader pcap_reader(1);

    //pcap_reader.ReadPcapFile("ryf.pcap");


    while (!CheckStoping()) {
        if (!gStopPeudo) {
            //nb_rx_enqueued = rte_ring_enqueue_bulk(ring[0], (void* const*)mbufs, num, &freed);
        }

          //rte_pktmbuf_alloc
    }
    return 0;
}
