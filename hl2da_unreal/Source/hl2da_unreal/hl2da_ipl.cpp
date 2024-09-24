// Fill out your copyright notice in the Description page of Project Settings.


#include "hl2da_ipl.h"
#include "hl2da_api.h"
#include "hl2da_itc.h"
#include "hl2da_framebuffer.h"

// Sets default values for this component's properties
Uhl2da_ipl::Uhl2da_ipl()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void Uhl2da_ipl::BeginPlay()
{
	Super::BeginPlay();

	// ...
	pvcf = hl2da_api::CreateFormat_PV(640, 360, 30, false, false); // TODO: if you change the resolution here you also have to change it in pv_viewer.cpp
	mcch = hl2da_api::MC_CHANNELS::USE_5;
	eefi = hl2da_api::EE_FPS_INDEX::FPS_90;
	eacf = hl2da_api::CreateFormat_EA(hl2da_api::MIXER_MODE::MICROPHONE, 2, 0); // Select external microphone if any
	evcf = hl2da_api::CreateFormat_EV(1280, 720, 30, L"YUY2", false, 2, 0, 0); // Select external camera if any  // TODO: if you change the resolution here you also have to change it in ev_viewer.cpp

	for (int id = 0; id <= 14; ++id) { last_framestamp[id] = -1; }
	pv_settings_latch = false;

	hl2da_api::InitializeLibrary();
	hl2da_itc::InitializeLibrary();

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
	/*
	// TEST 1
	for (int id = 0; id <= 14; ++id)
	{
		std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame((hl2da_api::SENSOR_ID)id, -1);
		if (fb->Status() != hl2da_api::STATUS::OK) { continue; }
		
		if (fb->Framestamp() <= last_framestamp[id]) { continue; }
		last_framestamp[id] = fb->Framestamp();
		
		switch (id)
		{
		case  0:
		case  1:
		case  2:
		case  3: Update_RM_VLC(fb); break;
		case  4: Update_RM_Depth_AHAT(fb); break;
		case  5: Update_RM_Depth_Longthrow(fb); break;
		case  6: Update_RM_IMU_Accelerometer(fb); break;
		case  7: Update_RM_IMU_Gyroscope(fb); break;
		case  8: Update_RM_IMU_Magnetometer(fb); break;
		case  9: Update_PV(fb); break;
		case 10: Update_Microphone(fb); break;
		case 11: Update_SpatialInput(fb); break;
		case 12: Update_ExtendedEyeTracking(fb); break;
		case 13: Update_ExtendedAudio(fb); break;
		case 14: Update_ExtendedVideo(fb); break;
		}
	}
	*/
	/*
	// TEST 2
	std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame(hl2da_api::SENSOR_ID::RM_VLC_LEFTFRONT, -2);
	if (fb->Status() != hl2da_api::STATUS::OK) { return; }
	if (last_framestamp[(int)fb->Id()] >= fb->Framestamp()) { return; }
	last_framestamp[(int)fb->Id()] = fb->Framestamp();

	std::shared_ptr<hl2da_framebuffer> fb_s = hl2da_framebuffer::GetFrame(hl2da_api::SENSOR_ID::RM_VLC_RIGHTFRONT, fb->Timestamp(), hl2da_api::TIME_PREFERENCE::NEAREST, false);
	if (fb_s->Status() != hl2da_api::STATUS::OK) { return; }

	FString text = FString::Printf(TEXT("GOT FRAME PAIR %lld - %lld, delta: %lld"), fb_s->Timestamp(), fb->Timestamp(), fb_s->Timestamp() - fb->Timestamp());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text).Get());
	*/

	/*
	// TEST 3
	std::shared_ptr<hl2da_framebuffer> fb = hl2da_framebuffer::GetFrame(hl2da_api::SENSOR_ID::PV, -1);
	if (fb->Status() != hl2da_api::STATUS::OK) { return; }
	if (pv_settings_latch) { return; }
	pv_settings_latch = true;

	hl2da_api::PV_SetFocus(hl2da_api::PV_FocusMode::Manual, hl2da_api::PV_AutoFocusRange::Normal, hl2da_api::PV_ManualFocusDistance::Infinity, 200, hl2da_api::PV_DriverFallback::Disable);
	hl2da_api::PV_SetVideoTemporalDenoising(hl2da_api::PV_VideoTemporalDenoisingMode::Off);
	hl2da_api::PV_SetWhiteBalance_Preset(hl2da_api::PV_ColorTemperaturePreset::Manual);
	hl2da_api::PV_SetWhiteBalance_Value(3000);
	hl2da_api::PV_SetExposure(hl2da_api::PV_ExposureMode::Manual, 2000);
	hl2da_api::PV_SetExposurePriorityVideo(hl2da_api::PV_ExposurePriorityVideo::Disabled);
	hl2da_api::PV_SetSceneMode(hl2da_api::PV_CaptureSceneMode::Backlit);
	hl2da_api::PV_SetIsoSpeed(hl2da_api::PV_IsoSpeedMode::Manual, 100);
	hl2da_api::PV_SetBacklightCompensation(hl2da_api::PV_BacklightCompensationState::Enable);
	*/
}

