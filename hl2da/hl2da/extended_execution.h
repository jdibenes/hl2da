
#pragma once

#include <stdint.h>

void ExtendedExecution_Request();
bool ExtendedExecution_Status();
void ExtendedExecution_SetInterfacePriority(uint32_t id, int32_t priority);
int32_t ExtendedExecution_GetInterfacePriority(uint32_t id);
