// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <memory>
#include "hl2da_api.h"
#include "hl2da_framebuffer.h"
#include "hl2da_ipl.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HL2DA_UNREAL_API Uhl2da_ipl : public UActorComponent
{
	GENERATED_BODY()

	int last_framestamp[15];
	void PrintDebugInfo();
	void Update_RM_VLC(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_RM_Depth_AHAT(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_RM_Depth_Longthrow(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_RM_IMU_Accelerometer(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_RM_IMU_Gyroscope(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_RM_IMU_Magnetometer(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_PV(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_Microphone(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_SpatialInput(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_ExtendedEyeTracking(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_ExtendedAudio(std::shared_ptr<hl2da_framebuffer> fb);
	void Update_ExtendedVideo(std::shared_ptr<hl2da_framebuffer> fb);

public:	
	// Sets default values for this component's properties
	Uhl2da_ipl();

	static FString PoseToString(float const* pose);

	hl2da_api::pv_captureformat pvcf;
	hl2da_api::MC_CHANNELS mcch;
	hl2da_api::EE_FPS_INDEX eefi;
	hl2da_api::ea_captureformat eacf;
	hl2da_api::ev_captureformat evcf;
	bool pv_settings_latch;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
