#include <string>
#include <stdio.h>
#include "port/dpdk.h"

DpdkRte* gDpdkRte = NULL;

void mp_dump(rte_mempool* mp, void* ptr)
{
    printf("name     : %s \n", mp->name);
    printf("socket_id: %u \n", mp->socket_id);
    printf("size     : %u (using %.0f%%) \n", mp->size,
    rte_mempool_in_use_count(mp)/float(mp->size)*100);
    printf("in-use   : %u \n", rte_mempool_in_use_count(mp));
    printf("avail    : %u \n\n", rte_mempool_avail_count(mp));
}

void dpdkctl_mp_show()
{
    //rte_mempool* mps[100];
    rte_mempool_walk(mp_dump, NULL);

    // printf("\n");
    // for (size_t i=0; i<n_mp; i++) {
    //     printf("Mempool[%zd]\n", i);
    //     printf("-----------------------------\n");
    //     //dpdk::mp_dump(mps[i]);
    //     printf("\n");
    // }
}

int main(int argc, char** argv)
{
  int innner_argc = 2;
  const char* innner_argv[2] = {
    argv[0],
    "--proc-type=secondary"
  };

  //rte_log_set_global_level(RTE_LOG_EMERG);
  gDpdkRte = DpdkRte::Instance();
  gDpdkRte->RteInit(innner_argc, (char**)innner_argv, true);
  //dpdk::dpdk_boot(innner_argc, (char**)innner_argv);
  dpdkctl_mp_show();
}
