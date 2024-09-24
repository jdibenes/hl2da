
#include <windows.h>
#include "locator.h"
#include "research_mode.h"
#include "personal_video.h"
#include "spatial_input.h"
#include "stream_rm.h"
#include "stream_pv.h"
#include "stream_mc.h"
#include "stream_si.h"
#include "stream_ee.h"
#include "stream_ea.h"
#include "stream_ev.h"
#include "converter.h"
#include "timestamps.h"
#include "log.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Input.h>

using namespace winrt::Windows;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Numerics;
using namespace winrt::Windows::UI;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Composition;

struct App : winrt::implements<App, IFrameworkViewSource, IFrameworkView>
{
    bool m_windowClosed = false;
    bool m_init = false;    

    IFrameworkView CreateView()
    {
        return *this;
    }

    void OnSuspending(IInspectable const& sender, SuspendingEventArgs const& args)
    {
        (void)sender;
        (void)args;

        CoreApplication::Exit(); // Suspending is not supported
    }

    void Initialize(CoreApplicationView const &)
    {
        CoreApplication::Suspending({ this, &App::OnSuspending });
    }

    void Load(winrt::hstring const&)
    {
    }

    void Uninitialize()
    {
    }

    void SetWindow(CoreWindow const& window)
    {
        window.Closed({ this, &App::OnWindowClosed });

        if (m_init) { return; }

        Locator_Initialize();
        ResearchMode_Initialize();
        PersonalVideo_Initialize();
        SpatialInput_Initialize();
        Converter_Initialize();

        RM_InitializeDepthLock();

        m_init = true;
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();

        window.Activate();

        pv_captureformat pv_cf;
        MRCAudioOptions ea_cf;
        ev_captureformat ev_cf;

        memset(&pv_cf, 0, sizeof(pv_cf));
        pv_cf.vf.width = 640;
        pv_cf.vf.height = 360;
        pv_cf.vf.framerate = 30;
        pv_cf.mrcvo.enable = false;
        pv_cf.mrcvo.shared = false;

        ea_cf.loopback_gain = 1.0;
        ea_cf.microphone_gain = 1.0;
        ea_cf.mixer_mode = 0;

        memset(&ev_cf, 0, sizeof(ev_cf));
        ev_cf.vf.width = 640;
        ev_cf.vf.height = 360;
        ev_cf.vf.framerate = 30;
        wcscpy_s(ev_cf.vf.subtype, L"NV12");
        ev_cf.mrcvo.shared = false;
        ev_cf.mrcvo.global_opacity = 0; // index group
        ev_cf.mrcvo.output_width = 2; // index source
        ev_cf.mrcvo.output_height = 4; // index profile        
        
        int32_t stamp = 0;
        void* frame;
        uint64_t timestamp;
        int32_t translated_stamp;
        int64_t start = GetCurrentQPCTimestamp();
        bool pv_status = true;

        RM_BypassDepthLock(true);

        PV_SetFormat(pv_cf);
        MC_SetFormat(true);
        EE_SetFormat(0);
        EA_SetFormat(ea_cf);
        EV_SetFormat(ev_cf);
        
        RM_Initialize(0, 15);
        RM_Initialize(1, 15);
        RM_Initialize(2, 15);
        RM_Initialize(3, 15);
        RM_Initialize(4, 15);
        RM_Initialize(5, 15);
        RM_Initialize(6, 15);
        RM_Initialize(7, 15);
        RM_Initialize(8, 15);

        PV_Initialize(15);
        MC_Initialize(15);
        SI_Initialize(15);
        EE_Initialize(15);
        EA_Initialize(15);
        EV_Initialize(15);

        RM_SetEnable(0, true);
        RM_SetEnable(1, true);
        RM_SetEnable(2, true);
        RM_SetEnable(3, true);
        RM_SetEnable(4, true);
        RM_SetEnable(5, true);
        RM_SetEnable(6, true);
        RM_SetEnable(7, true);
        RM_SetEnable(8, true);

        PV_SetEnable(true);
        MC_SetEnable(true);
        SI_SetEnable(true);
        EE_SetEnable(true);
        EA_SetEnable(true);
        EV_SetEnable(true);      
        
        while (!m_windowClosed)
		{
		window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        uint64_t vlc_timestamp;
        int32_t vlc_translated_stamp;
        void* vlc_frame;
        int status_vlc = RM_Get(0, -1, vlc_frame, vlc_timestamp, vlc_translated_stamp);
        if (status_vlc == 0) { RM_Release(vlc_frame); }

        int status = PV_Get(stamp, frame, timestamp, translated_stamp);
        if (status < 0)
        {
            stamp++;
        }
        else if (status == 0)
        {
            stamp++;
            PV_Release(frame);
        }
        else
        {
        }

        int64_t end = GetCurrentQPCTimestamp();
        if ((end - start) > (60 * 10000000))
        {
            start = end;
            pv_status = !pv_status;
            ShowMessage("Switch state to %d at stamp %lld", (int)pv_status, stamp);
            
            RM_SetEnable(0, pv_status);
            RM_SetEnable(1, pv_status);
            RM_SetEnable(2, pv_status);
            RM_SetEnable(3, pv_status);
            RM_SetEnable(4, pv_status);
            RM_SetEnable(5, pv_status);
            RM_SetEnable(6, pv_status);
            RM_SetEnable(7, pv_status);
            RM_SetEnable(8, pv_status);

            PV_SetEnable(pv_status);
            MC_SetEnable(pv_status);
            SI_SetEnable(pv_status);
            EE_SetEnable(pv_status);
            EA_SetEnable(pv_status);
            EV_SetEnable(pv_status);
        }
        }

		CoreApplication::Exit();
    }

    void OnWindowClosed(CoreWindow const& sender, CoreWindowEventArgs const& args)
    {
        (void)sender;
        (void)args;

        m_windowClosed = true;
    }    
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    CoreApplication::Run(winrt::make<App>());
}
