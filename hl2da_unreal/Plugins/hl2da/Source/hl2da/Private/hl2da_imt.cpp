// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_imt.h"

hl2da_imt::hl2da_imt()
{
    Reset();
}

hl2da_imt::~hl2da_imt()
{
    Destroy();
}

void const* hl2da_imt::Buffer()
{
    return buffer;
}

int32_t hl2da_imt::Length()
{
    return length;
}

void hl2da_imt::Reset()
{
    container = nullptr;
    buffer = nullptr;
    length = 0;
}

void hl2da_imt::Destroy()
{
    if (!container) { return; }
    hl2da_api::IMT_Release(container);
    Reset();
}

std::shared_ptr<hl2da_imt> hl2da_imt::Convert(uint8_t const* image, uint32_t stride, uint32_t height, hl2da_api::IMT_Format format_in, hl2da_api::IMT_Format format_out)
{
    std::shared_ptr<hl2da_imt> p = std::make_shared<hl2da_imt>();
    hl2da_api::IMT_YUV2RGB(image, stride, height, format_in, format_out, &p->container);
    if (p->container) { hl2da_api::IMT_Extract(p->container, &p->buffer, &p->length); }
    return p;
}

uint32_t hl2da_imt::GetStride_PV(uint32_t width)
{
    return width + ((64 - (width & 63)) & 63);
}
