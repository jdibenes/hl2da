
#include "stream_mc.h"
#include "microphone_capture.h"
#include "ring_buffer.h"
#include "lock.h"
#include "log.h"

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.ApplicationModel.Core.h>

using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::ApplicationModel::Core;

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static SRWLOCK g_lock;
static ring_buffer<mc_frame*> g_buffer;
static HANDLE g_event_enable = NULL; // CloseHandle
static HANDLE g_event_quit = NULL; // CloseHandle
static HANDLE g_thread = NULL; // CloseHandle

static winrt::com_ptr<MicrophoneCapture> g_microphoneCapture = nullptr;
bool g_raw = false;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
mc_frame::mc_frame(uint8_t* b, int32_t l) : buffer(b), length(l), m_count(1)
{
}

// OK
ULONG mc_frame::AddRef()
{
	return InterlockedIncrement(&m_count);
}

// OK
ULONG mc_frame::Release()
{
	ULONG uCount = InterlockedDecrement(&m_count);
	if (uCount != 0) { return uCount; }
	if (buffer) { delete[] buffer; }
	delete this;
	return uCount;
}

// OK
static void MC_Insert(uint8_t* buffer, int32_t length, uint64_t timestamp)
{
	mc_frame* f;
	SRWLock srw(&g_lock, true);
	f = g_buffer.insert(new mc_frame(buffer, length), timestamp);
	if (f) { f->Release(); }
}

// OK
static void MC_Clear()
{
	SRWLock srw(&g_lock, true);
	for (int32_t i = 0; i < g_buffer.size(); ++i) { g_buffer.at(i)->Release(); }
	g_buffer.reset();
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

	MC_Clear();

	g_microphoneCapture->Stop();
}

// OK
static DWORD WINAPI MC_EntryPoint(void*)
{
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
int MC_Get(int32_t stamp, mc_frame*& f, uint64_t& t, int32_t& s)
{
	SRWLock srw(&g_lock, false);
	int v = g_buffer.get(stamp, f, t, s);
	if (v == 0) { f->AddRef(); }
	return v;
}

// OK
int MC_Get(uint64_t timestamp, int time_preference, bool tiebreak_right, mc_frame*& f, uint64_t& t, int32_t& s)
{
	SRWLock srw(&g_lock, false);
	int v = g_buffer.get(timestamp, time_preference, tiebreak_right, f, t, s);
	if (v == 0) { f->AddRef(); }
	return v;
}

// OK
void MC_Initialize(int32_t buffer_size)
{
	InitializeSRWLock(&g_lock);
	g_buffer.reset(buffer_size);
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
