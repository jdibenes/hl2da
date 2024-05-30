// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_framebuffer.h"
#include "hl2da_api.h"

hl2da_framebuffer::hl2da_framebuffer()
{
    Reset();
}

hl2da_framebuffer::~hl2da_framebuffer()
{
    Destroy();
}

int32_t hl2da_framebuffer::Status()
{
    return status;
}

int32_t hl2da_framebuffer::Id()
{
    return id;
}

uint64_t hl2da_framebuffer::Timestamp()
{
    return timestamp;
}

int32_t hl2da_framebuffer::Framestamp()
{
    return framestamp;
}

int32_t hl2da_framebuffer::Valid()
{
    return valid;
}

void const* hl2da_framebuffer::Buffer(uint32_t index)
{
    if (index >= (sizeof(buffer) / sizeof(void*))) { return nullptr; }
    return buffer[index];
}

int32_t hl2da_framebuffer::Length(uint32_t index)
{
    if (index >= (sizeof(buffer) / sizeof(void*))) { return 0; }
    return length[index];
}

void hl2da_framebuffer::Reset()
{
    frame = nullptr;
    status = -1;
    id = -1;
    timestamp = 0;
    framestamp = 0;
    valid = 0;
    memset(buffer, 0, sizeof(buffer));
    memset(length, 0, sizeof(length));
}

void hl2da_framebuffer::Destroy()
{
    if (!frame) { return; }
    hl2da_api::Release(id, frame);
    Reset();
}

void hl2da_framebuffer::Extract(hl2da_framebuffer& fb)
{
    hl2da_api::Extract(fb.id, fb.frame, &fb.valid, fb.buffer, fb.length);
}

std::shared_ptr<hl2da_framebuffer> hl2da_framebuffer::GetFrame(int id, int framestamp)
{
    std::shared_ptr<hl2da_framebuffer> fb = std::make_shared<hl2da_framebuffer>();
    fb->id = id;
    fb->status = hl2da_api::GetByFramestamp(id, framestamp, &fb->frame, &fb->timestamp, &fb->framestamp);
    if (fb->frame) { Extract(*fb); }
    return fb;
}

std::shared_ptr<hl2da_framebuffer> hl2da_framebuffer::GetFrame(int id, uint64_t timestamp, int time_preference, bool tiebreak_right)
{
    std::shared_ptr<hl2da_framebuffer> fb = std::make_shared<hl2da_framebuffer>();
    fb->id = id;
    fb->status = hl2da_api::GetByTimestamp(id, timestamp, time_preference, tiebreak_right, &fb->frame, &fb->timestamp, &fb->framestamp);
    if (fb->frame) { Extract(*fb); }
    return fb;
}
