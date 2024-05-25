
#pragma once

#include <vector>
#include <researchmode/ResearchModeApi.h>

#include <winrt/Windows.Perception.Spatial.h>

bool ResearchMode_WaitForConsent(IResearchModeSensor* sensor);

bool ResearchMode_Initialize();
void ResearchMode_Cleanup();

IResearchModeSensor* ResearchMode_GetSensor(ResearchModeSensorType type);
ResearchModeSensorType const* ResearchMode_GetSensorTypes();
int ResearchMode_GetSensorTypeCount();

winrt::Windows::Perception::Spatial::SpatialLocator ResearchMode_GetLocator();