FString Uhl2da_ipl::PoseToString(float const* pose)
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
		hl2da_api::RM_GetExtrinsics((hl2da_api::SENSOR_ID)id, &extrinsics[0][0]);
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

		hl2da_api::RM_MapImagePointToCameraUnitPlane((hl2da_api::SENSOR_ID)id, image_points, 2, mpoint, 2, sizeof(image_points) / sizeof(float) / 2);
		hl2da_api::RM_MapCameraSpaceToImagePoint((hl2da_api::SENSOR_ID)id, camera_points, 2, ppoint, 2, sizeof(camera_points) / sizeof(float) / 2);
		hl2da_api::DebugMessage(StringCast<ANSICHAR>(*FString::Printf(TEXT("RM %d calibration %f %f %f %f"), id, mpoint[0], mpoint[1], ppoint[0], ppoint[1])).Get());
	}

	uint64_t utc_offset = hl2da_api::GetUTCOffset();
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*FString::Printf(TEXT("utc_offset=%lld"), utc_offset)).Get());
}

void Uhl2da_ipl::Update_RM_VLC(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* pose = (float*)fb->Buffer(3);
	FString text_pose = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_RM_Depth_AHAT(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* pose = (float*)fb->Buffer(3);
	FString text_pose = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_RM_Depth_Longthrow(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* pose = (float*)fb->Buffer(3);
	FString text_pose = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_RM_IMU_Accelerometer(std::shared_ptr<hl2da_framebuffer> fb)
{
	hl2da_api::AccelDataStruct const* samples = (hl2da_api::AccelDataStruct*)fb->Buffer(0);
	float const* pose = (float*)fb->Buffer(3);

	FString text_sample = FString::Printf(TEXT("RM %d sample 0: VinylHupTicks=%lld, SocTicks=%lld, x=%f, y=%f, z=%f, temperature=%f"), fb->Id(), samples[0].VinylHupTicks, samples[0].SocTicks, samples[0].AccelValues[0], samples[0].AccelValues[1], samples[0].AccelValues[2], samples[0].temperature);
	FString text_pose = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_sample).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_RM_IMU_Gyroscope(std::shared_ptr<hl2da_framebuffer> fb)
{
	hl2da_api::GyroDataStruct const* samples = (hl2da_api::GyroDataStruct*)fb->Buffer(0);
	float const* pose = (float*)fb->Buffer(3);

	FString text_sample = FString::Printf(TEXT("RM %d sample 0: VinylHupTicks=%lld, SocTicks=%lld, x=%f, y=%f, z=%f, temperature=%f"), fb->Id(), samples[0].VinylHupTicks, samples[0].SocTicks, samples[0].GyroValues[0], samples[0].GyroValues[1], samples[0].GyroValues[2], samples[0].temperature);
	FString text_pose   = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_sample).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_RM_IMU_Magnetometer(std::shared_ptr<hl2da_framebuffer> fb)
{
	hl2da_api::MagDataStruct const* samples = (hl2da_api::MagDataStruct*)fb->Buffer(0);
	float const* pose = (float*)fb->Buffer(3);

	FString text_sample = FString::Printf(TEXT("RM %d sample 0: VinylHupTicks=%lld, SocTicks=%lld, x=%f, y=%f, z=%f"), fb->Id(), samples[0].VinylHupTicks, samples[0].SocTicks, samples[0].MagValues[0], samples[0].MagValues[1], samples[0].MagValues[2]);
	FString text_pose   = FString::Printf(TEXT("RM %d Pose: "), fb->Id()) + PoseToString(pose);

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_sample).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_PV(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* k    = (float*)fb->Buffer(2);
	float const* pose = (float*)fb->Buffer(3);

	FString text_k    = FString::Printf(TEXT("PV Intrinsics: fx=%f, fy=%f, cx=%f, cy=%f"), k[0], k[1], k[2], k[3]);
	FString text_pose = TEXT("PV Pose: ") + PoseToString(pose);

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_k).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_Microphone(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* samples = (float*)fb->Buffer(0);
	FString text;

	switch (mcch)
	{
	case hl2da_api::MC_CHANNELS::USE_5: text = FString::Printf(TEXT("Microphone: ch0=%f, ch1=%f, ch2=%f, ch3=%f, ch4=%f"), samples[0], samples[1], samples[2], samples[3], samples[4]); break;
	case hl2da_api::MC_CHANNELS::USE_2: text = FString::Printf(TEXT("Microphone: ch0=%f, ch1=%f"), samples[0], samples[1]); break;
	default: text = TEXT("Microphone: <INVALID>");
	}

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text).Get());
}

void Uhl2da_ipl::Update_SpatialInput(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* head = (float*)fb->Buffer(0);
	float const* eye = (float*)fb->Buffer(1);
	hl2da_api::JointPose const* left = (hl2da_api::JointPose*)fb->Buffer(2);
	hl2da_api::JointPose const* right = (hl2da_api::JointPose*)fb->Buffer(3);

	hl2da_api::JointPose const* lw = &left[(int)hl2da_api::SI_HandJointKind::Wrist];
	hl2da_api::JointPose const* rw = &right[(int)hl2da_api::SI_HandJointKind::Wrist];
	int32_t valid = fb->Valid();

	FString text_head  = (valid & (int)hl2da_api::SI_VALID::HEAD)  ? FString::Printf(TEXT("SI Head: position=[%f, %f, %f], forward=[%f, %f, %f], up=[%f, %f, %f]"), head[0], head[1], head[2], head[3], head[4], head[5], head[6], head[7], head[8]) : TEXT("<INVALID>");
	FString text_eye   = (valid & (int)hl2da_api::SI_VALID::EYE)   ? FString::Printf(TEXT("SI Eye: origin=[%f, %f, %f], direction=[%f, %f, %f]"), eye[0], eye[1], eye[2], eye[3], eye[4], eye[5]) : TEXT("<INVALID>");
	FString text_left  = (valid & (int)hl2da_api::SI_VALID::LEFT)  ? FString::Printf(TEXT("SI Left Wrist: orientation=[%f, %f, %f, %f], position=[%f, %f, %f], radius=%f, accuracy=%d"), lw->rx, lw->ry, lw->rz, lw->rw, lw->tx, lw->ty, lw->tz, lw->radius, lw->accuracy) : TEXT("<INVALID>");
	FString text_right = (valid & (int)hl2da_api::SI_VALID::RIGHT) ? FString::Printf(TEXT("SI Right Wrist: orientation=[%f, %f, %f, %f], position=[%f, %f, %f], radius=%f, accuracy=%d"), rw->rx, rw->ry, rw->rz, rw->rw, rw->tx, rw->ty, rw->tz, rw->radius, rw->accuracy) : TEXT("<INVALID>");

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_head).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_eye).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_left).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_right).Get());
}

