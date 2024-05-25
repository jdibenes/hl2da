
#include "research_mode.h"

#include <winrt/Windows.Perception.Spatial.h>
#include <winrt/Windows.Perception.Spatial.Preview.h>

using namespace winrt::Windows::Perception::Spatial;
using namespace winrt::Windows::Perception::Spatial::Preview;

extern "C" { HMODULE LoadLibraryA(LPCSTR lpLibFileName); }

typedef HRESULT(__cdecl* PFN_CREATEPROVIDER)(IResearchModeSensorDevice**);

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static ResearchModeSensorType const g_types[] =
{
	LEFT_FRONT,
	LEFT_LEFT,
	RIGHT_FRONT,
	RIGHT_RIGHT,
	DEPTH_AHAT,
	DEPTH_LONG_THROW,
	IMU_ACCEL,
	IMU_GYRO,
	IMU_MAG
};

static HMODULE g_hrResearchMode = NULL; // FreeLibrary
static IResearchModeSensorDevice* g_pSensorDevice = NULL; // Release
static IResearchModeSensorDeviceConsent* g_pSensorDeviceConsent = NULL; // Release
static HANDLE g_camera_consent_event = NULL; // CloseHandle
static HANDLE g_imu_consent_event = NULL; // CloseHandle
static ResearchModeSensorConsent g_camera_consent_value = ResearchModeSensorConsent::UserPromptRequired;
static ResearchModeSensorConsent g_imu_consent_value = ResearchModeSensorConsent::UserPromptRequired;
static std::vector<IResearchModeSensor*> g_sensors; // Release
static SpatialLocator g_locator = nullptr;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
static void ResearchMode_CamAccessCallback(ResearchModeSensorConsent consent)
{
	g_camera_consent_value = consent;
	SetEvent(g_camera_consent_event);
}

// OK
static void ResearchMode_IMUAccessCallback(ResearchModeSensorConsent consent)
{
	g_imu_consent_value = consent;
	SetEvent(g_imu_consent_event);
}

// OK
static bool ResearchMode_WaitForCameraConsent()
{
	WaitForSingleObject(g_camera_consent_event, INFINITE);
	return g_camera_consent_value == ResearchModeSensorConsent::Allowed;
}

// OK
static bool ResearchMode_WaitForIMUConsent()
{
	WaitForSingleObject(g_imu_consent_event, INFINITE);
	return g_imu_consent_value == ResearchModeSensorConsent::Allowed;
}

// OK
bool ResearchMode_WaitForConsent(IResearchModeSensor *sensor)
{
	switch (sensor->GetSensorType())
	{
	case LEFT_FRONT:
	case LEFT_LEFT:
	case RIGHT_FRONT:
	case RIGHT_RIGHT:
	case DEPTH_AHAT:
	case DEPTH_LONG_THROW: return ResearchMode_WaitForCameraConsent();
	case IMU_ACCEL:
	case IMU_GYRO:
	case IMU_MAG:          return ResearchMode_WaitForIMUConsent();
	}

	return false;
}

// OK
bool ResearchMode_Initialize()
{
	IResearchModeSensorDevicePerception* pSensorDevicePerception; // Release
	HRESULT hr;
	std::vector<ResearchModeSensorDescriptor> sensordescriptors;
	PFN_CREATEPROVIDER pfnCreate;
	size_t sensorcount;
	size_t sensorsloaded;
	GUID rigNodeId;
	
	g_hrResearchMode = LoadLibraryA("ResearchModeAPI");
	if (!g_hrResearchMode) { return false; }

	pfnCreate = reinterpret_cast<PFN_CREATEPROVIDER>(GetProcAddress(g_hrResearchMode, "CreateResearchModeSensorDevice"));
	if (!pfnCreate) { return false; }

	hr = pfnCreate(&g_pSensorDevice);
	if (FAILED(hr)) { return false; }

	hr = g_pSensorDevice->QueryInterface(IID_PPV_ARGS(&g_pSensorDeviceConsent));
	if (FAILED(hr)) { return false; }

	g_camera_consent_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!g_camera_consent_event) { return false; }

	hr = g_pSensorDeviceConsent->RequestCamAccessAsync(ResearchMode_CamAccessCallback);
	if (FAILED(hr)) { return false; }

	g_imu_consent_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!g_imu_consent_event) { return false; }

	hr = g_pSensorDeviceConsent->RequestIMUAccessAsync(ResearchMode_IMUAccessCallback);
	if (FAILED(hr)) { return false; }
	
	hr = g_pSensorDevice->GetSensorCount(&sensorcount);
	if (FAILED(hr)) { return false; }

	sensordescriptors.resize(sensorcount);	
	hr = g_pSensorDevice->GetSensorDescriptors(sensordescriptors.data(), sensordescriptors.size(), &sensorsloaded);
	if (FAILED(hr)) { return false; }
	if (sensorcount != sensorsloaded) { return false; }

	g_sensors.clear();
	g_sensors.resize(sensorcount);

	memset(g_sensors.data(), 0, g_sensors.size() * sizeof(IResearchModeSensor*));

	for (auto const& sensorDescriptor : sensordescriptors)
	{
	hr = g_pSensorDevice->GetSensor(sensorDescriptor.sensorType, &g_sensors[sensorDescriptor.sensorType]);
	if (FAILED(hr)) { return false; }
	}

	hr = g_pSensorDevice->QueryInterface(IID_PPV_ARGS(&pSensorDevicePerception));
	if (FAILED(hr)) { return false; }

	hr = pSensorDevicePerception->GetRigNodeId(&rigNodeId);
	if (FAILED(hr)) { return false; }

	pSensorDevicePerception->Release();

	g_locator = SpatialGraphInteropPreview::CreateLocatorForNode(rigNodeId);

	return true;
}

// OK
void ResearchMode_Cleanup()
{
	for (auto sensor : g_sensors) { if (sensor) { sensor->Release(); } }	

	if (g_camera_consent_event) { CloseHandle(g_camera_consent_event); }
	if (g_imu_consent_event) { CloseHandle(g_imu_consent_event); }

	if (g_pSensorDeviceConsent) { g_pSensorDeviceConsent->Release(); }
	if (g_pSensorDevice) { g_pSensorDevice->Release(); }

	if (g_hrResearchMode) { FreeLibrary(g_hrResearchMode); }

	g_hrResearchMode = NULL;
	g_pSensorDevice = NULL;
	g_pSensorDeviceConsent = NULL;
	g_camera_consent_event = NULL;
	g_imu_consent_event = NULL;
	g_camera_consent_value = ResearchModeSensorConsent::UserPromptRequired;
	g_imu_consent_value = ResearchModeSensorConsent::UserPromptRequired;	
	g_sensors.clear();
	g_locator = nullptr;
}

// OK
IResearchModeSensor* ResearchMode_GetSensor(ResearchModeSensorType type)
{
	return g_sensors[type];
}

// OK
ResearchModeSensorType const* ResearchMode_GetSensorTypes()
{
	return g_types;
}

// OK
int ResearchMode_GetSensorTypeCount()
{
	return sizeof(g_types) / sizeof(ResearchModeSensorType);
}

// OK
SpatialLocator ResearchMode_GetLocator()
{
	return g_locator;
}
