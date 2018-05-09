//
// Created by colin on 18-5-6.
//

#ifndef ADPS_PORT_STAT_H
#define ADPS_PORT_STAT_H

#include <string>

int stat_slave(__attribute__((unused)) void *ptr_data);

std::string PortStat();
void PortStatReset();

#endif //ADPS_PORT_STAT_H
