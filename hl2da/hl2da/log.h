
#pragma once

#include <winrt/Windows.Foundation.h>

void ShowMessage(const char* format, ...);
void ShowMessage(const wchar_t* format, ...);

winrt::hstring GUIDToString(GUID const& iid);
