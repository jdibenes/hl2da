
#include "lock.h"
#include "custom_audio_effect.h"
#include "nfo.h"
#include "log.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.MediaProperties.h>
#include <winrt/Windows.Media.Devices.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.Data.Json.h>

using namespace winrt::Windows::Media::MediaProperties;
using namespace winrt::Windows::Media::Devices;
using namespace winrt::Windows::Media::Capture;
using namespace winrt::Windows::Media::Capture::Frames;
using namespace winrt::Windows::Data::Json;

struct source_format
{
    MediaFrameSource source;
    MediaFrameFormat format;
};

//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------

static HANDLE g_event = NULL;

static MediaCapture g_mediaCapture = nullptr;
static MediaFrameSource g_audioSource = nullptr;

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------

// OK
static void ExtendedAudio_OnFailed(MediaCapture const&, MediaCaptureFailedEventArgs const& b)
{
    ShowMessage(L"ExtendedAudio_OnFailed - 0x%X : '%s'", b.Code(), b.Message().c_str());
    if (g_event != NULL) { SetEvent(g_event); }
}

// OK
static bool ExtendedAudio_FindAudioSource(MediaCapture const& mediaCapture, MediaFrameSource& audioSource, int32_t source_index)
{
    std::vector<source_format> sources;

    for (auto const& frameSource : mediaCapture.FrameSources())
    {
    auto source = frameSource.Value();
    if (source.Info().MediaStreamType() != MediaStreamType::Audio) { continue; }
    for (auto const& format : source.SupportedFormats())
    {
    sources.push_back({ source, format });
    }
    }

    if (source_index >= sources.size()) { return false; }

    auto const& sf = sources[source_index];
    audioSource = sf.source;
    audioSource.SetFormatAsync(sf.format).get();
    return true;
}

// OK
void ExtendedAudio_QueryDevices(winrt::hstring& out)
{
    std::vector<winrt::hstring> ids;
    std::vector<winrt::hstring> names;

    GetAudioCaptureIdsAndNames(ids, names);

    JsonObject root = JsonObject();
    for (uint32_t i = 0; i < ids.size(); ++i)
    {
    JsonObject jsourceinfos = JsonObject();
    jsourceinfos.Insert(L"Id", JsonValue::CreateStringValue(ids[i]));
    jsourceinfos.Insert(L"Name", JsonValue::CreateStringValue(names[i]));

    root.Insert(winrt::to_hstring(i), jsourceinfos);
    }

    out = root.ToString();
}

// OK
void ExtendedAudio_RegisterEvent(HANDLE h)
{
    g_event = h;
}

// OK
bool ExtendedAudio_Open(MRCAudioOptions const& options)
{
    uint32_t index        = (options.mixer_mode & 0x0000FF00) >>  8;
    uint32_t source_index = (options.mixer_mode & 0x00FF0000) >> 16;
    MediaCaptureInitializationSettings settings;
    std::vector<winrt::hstring> ids;
    winrt::hstring id;
    bool ok;

    if (index <= 0)
    {
    id = MediaDevice::GetDefaultAudioCaptureId(AudioDeviceRole::Default);
    }
    else
    {
    index--;
    GetAudioCaptureIds(ids);
    if (index >= ids.size()) { return false; }
    id = ids[index];
    }

    settings.AudioDeviceId(id);
    settings.StreamingCaptureMode(StreamingCaptureMode::Audio);
    settings.SharingMode(MediaCaptureSharingMode::SharedReadOnly);
    settings.MediaCategory(MediaCategory::Media);

    g_mediaCapture = MediaCapture();
    g_mediaCapture.InitializeAsync(settings).get();
    g_mediaCapture.AddAudioEffectAsync(MRCAudioEffect(options)).get();

    g_mediaCapture.Failed({ ExtendedAudio_OnFailed });
    ok = ExtendedAudio_FindAudioSource(g_mediaCapture, g_audioSource, source_index);
    if (!ok)
    {
    g_mediaCapture.Close();
    g_mediaCapture = nullptr;
    return false;
    }

    return true;
}

// OK
void ExtendedAudio_Close()
{
    g_audioSource = nullptr;
    g_mediaCapture.Close();
    g_mediaCapture = nullptr;
}

// OK
MediaFrameReader ExtendedAudio_CreateFrameReader()
{
    return g_mediaCapture.CreateFrameReaderAsync(g_audioSource).get();
}
