
#include "microphone_capture.h"
#include "stream_mc.h"
#include "nfo.h"
#include "neon.h"
#include "timestamps.h"
#include "log.h"
#include "types.h"

//-----------------------------------------------------------------------------
// MicrophoneCapture Methods
//-----------------------------------------------------------------------------

// OK
MicrophoneCapture::MicrophoneCapture() :
	m_audioClient(nullptr),
	m_audioCaptureClient(nullptr),
	m_wfx(nullptr),
	m_eventActivate(NULL),
	m_eventData(NULL),
	m_raw(false),
	m_status(false)
{
}

// OK
MicrophoneCapture::~MicrophoneCapture()
{
	m_audioClient = nullptr;
	m_audioCaptureClient = nullptr;
	if (m_wfx) { CoTaskMemFree(m_wfx); }
	if (m_eventActivate) { CloseHandle(m_eventActivate); }
	if (m_eventData) { CloseHandle(m_eventData); }
}

// OK
bool MicrophoneCapture::Initialize(bool raw)
{
	m_raw = raw;

	m_eventActivate = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	return m_eventActivate;
}

// OK
void MicrophoneCapture::Activate()
{
	winrt::com_ptr<IActivateAudioInterfaceAsyncOperation> asyncOp;
	HRESULT hr;

	hr = ActivateAudioInterfaceAsync(GetBuiltInAudioCaptureId().c_str(), __uuidof(IAudioClient3), nullptr, this, asyncOp.put());
	if (FAILED(hr)) { SetEvent(m_eventActivate); }	
}

// OK
HRESULT MicrophoneCapture::Configure(IActivateAudioInterfaceAsyncOperation* operation)
{
	INT64 const buffer_length = 8 * HNS_BASE;
	UINT32 defaultPeriodInFrames;
	UINT32 fundamentalPeriodInFrames;
	UINT32 minPeriodInFrames;
	UINT32 maxPeriodInFrames;

	winrt::com_ptr<IUnknown> punkAudioInterface;
	AudioClientProperties acp;
	HRESULT activateStatus;
	HRESULT hr;
	WAVEFORMATEXTENSIBLE* wfe;

	hr = operation->GetActivateResult(&activateStatus, punkAudioInterface.put());
	if (FAILED(hr)) { return hr; }
	if (FAILED(activateStatus)) { return activateStatus; }

	m_audioClient = punkAudioInterface.as<IAudioClient3>();

	if (m_raw)
	{
	acp.cbSize = sizeof(AudioClientProperties);
	acp.eCategory = AudioCategory_Media;
	acp.Options = AUDCLNT_STREAMOPTIONS_RAW;
	acp.bIsOffload = FALSE;
	hr = m_audioClient->SetClientProperties(&acp);
	if (FAILED(hr)) { return hr; }
	}

	hr = m_audioClient->GetMixFormat(&m_wfx);
	if (FAILED(hr)) { return hr; }

	wfe = reinterpret_cast<WAVEFORMATEXTENSIBLE*>(m_wfx);

	hr = m_audioClient->GetSharedModeEnginePeriod(m_wfx, &defaultPeriodInFrames, &fundamentalPeriodInFrames, &minPeriodInFrames, &maxPeriodInFrames);
	if (FAILED(hr)) { return hr; }

	hr = m_audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_EVENTCALLBACK, buffer_length, 0, m_wfx, NULL);
	if (FAILED(hr)) { return hr; }

	m_eventData = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (!m_eventData) { return E_FAIL; }

	hr = m_audioClient->SetEventHandle(m_eventData);
	if (FAILED(hr)) { return hr; }

	m_audioCaptureClient.capture(m_audioClient, &IAudioClient::GetService);

	return S_OK;
}

// OK
HRESULT MicrophoneCapture::ActivateCompleted(IActivateAudioInterfaceAsyncOperation* operation)
{
	HRESULT hr = Configure(operation);
	if (SUCCEEDED(hr)) { m_status = true; }
	SetEvent(m_eventActivate);
	return S_OK;
}

// OK
bool MicrophoneCapture::WaitActivate(DWORD milliseconds)
{
	return WaitForSingleObject(m_eventActivate, milliseconds) == WAIT_OBJECT_0;
}

// OK
void MicrophoneCapture::Start()
{
	m_audioClient->Start();
}

// OK
bool MicrophoneCapture::Status()
{
	return m_status;
}

// OK
void MicrophoneCapture::Stop()
{
	m_audioClient->Stop();
	m_audioClient->Reset();
}

// OK
void MicrophoneCapture::WriteSample(void(*writer)(uint8_t*, int32_t, uint64_t))
{
	UINT32 framesAvailable; // ReleaseBuffer
	uint8_t* buffer;
	BYTE* data;
	DWORD dwCaptureFlags;
	int data_size;
	int bytes;
	UINT64 dps;
	UINT64 qpc;

	WaitForSingleObject(m_eventData, INFINITE);

	while (m_audioCaptureClient->GetBuffer(&data, &framesAvailable, &dwCaptureFlags, &dps, &qpc) == S_OK)
	{
	data_size = framesAvailable * m_wfx->nBlockAlign;
	bytes = m_raw ? (framesAvailable * 5 * sizeof(float)) : data_size;
	buffer = new uint8_t[bytes];

	if (dwCaptureFlags & AUDCLNT_BUFFERFLAGS_SILENT) { memset(buffer, 0, bytes); } else if (m_raw) { Neon_F32CropAudio11to5((float*)data, data_size / sizeof(float), (float*)buffer); } else { memcpy(buffer, data, bytes); }

	m_audioCaptureClient->ReleaseBuffer(framesAvailable);

	writer(buffer, bytes / sizeof(float), qpc);
	}
}
