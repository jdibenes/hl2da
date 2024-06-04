
#include "microphone_capture.h"
#include "frame_buffer.h"
#include "log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;

class mc_frame : public sensor_frame
{
public:
	uint8_t* buffer;
	int32_t length;

	mc_frame(uint8_t* b, int32_t l);
	~mc_frame();
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static frame_buffer g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

static winrt::com_ptr<MicrophoneCapture> g_microphoneCapture = nullptr;
static bool g_raw = false;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
mc_frame::mc_frame(uint8_t* b, int32_t l) : sensor_frame(), buffer(b), length(l)
{
}

mc_frame::~mc_frame()
{
	delete[] buffer;
}

// OK
static void MC_Insert(uint8_t* buffer, int32_t length, uint64_t timestamp)
{
	g_buffer.Insert(new mc_frame(buffer, length), timestamp);
}

// OK
static void MC_Acquire()
{
	bool ok;

	WaitForSingleObject(g_event_enable, INFINITE);

	g_microphoneCapture = winrt::make_self<MicrophoneCapture>();
	g_microphoneCapture->Initialize(g_raw);

	CoreApplication::Views().GetAt(0).Dispatcher().RunAsync(CoreDispatcherPriority::High, [=]() { g_microphoneCapture->Activate(); }).get();
	
	g_microphoneCapture->WaitActivate(INFINITE);

	ok = g_microphoneCapture->Status();
	if (!ok) { return; }

	g_microphoneCapture->Start();

	do { g_microphoneCapture->WriteSample(MC_Insert); } while (WaitForSingleObject(g_event_enable, 0) == WAIT_OBJECT_0);

	g_buffer.Clear();

	g_microphoneCapture->Stop();
}

// OK
static DWORD WINAPI MC_EntryPoint(void* param)
{
	(void)param;
	do { MC_Acquire(); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
	return 0;
}

// OK
void MC_SetFormat(bool raw)
{
	g_raw = raw;
}

// OK
void MC_SetEnable(bool enable)
{
    if (enable) { SetEvent(g_event_enable); } else { ResetEvent(g_event_enable); }
}

// OK
int MC_Get(int32_t stamp, void*& f, uint64_t& t, int32_t& s)
{
	return g_buffer.Get(stamp, (sensor_frame*&)f, t, s);
}

// OK
int MC_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, void*& f, uint64_t& t, int32_t& s)
{
	return g_buffer.Get(timestamp, time_preference, tiebreak_right, (sensor_frame*&)f, t, s);
}

// OK
void MC_Release(void* frame)
{
	((mc_frame*)frame)->Release();
}

// OK
void MC_Extract(void* frame, void const** buffer, int32_t* length)
{
	mc_frame* f = (mc_frame*)frame;
	*buffer = f->buffer;
	*length = (int32_t)f->length;
}

// OK
void MC_Initialize(int32_t buffer_size)
{
	g_buffer.Reset(buffer_size);
	g_event_enable = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_thread = CreateThread(NULL, 0, MC_EntryPoint, NULL, 0, NULL);
}

// OK
void MC_Quit()
{
	SetEvent(g_event_quit);
}

// OK
void MC_Cleanup()
{
	WaitForSingleObject(g_thread, INFINITE);

	CloseHandle(g_thread);
	CloseHandle(g_event_quit);
	CloseHandle(g_event_enable);

	g_thread = NULL;
	g_event_quit = NULL;
	g_event_enable = NULL;
	g_microphoneCapture = nullptr;
}
