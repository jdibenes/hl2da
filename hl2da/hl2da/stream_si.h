
#pragma once

#include <Windows.h>
#include "spatial_input.h"

#include <winrt/Windows.Perception.h>

class si_frame
{
private:
    ULONG m_count;

public:
    int32_t valid;
    SpatialInput_Frame head_pose;
    SpatialInput_Ray eye_ray;
    winrt::Windows::Perception::People::JointPose left_hand[HAND_JOINTS];
    winrt::Windows::Perception::People::JointPose right_hand[HAND_JOINTS];

    si_frame();

    ULONG AddRef();
    ULONG Release();
};

void SI_SetEnable(bool enable);

int SI_Get(int32_t stamp, si_frame*& f, uint64_t& t, int32_t& s);
int SI_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, si_frame*& f, uint64_t& t, int32_t& s);

void SI_Initialize(int buffer_size);
void SI_Quit();
void SI_Cleanup();
