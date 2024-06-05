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
	hl2da_api::ea_captureformat eacf = hl2da_api::CreateFormat_EA(hl2da_api::MIXER_MODE::MICROPHONE, 2, 0); // Select external microphone if any
	hl2da_api::ev_captureformat evcf = hl2da_api::CreateFormat_EV(1280, 720, 30, L"YUY2", false, 2, 0, 0); // Select external camera if any

	hl2da_api::InitializeLibrary();
	hl2da_api::InitializeComponents();

	// TODO: Link coordinate systems
	hl2da_api::OverrideWorldCoordinateSystem(NULL); 

	hl2da_api::SetFormat_PV(pvcf);
	hl2da_api::SetFormat_Microphone(mcch);
	hl2da_api::SetFormat_ExtendedEyeTracking(eefi);
	hl2da_api::SetFormat_ExtendedAudio(eacf);
	hl2da_api::SetFormat_ExtendedVideo(evcf);

	PrintDebugInfo();

	hl2da_api::BypassDepthLock_RM(true);

	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_LEFTFRONT,       60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_LEFTLEFT,        60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_RIGHTFRONT,      60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_VLC_RIGHTRIGHT,      60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_DEPTH_AHAT,          90);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_DEPTH_LONGTHROW,     15); // Maximum is 18
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_ACCELEROMETER,   24);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_GYROSCOPE,       48);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::RM_IMU_MAGNETOMETER,    10);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::PV,                     15); // Maximum is 18
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::MICROPHONE,            125);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::SPATIAL_INPUT,          60);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::EXTENDED_EYE_TRACKING, 180);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::EXTENDED_AUDIO,        125);
	hl2da_api::Initialize(hl2da_api::SENSOR_ID::EXTENDED_VIDEO,         15); // Maximum is 18

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
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::EXTENDED_AUDIO,        true);
	hl2da_api::SetEnable(hl2da_api::SENSOR_ID::EXTENDED_VIDEO,        true);
}


// Called every frame
void Uhl2da_ipl::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FString Uhl2da_ipl::PoseToString(float* pose)
{
	return FString::Printf(TEXT("[[%f, %f, %f, %f], [%f, %f, %f, %f], [%f, %f, %f, %f], [%f, %f, %f, %f]]"), pose[0], pose[1], pose[2], pose[3], pose[4], pose[5], pose[6], pose[7], pose[8], pose[9], pose[10], pose[11], pose[12], pose[13], pose[14], pose[15]);
}

void Uhl2da_ipl::PrintDebugInfo()
{
	float extrinsics[4][4];
	float image_points[2];
	float camera_points[2];
	float mpoint[2];
	float ppoint[2];

	for (int id = 0; id <= 7; ++id)
	{
		hl2da_api::GetExtrinsics_RM((hl2da_api::SENSOR_ID)id, &extrinsics[0][0]);
		hl2da_api::DebugMessage(StringCast<ANSICHAR>(*(FString::Printf(TEXT("RM %d extrinsics "), id) + PoseToString(&extrinsics[0][0]))).Get());
	}

	for (int id = 0; id <= 5; ++id)
	{
		switch (id)
		{
		case 0:
		case 1:
		case 2:
		case 3: image_points[0] = 320; image_points[1] = 240; camera_points[0] = 0; camera_points[1] = 0; break;
		case 4: image_points[0] = 256; image_points[1] = 256; camera_points[0] = 0; camera_points[1] = 0; break;
		case 5: image_points[0] = 160; image_points[1] = 144; camera_points[0] = 0; camera_points[1] = 0; break;
		}

		hl2da_api::MapImagePointToCameraUnitPlane_RM((hl2da_api::SENSOR_ID)id, image_points, mpoint, sizeof(image_points) / sizeof(float) / 2);
		hl2da_api::MapCameraSpaceToImagePoint_RM((hl2da_api::SENSOR_ID)id, camera_points, ppoint, sizeof(camera_points) / sizeof(float) / 2);
		hl2da_api::DebugMessage(StringCast<ANSICHAR>(*FString::Printf(TEXT("RM %d calibration %f %f %f %f"), id, mpoint[0], mpoint[1], ppoint[0], ppoint[1])).Get());
	}

	uint64_t utc_offset = hl2da_api::GetUTCOffset();
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*FString::Printf(TEXT("utc_offset=%lld"), utc_offset)).Get());
}
