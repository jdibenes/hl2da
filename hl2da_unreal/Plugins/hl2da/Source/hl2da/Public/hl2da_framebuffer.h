// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "hl2da_api.h"

/**
 * 
 */
class HL2DA_API hl2da_framebuffer
{
private:    
    void* frame;

    hl2da_api::STATUS status;
    hl2da_api::SENSOR_ID id;
    uint64_t timestamp;
    int32_t framestamp;
    int32_t valid;

    void const* buffer[4];
    int32_t length[4];

    void Reset();
    static void Extract(hl2da_framebuffer& fb);

public:
    hl2da_framebuffer();
	~hl2da_framebuffer();

    hl2da_api::STATUS Status();
    hl2da_api::SENSOR_ID Id();
    uint64_t Timestamp();
    int32_t Framestamp();
    int32_t Valid();

    void const* Buffer(uint32_t index);
    int32_t Length(uint32_t index);

    void Destroy();

    static std::shared_ptr<hl2da_framebuffer> GetFrame(hl2da_api::SENSOR_ID id, int framestamp);
    static std::shared_ptr<hl2da_framebuffer> GetFrame(hl2da_api::SENSOR_ID id, uint64_t timestamp, hl2da_api::TIME_PREFERENCE time_preference, bool tiebreak_right);
};
