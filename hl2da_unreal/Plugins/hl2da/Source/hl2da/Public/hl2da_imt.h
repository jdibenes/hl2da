// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <memory>
#include "hl2da_api.h"

/**
 * 
 */
class HL2DA_API hl2da_imt
{
private:
	void* container;

	void const* buffer;
	int32_t length;

	void Reset();

public:
	hl2da_imt();
	~hl2da_imt();

	void const* Buffer();
	int32_t Length();

	void Destroy();

	static std::shared_ptr<hl2da_imt> Convert(uint8_t const* image, uint32_t stride, uint32_t height, hl2da_api::IMT_Format format_in, hl2da_api::IMT_Format format_out);
	static uint32_t GetStride_PV(uint32_t width);
};
