// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>

/**
 * 
 */
class HL2DA_API hl2da_framebuffer
{
private:    
    void* frame;

    int32_t status;
    int32_t id;
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

    int32_t Status();
    int32_t Id();
    uint64_t Timestamp();
    int32_t Framestamp();
    int32_t Valid();

    void const* Buffer(uint32_t index);
    int32_t Length(uint32_t index);

    void Destroy();

    static std::shared_ptr<hl2da_framebuffer> GetFrame(int id, int framestamp);
    static std::shared_ptr<hl2da_framebuffer> GetFrame(int id, uint64_t timestamp, int time_preference, bool tiebreak_right);
};