void Uhl2da_ipl::Update_ExtendedEyeTracking(std::shared_ptr<hl2da_framebuffer> fb)
{
	float const* d = (float*)fb->Buffer(0);
	float const* pose = (float*)fb->Buffer(3);
	int32_t valid = fb->Valid();

	FString text_combined = (valid & (int)hl2da_api::EE_VALID::COMBINED_GAZE) ? FString::Printf(TEXT("EE Combined Gaze: origin=[%f, %f, %f] direction=[%f, %f, %f] vergence=%f"), d[0], d[1], d[2], d[3], d[4], d[5], d[20]) : TEXT("<INVALID>");
	FString text_left     = (valid & (int)hl2da_api::EE_VALID::LEFT_GAZE)     ? FString::Printf(TEXT("EE Left Gaze: origin=[%f, %f, %f] direction=[%f, %f, %f] openness=%f"), d[6], d[7], d[8], d[9], d[10], d[11], d[18]) : TEXT("<INVALID>");
	FString text_right    = (valid & (int)hl2da_api::EE_VALID::RIGHT_GAZE)    ? FString::Printf(TEXT("EE Right Gaze: origin=[%f, %f, %f] direction=[%f, %f, %f] openness=%f"), d[12], d[13], d[14], d[15], d[16], d[17], d[19]) : TEXT("<INVALID>");
	FString text_pose     = TEXT("EE Pose: ") + PoseToString(pose) + ((valid & (int)hl2da_api::EE_VALID::CALIBRATION) ? TEXT("") : TEXT("<BAD CALIBRATION>"));

	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_combined).Get());	
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_left).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_right).Get());
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text_pose).Get());
}

void Uhl2da_ipl::Update_ExtendedAudio(std::shared_ptr<hl2da_framebuffer> fb)
{
	void const* samples = fb->Buffer(0);
	hl2da_api::ea_audioformat const* format = (hl2da_api::ea_audioformat*)fb->Buffer(2);
	
	FString text_sample;
	switch (format->bits_per_sample)
	{
	case 32: text_sample = FString::Printf(TEXT("ch0=%f"), ((float*)samples)[0]); break;
	case 16: text_sample = FString::Printf(TEXT("ch0=%d"), ((int16_t*)samples)[0]); break;
	default: text_sample = TEXT("ch0=<U>");
	}

	FString text = FString::Printf(TEXT("EA: channel_count=%d, bits_per_sample=%d, sample_rate=%d, bitrate=%d, subtype=%s, "), format->channel_count, format->bits_per_sample, format->sample_rate, format->bitrate, format->subtype) + text_sample;
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text).Get());
}

void Uhl2da_ipl::Update_ExtendedVideo(std::shared_ptr<hl2da_framebuffer> fb)
{
	hl2da_api::ev_videoformat const* format = (hl2da_api::ev_videoformat*)fb->Buffer(2);
	FString text = FString::Printf(TEXT("EV: width=%d, height=%d, framerate=%d, subtype=%s"), format->width, format->height, format->framerate, format->subtype);
	hl2da_api::DebugMessage(StringCast<ANSICHAR>(*text).Get());
}
