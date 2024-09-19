
#pragma once

#include <stdint.h>

enum INTERFACE_ID
{
    ID_RM_VLF,
    ID_RM_VLL,
    ID_RM_VRF,
    ID_RM_VRR,
    ID_RM_ZHT,
    ID_RM_ZLT,
    ID_RM_ACC,
    ID_RM_GYR,
    ID_RM_MAG,
    ID_PV,
    ID_MC,
    ID_SI,
    ID_EET,
    ID_EA,
    ID_EV,
};

void ExtendedExecution_Request();
bool ExtendedExecution_Status();
void ExtendedExecution_SetInterfacePriority(uint32_t id, int32_t priority);
int32_t ExtendedExecution_GetInterfacePriority(uint32_t id);
