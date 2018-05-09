#include <stdio.h>
#include <stdarg.h>

#include <string>
#include <stdexcept>

#include "port/dpdk.h"
#include "rte_bus_pci.h"

inline std::string format(const char* fmt, ...)
{
    char str[1000];
    va_list args;
    va_start(args, fmt);
    vsprintf(str, fmt, args);
    va_end(args);
  return str;
}


inline std::string ether_addr2str(const ether_addr* addr)
{
  std::string s;
  for (size_t i=0; i<6; i++) {
    s += format("%02x%s", addr->addr_bytes[i], i<5?":":"");
  }
  return s;
}

inline std::string rte_pci_device2str(struct rte_pci_device* dev)
{
  std::string str = format("%04x:%02x:%02x.%01x",
    dev->addr.domain, dev->addr.bus,
    dev->addr.devid, dev->addr.function
    );
  return str;
}

void port_dump(size_t pid)
{
  constexpr const char* space = "        ";
  struct rte_eth_stats stats = {0,};

  if (!rte_eth_dev_is_valid_port(pid)) {
    printf("%s, [%d]\n", "not valid port", pid);
    return;
  }

  struct rte_eth_link link;
  rte_eth_link_get(pid, &link);
  //hahaha
  if (link.link_duplex == link.link_autoneg) {
    int ret = rte_eth_stats_get(pid, &stats);
    if (ret < 0) {
      //throw std::exception("some err occured");
      return;
    }
  }

  struct ether_addr addr;
  struct rte_eth_dev_info info;
  rte_eth_macaddr_get(pid, &addr);
  rte_eth_dev_info_get(pid, &info);

  printf("dpdk%1zd   HWaddr %s %s%s\n", pid,
      ether_addr2str(&addr).c_str(),
      rte_eth_promiscuous_get(pid)==1?"PROMISC ":"",
      rte_eth_allmulticast_get(pid)==1?"MULTICAST ":"");
  printf("%slink_speed:%u link_duplex:%d link_autoneg:%d link_status:%d\n", space,
    link.link_speed, link.link_duplex, link.link_autoneg, link.link_status
    );
  printf("%spciaddr %s socket_id:%u drv:%s\n",
      space, rte_pci_device2str(info.pci_dev).c_str(),
      rte_eth_dev_socket_id(pid), info.driver_name);
  printf("%sRX packets:%zd errors:%zd missed:%zd nombuf:%zd \n", space,
    stats.ipackets, stats.ierrors,
    stats.imissed, stats.rx_nombuf);
  printf("%sTX opackets:%zd errors:%zd RX nombuf:%zd\n",space,
    stats.opackets, stats.oerrors, stats.rx_nombuf);
  printf("%sRX bytes:%zd (%.1lf MB) TX bytes:%zd (%.1lf MB)\n", space,
    stats.ibytes, stats.ibytes/1000000.0,
    stats.obytes, stats.obytes/1000000.0);
}


void dpdkctl_port_show()
{
  size_t n_port = rte_eth_dev_count();
  for (size_t i=0; i < n_port; i++) {
    port_dump(i);
    printf("\n");
  }
}

int main(int argc, char** argv)
{
  int innner_argc = 2;
  const char* innner_argv[2] = {
    argv[0],
    "--proc-type=secondary"
  };

    rte_log_set_global_level(RTE_LOG_EMERG);
    DpdkRte::Instance()->RteInit(innner_argc, (char**)innner_argv, true);
    dpdkctl_port_show();
}
