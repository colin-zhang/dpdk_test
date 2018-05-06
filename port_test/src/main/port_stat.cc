//
// Created by colin on 18-5-6.
//
#include <time.h>
#include <sys/time.h>

#include "port/dpdk.h"
#include "util/access_cmdline.h"
#include "util/utils.h"
#include "util/sfrb.h"

#include "main/main.h"


struct EthStats
{
    struct timeval tv;
    struct rte_eth_stats iostats;
};

struct EthStatsArray
{
    EthStats eth_stat[8];
    EthStatsArray() {
        memset(&eth_stat, 0, sizeof(eth_stat));
    }
};

struct EthRate
{
    double iMpps;
    double oMpps;
    double iMbps;
    double oMbps;
    void Clear() {
        iMpps = 0.0;
        oMpps = 0.0;
        iMbps = 0.0;
        oMbps = 0.0;
    }
};


Sfrb<EthStatsArray>* gEthStatRbPtr = NULL;

int CalaRate(EthStatsArray* est1, EthStatsArray* est0, EthRate* er, int i)
{
    uint64_t vs0 = static_cast<int64_t>(est0->eth_stat[i].tv.tv_sec) * 1000 * 1000 + est0->eth_stat[i].tv.tv_usec;
    uint64_t vs1 = static_cast<int64_t>(est1->eth_stat[i].tv.tv_sec) * 1000 * 1000 + est1->eth_stat[i].tv.tv_usec;

    er->Clear();

    if (vs1 <= vs0) {
        return -1;
    }

    uint64_t d_vs = vs1 - vs0;
    uint64_t d_ipackets = est1->eth_stat[i].iostats.ipackets - est0->eth_stat[i].iostats.ipackets;
    uint64_t d_opackets = est1->eth_stat[i].iostats.opackets - est0->eth_stat[i].iostats.opackets;
    uint64_t d_ibytes   = est1->eth_stat[i].iostats.ibytes - est0->eth_stat[i].iostats.ibytes;
    uint64_t d_obytes   = est1->eth_stat[i].iostats.obytes - est0->eth_stat[i].iostats.obytes;

    er->iMpps = static_cast<double>(d_ipackets) / d_vs;
    er->oMpps = static_cast<double>(d_opackets) / d_vs;
    er->iMbps = static_cast<double>(d_ibytes) / d_vs;
    er->oMbps = static_cast<double>(d_obytes) / d_vs;
    return 0;
}

std::string PortStat()
{
    int i = 0;
    struct rte_eth_stats iostats;
    struct rte_eth_link  link;
    std::string result;
    EthStatsArray esa0;
    EthStatsArray esa5;
    EthRate eth_rate;

    gEthStatRbPtr->GetOffset(&esa0, 0);
    gEthStatRbPtr->GetOffset(&esa5, 5);

    for (i = 0; i< GetRte()->port_num; i++)
    {
        uint8_t port_id = GetRte()->GetPort(i)->PortId();
        int ret = rte_eth_stats_get(port_id, &iostats);
        if (ret != 0) return "";
        CalaRate(&esa0, &esa5, &eth_rate, i);
        result += FormatStr("port %-3d \n"
                            "         ipackets  %-11llu\topackets  %-11llu\n"
                            "         ibytes    %-11llu\tobytes    %-11llu\n"
                            "         ierrors   %-11llu\toerrors   %-11llu\n"
                            "         rx_nombuf %-11llu\timissed   %-11llu\n"
                            "         iMpps     %-5.5f\toMpps     %-5.5f\n"
                            "         iMbps     %-5.5f\toMbps     %-5.5f\n"
                , port_id
                , iostats.ipackets,  iostats.opackets
                , iostats.ibytes,    iostats.obytes
                , iostats.ierrors,   iostats.oerrors
                , iostats.rx_nombuf, iostats.imissed
                , eth_rate.iMpps
                , eth_rate.oMpps
                , eth_rate.iMbps * 8
                , eth_rate.oMbps * 8
        );
        rte_eth_link_get_nowait(port_id, &link);
        result += FormatStr("         link      %-11s\t",
                            (link.link_speed == ETH_LINK_DOWN) ? "down" : "up");
        result += FormatStr("speed     %u %s\n",
                            (link.link_speed > ETH_SPEED_NUM_100M) ? link.link_speed / 1000 : link.link_speed,
                            (link.link_speed > ETH_SPEED_NUM_100M) ? "Gbps" : "Mbps");
    }
    return result;
}


int stat_slave(__attribute__((unused)) void *ptr_data)
{
    EthStatsArray esa;
    gEthStatRbPtr = new Sfrb<EthStatsArray>(30);

    while (!CheckStoping()) {
        sleep(1);
        for (int i = 0; i < GetRte()->port_num; i++) {
            uint8_t port_id = GetRte()->GetPort(i)->PortId();
            rte_eth_stats_get(port_id, &esa.eth_stat[i].iostats);
            gettimeofday(&esa.eth_stat[i].tv, NULL);
            gEthStatRbPtr->Set(&esa);
        }
    }

    delete gEthStatRbPtr;
    return 0;
}


