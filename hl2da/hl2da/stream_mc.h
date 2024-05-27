
#pragma once

#include <Windows.h>

class mc_frame
{
private:
	ULONG m_count;

public:
	uint8_t* buffer;
	int32_t length;

	mc_frame(uint8_t* b, int32_t l);

	ULONG AddRef();
	ULONG Release();
};

void MC_SetFormat(bool raw);
void MC_SetEnable(bool enable);

int MC_Get(int32_t stamp, mc_frame*& f, uint64_t& t, int32_t& s);
int MC_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, mc_frame*& f, uint64_t& t, int32_t& s);

void MC_Initialize(int32_t buffer_size);
void MC_Quit();
void MC_Cleanup();
