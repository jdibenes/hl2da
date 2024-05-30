// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct pv_captureformat
{
    bool enable_mrc;
    bool hologram_composition;
    bool recording_indicator;
    bool video_stabilization;
    bool blank_protected;
    bool show_mesh;
    bool shared;
    uint8_t _reserved_0[1];
    float global_opacity;
    float output_width;
    float output_height;
    uint32_t video_stabilization_length;
    uint32_t hologram_perspective;
    uint16_t width;
    uint16_t height;
    uint8_t framerate;
    uint8_t _reserved_1[3];
};

/**
 * 
 */
class HL2DA_API hl2da_api
{
private:
    static void* hmod_mmret;
    static void* hmod_hl2ss;

    static void* p_DebugMessage;
    static void* p_InitializeComponents;
    static void* p_OverrideWorldCoordinateSystem;
    static void* p_Initialize;
    static void* p_SetEnable;
    static void* p_GetByFramestamp;
    static void* p_GetByTimestamp;
    static void* p_Release;
    static void* p_Extract;
    static void* p_GetExtrinsics_RM;
    static void* p_MapImagePointToCameraUnitPlane_RM;
    static void* p_MapCameraSpaceToImagePoint_RM;
    static void* p_SetFormat_PV;
    static void* p_SetFormat_MC;
    static void* p_SetFormat_EE;

    static int Load();
    
public:
    static int InitializeLibrary();
    static void DebugMessage(char const* str);
    static void InitializeComponents();
    static int OverrideWorldCoordinateSystem(void*);
    static void Initialize(int id, int buffer_size);
    static void SetEnable(int id, bool enable);
    static int GetByFramestamp(int id, int stamp, void** frame, uint64_t* timestamp, int32_t* framestamp);
    static int GetByTimestamp(int id, uint64_t stamp, int time_preference, bool tiebreak_right, void** frame, uint64_t* timestamp, int32_t* framestamp);
    static void Release(int id, void* frame);
    static void Extract(int id, void* frame, int32_t* valid, void const** b, int32_t* l);
    static void GetExtrinsics_RM(int id, float* extrinsics);
    static void MapImagePointToCameraUnitPlane_RM(int id, float const* image_points, float* camera_points, int point_count);
    static void MapCameraSpaceToImagePoint_RM(int id, float const* camera_points, float* image_points, int point_count);
    static void SetFormat_PV(pv_captureformat const& cf);
    static void SetFormat_MC(bool raw);
    static void SetFormat_EE(int fps_index);
};
