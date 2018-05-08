#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <sys/time.h>

#include "rte_ethdev.h"
#include "rte_version.h"

#include "port/dpdk.h"
#include "util/access_cmdline.h"

#include "main/port_stat.h"
#include "main/pseudo_port.h"
#include "main/packet_thread.h"

#define CMD_PROMPT "port_test > "
#define PORT_TEST_VERSION "v0.0.2_r4"


bool gStopRunning = false;

DpdkRte* gDpdkRte = NULL;

static uint64_t gStartTime = 0;


static void StopAll()
{
    gStopRunning = true;
}

bool CheckStoping()
{
    return gStopRunning;
}

static uint64_t GetTimeUpNow() {
    struct timespec tpstart;
    clock_gettime(CLOCK_MONOTONIC, &tpstart);
    return tpstart.tv_sec;
}

static void PrintUseTime(uint64_t time) 
{
    int hour = time / 3600;
    int minute = (time - hour*3600) / 60;
    int second = time % 60;
    printf("程序运行距今: %d小时 %d分 %d秒\n", hour, minute, second);
}


static void help()
{
    printf(
           "支持以下命令:\n"
           "version 显示程序版本 \n"
           "uptime  程序运行时长 \n"
           "stat    端口状态 \n"
           "exit    退出程序\n"
        );
}

static void version()
{
    printf("dpdk version: %s\n"
            "tool version: %s \n"
        , rte_version()
        , PORT_TEST_VERSION
        );
}

static void CmdLineProcess()
{
    CmdLine cmd_line(CMD_PROMPT, ".test_history");
    cmd_line.addHints("exit");
    cmd_line.addHints("stop_peudo");
    cmd_line.addHints("stat");
    cmd_line.addHints("help");
    cmd_line.addHints("version");
    cmd_line.addHints("uptime");

    while (1) {
        if (unlikely(gStopRunning)) {
            break;
        }
        std::string cmd = cmd_line.readLine();
        if (cmd.size() > 0) {
            if (cmd == "exit") {
                StopAll();
                break;
            } else if (cmd == "stat") {
                printf("%s\n", PortStat().c_str());
            } else if (cmd == "stop_peudo") {
                PseudoSetStop(true);
            } else if (cmd == "help" || cmd == "?") {
                help();
            } else if (cmd == "version") {
                version();
            } else if (cmd == "uptime") {
                PrintUseTime(GetTimeUpNow() - gStartTime);
            }
            printf(CMD_PROMPT);
            fflush(stdout);
        }
    }
}

DpdkRte* GetRte()
{
    return gDpdkRte;
}

#include <unistd.h>
unsigned int ProcessingUnits()
{
    return ::sysconf(_SC_NPROCESSORS_ONLN);
}


int main(int argc, char *argv[])
{
    gDpdkRte = DpdkRte::Instance();
    gDpdkRte->RteInit(argc, argv);
    gDpdkRte->PortsInit();

    uint32_t id_core = rte_lcore_id();
    printf("main::id_core : %d \n", id_core);
    for (int c = 0; c < gDpdkRte->CapPortNum(); c++) {
        id_core = rte_get_next_lcore(id_core, 1, 1);
        if (rte_eal_remote_launch(cap_slave, NULL, id_core) != 0) {
            fprintf(stderr, "%2d, id_core[%d] %s\n", c, id_core, "can't launch");
        }
    }

    id_core = rte_get_next_lcore(id_core, 1, 1);
    if (rte_eal_remote_launch(stat_slave, NULL, id_core) != 0) {
        fprintf(stderr, "id_core[%d] %s\n",  id_core, "can't launch");
    }

//pseudo send
    id_core = rte_get_next_lcore(id_core, 1, 1);
    if (rte_eal_remote_launch(pseudo_send, NULL, id_core) != 0) {
        fprintf(stderr, "id_core[%d] %s\n",  id_core, "can't launch");
    }
//

    printf("main::cores:%u, lcore_count:%d\n", ProcessingUnits(), rte_lcore_count());

    gStartTime = GetTimeUpNow();

    gDpdkRte->StartPorts();
    CmdLineProcess();

    RTE_LCORE_FOREACH_SLAVE(id_core) {
        if (rte_eal_wait_lcore(id_core) < 0) {
            return -1;
        }
    }
    delete gDpdkRte;
    return 0;
}
