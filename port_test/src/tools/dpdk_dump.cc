#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>
#include <signal.h>

#include "port/dpdk.h"

/* 
typedef uint16_t (*rte_rx_callback_fn)(uint16_t port, uint16_t queue,
  struct rte_mbuf *pkts[], uint16_t nb_pkts, uint16_t max_pkts,
  void *user_param);

*/
uint16_t callback(uint16_t port, uint16_t queue,
    struct rte_mbuf *pkts[], uint16_t nb_pkts,
    uint16_t max_pkts, void *user_param)
{
  for (size_t i=0; i<nb_pkts; i++) {
    rte_pktmbuf_dump(stdout, pkts[i], 0);
    printf("\n");
  }
}

//can not dump , why ???
int main(int argc, char** argv)
{
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <pid> <qid> \n", argv[0]);
    return -1;
  }

  int innner_argc = 2;
  const char* innner_argv[2] = {
    argv[0],
    "--proc-type=secondary"
  };

  //rte_log_set_global_level(RTE_LOG_EMERG);
  DpdkRte::Instance()->RteInit(innner_argc, (char**)innner_argv, true);

  const size_t pid = atoi(argv[1]);
  const size_t qid = atoi(argv[2]);
  printf("polling port%zd:%zd ...\n", pid, qid);
  rte_eth_add_rx_callback(pid, qid, callback, nullptr);
  while (true) ;
  rte_eth_remove_rx_callback(pid, qid, nullptr);
}