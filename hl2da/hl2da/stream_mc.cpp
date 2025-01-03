
#include "microphone_capture.h"
#include "extended_execution.h"
#include "frame_buffer.h"
#include "server_ports.h"

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
static bool g_raw = false;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
mc_frame::mc_frame(uint8_t* b, int32_t l) : sensor_frame(), buffer(b), length(l)
{
}

// OK
mc_frame::~mc_frame()
{
	delete[] buffer;
}

// OK
static void MC_AudioF32Crop11To5(float* out, float const* in, int32_t samples)
{
	for (int i = 0; i < (samples / 4); ++i)
	{
	float32x4_t f = vld1q_f32(in);
	switch (i % 11)
	{
	case 0:
		out[0] = f.n128_f32[0];
		out[1] = f.n128_f32[1];
		out[2] = f.n128_f32[2];
		out[3] = f.n128_f32[3];
		out += 4;
		break;
	case 1:
		out[0] = f.n128_f32[0];
		out += 1;
		break;
	case 2:
		out[0] = f.n128_f32[3];
		out += 1;
		break;
	case 3:
		out[0] = f.n128_f32[0];
		out[1] = f.n128_f32[1];
		out[2] = f.n128_f32[2];
		out[3] = f.n128_f32[3];
		out += 4;
		break;
	case 4:
		break;
	case 5:
		out[0] = f.n128_f32[2];
		out[1] = f.n128_f32[3];
		out += 2;
		break;
	case 6:
		out[0] = f.n128_f32[0];
		out[1] = f.n128_f32[1];
		out[2] = f.n128_f32[2];
		out += 3;
		break;
	case 7:
		break;
	case 8:
		out[0] = f.n128_f32[1];
		out[1] = f.n128_f32[2];
		out[2] = f.n128_f32[3];
		out += 3;
		break;
	case 9:
		out[0] = f.n128_f32[0];
		out[1] = f.n128_f32[1];
		out += 2;
		break;
	case 10:
		break;
	}
	in += 4;
	}
}

// OK
static void MC_Push(BYTE* data, UINT32 frames, bool silent, UINT64 timestamp, void* param)
{
	bool     raw      = *(bool*)param;
	uint32_t channels = raw ? 5 : 2;
	uint32_t samples  = frames * channels;
	uint32_t bytes    = samples * sizeof(float);
	uint8_t* buffer   = new uint8_t[bytes];

	if      (silent) { memset(                      buffer,            0,       bytes); }
	else if (raw)    { MC_AudioF32Crop11To5((float*)buffer, (float*)data, frames * 11); }
	else             { memcpy(                      buffer,         data,       bytes); }

	g_buffer.Insert(new mc_frame(buffer, samples), timestamp);
}

// OK
static void MC_Acquire(int base_priority)
{
	WaitForSingleObject(g_event_enable, INFINITE);

	bool raw = g_raw;
	MicrophoneCapture_Open(raw);
	if (!MicrophoneCapture_Status()) { return; }
	SetThreadPriority(GetCurrentThread(), ExtendedExecution_GetInterfacePriority(PORT_ID_MC));
	MicrophoneCapture_ExecuteSensorLoop(MC_Push, &raw, g_event_enable);
	SetThreadPriority(GetCurrentThread(), base_priority);
	MicrophoneCapture_Close();
	g_buffer.Clear();	
}

// OK
static DWORD WINAPI MC_EntryPoint(void* param)
{
	(void)param;
	int base_priority = GetThreadPriority(GetCurrentThread());
	do { MC_Acquire(base_priority); } while (WaitForSingleObject(g_event_quit, 0) == WAIT_TIMEOUT);
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
	(void)enable;
    SetEvent(g_event_enable);
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
	g_event_quit = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_event_enable = CreateEvent(NULL, FALSE, FALSE, NULL);
	g_thread = CreateThread(NULL, 0, MC_EntryPoint, NULL, 0, NULL);
}

// OK
void MC_Cleanup()
{
	SetEvent(g_event_quit);
	WaitForSingleObject(g_thread, INFINITE);
	CloseHandle(g_thread);
	CloseHandle(g_event_enable);
	CloseHandle(g_event_quit);
}
