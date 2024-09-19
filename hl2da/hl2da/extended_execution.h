
#pragma once

#include <stdint.h>

enum INTERFACE_ID
{
    ID_RM_VLF =  0,
    ID_RM_VLL =  1,
    ID_RM_VRF =  2,
    ID_RM_VRR =  3,
    ID_RM_ZHT =  4,
    ID_RM_ZLT =  5,
    ID_RM_ACC =  6,
    ID_RM_GYR =  7,
    ID_RM_MAG =  8,
    ID_PV     = 10,
    ID_MC     = 11,
    ID_SI     = 12,
    ID_EET    = 17,
    ID_EA     = 18,
    ID_EV     = 19,
};

void ExtendedExecution_Request();
bool ExtendedExecution_Status();
void ExtendedExecution_SetInterfacePriority(uint32_t id, int32_t priority);
int32_t ExtendedExecution_GetInterfacePriority(uint32_t id);
