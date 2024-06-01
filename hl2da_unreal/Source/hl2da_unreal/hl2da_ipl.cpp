// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_ipl.h"
#include "hl2da_api.h"

// Sets default values for this component's properties
Uhl2da_ipl::Uhl2da_ipl()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void Uhl2da_ipl::BeginPlay()
{
	Super::BeginPlay();

	// ...
	hl2da_api::pv_captureformat pvcf = hl2da_api::CreateFormat_PV(640, 360, 30, false, false);
	hl2da_api::MC_CHANNELS mcch = hl2da_api::MC_CHANNELS::USE_5;
	hl2da_api::EE_FPS_INDEX eefi = hl2da_api::EE_FPS_INDEX::FPS_90;

	hl2da_api::InitializeLibrary();
	hl2da_api::InitializeComponents();

	// TODO: Link coordinate systems
	hl2da_api::OverrideWorldCoordinateSystem(NULL); 

	hl2da_api::SetFormat_PV(pvcf);
	hl2da_api::SetFormat_Microphone(mcch);
	hl2da_api::SetFormat_ExtendedEyeTracking(eefi);

	hl2da_api::BypassDepthLock_RM(true);

	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_LEFTFRONT,       60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_LEFTLEFT,        60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_RIGHTFRONT,      60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_RIGHTRIGHT,      60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_DEPTH_AHAT,          90);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_DEPTH_LONGTHROW,     60); // Maximum is 18
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_ACCELEROMETER,   24);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_GYROSCOPE,       48);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_MAGNETOMETER,    10);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::PV,                     15); // Maximum is 18
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::MICROPHONE,            125);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::SPATIAL_INPUT,          60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::EXTENDED_EYE_TRACKING, 180);

	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_VLC_LEFTFRONT,      true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_VLC_LEFTLEFT,       true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_VLC_RIGHTFRONT,     true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_VLC_RIGHTRIGHT,     true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_DEPTH_AHAT,         true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_DEPTH_LONGTHROW,    true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_IMU_ACCELEROMETER,  true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_IMU_GYROSCOPE,      true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::RM_IMU_MAGNETOMETER,   true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::PV,                    true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::MICROPHONE,            true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::SPATIAL_INPUT,         true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::EXTENDED_EYE_TRACKING, true);
}


// Called every frame
void Uhl2da_ipl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

