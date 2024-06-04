
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

        RM_InitializeDepthLock();

        m_init = true;
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();

        window.Activate();

        ev_captureformat cf;
        void* frame;
        uint64_t timestamp;
        int32_t framestamp;
        int32_t last_fs = -1;
        void const* buffer;
        int32_t length;
        void const* format_buffer;
        int32_t format_length;

        cf.vf.width = 1280;
        cf.vf.height = 720;
        cf.vf.framerate = 30;
        wcscpy_s(cf.vf.subtype, 5, L"YUY2");

        cf.mrcvo.shared = false;
        cf.mrcvo.global_opacity = 2; // index group
        cf.mrcvo.output_width = 0; // index source
        cf.mrcvo.output_height = 0; // index profile

        EV_SetFormat(cf);
        EV_Initialize(18);
        EV_SetEnable(true);

        while (!m_windowClosed)
		{
		window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        int v = EV_Get(-1, frame, timestamp, framestamp);
        if (v == 0)
        {
            if (last_fs < framestamp)
            {
                last_fs = framestamp;

                EV_Extract(frame, &buffer, &length, &format_buffer, &format_length);

                ShowMessage(L"GOT EV FRAME %d %lld - %d %d %d %s", framestamp, timestamp, ((uint16_t*)format_buffer)[0], ((uint16_t*)format_buffer)[1], ((uint8_t*)format_buffer)[4], (wchar_t*)(((uint16_t*)format_buffer) + 3));
            }

            EV_Release(frame);
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


/*
RM_Initialize(0, 30);
RM_Initialize(1, 30);
RM_Initialize(2, 30);
RM_Initialize(3, 30);

RM_Initialize(4, 45);
RM_Initialize(5, 5);

RM_Initialize(6, 12);
RM_Initialize(7, 24);
RM_Initialize(8, 5);
*/

//PV_Initialize(18); // MAX 18
//MC_Initialize(62); // Internal copies (no hard limit)
//SI_Initialize(30); // Internal copies (no hard limit)
//EE_Initialize(90); // Internal copies (no hard limit)

/*
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
*/
/*
        v_ref = PV_Get(-PV_BUF_SIZE, f_ref, t_ref, s_ref);
        if (v_ref == 0)
        {
            if (p_s_ref < s_ref)
            {
                PV_Extract(f_ref, buffer + 0, length + 0, buffer + 1, length + 1, buffer + 2, length + 2);

                p_s_ref = s_ref;

                v[0] = RM_Get(0, t_ref, 0, false, f[0], t[0], s[0]);
                v[1] = RM_Get(1, t_ref, 0, false, f[1], t[1], s[1]);
                v[2] = RM_Get(2, t_ref, 0, false, f[2], t[2], s[2]);
                v[3] = RM_Get(3, t_ref, 0, false, f[3], t[3], s[3]);
                v[4] = RM_Get(4, t_ref, 0, false, f[4], t[4], s[4]);
                v[5] = RM_Get(5, t_ref, 0, false, f[5], t[5], s[5]);
                v[6] = RM_Get(6, t_ref, 0, false, f[6], t[6], s[6]);
                v[7] = RM_Get(7, t_ref, 0, false, f[7], t[7], s[7]);
                v[8] = RM_Get(8, t_ref, 0, false, f[8], t[8], s[8]);
                v[9] = PV_Get(t_ref, 0, false, f[9], t[9], s[9]);
                v[10] = MC_Get(t_ref, 0, false, f[10], t[10], s[10]);
                v[11] = SI_Get(t_ref, 0, false, f[11], t[11], s[11]);
                v[12] = EE_Get(t_ref, 0, false, f[12], t[12], s[12]);

                for (int i = 0; i < 13; ++i)
                {
                    if (v[i] != 0) { continue; }

                    ShowMessage("GOT PAIR %d (%d, %d) at (%lld, %lld) delta: %lld ", i, s_ref, s[i], t_ref, t[i], (int64_t)t_ref - (int64_t)t[i]);

                    if (i <= 8) { RM_Release(f[i]); }
                    if (i == 9) { PV_Release(f[i]); }
                    if (i == 10) { MC_Release(f[i]); }
                    if (i == 11) { SI_Release(f[i]); }
                    if (i == 12) { EE_Release(f[i]); }
                }
            }

            PV_Release(f_ref);
        }
        */
        /*
        v_ref = RM_Get(RM_ID, -RM_BUF_SIZE, f_ref, t_ref, s_ref);
        if (v_ref == 0)
        {
            if (p_s_ref < s_ref)
            {
                switch (RM_ID)
                {
                case 0:
                case 1:
                case 2:
                case 3: RM_Extract_VLC(f_ref, buffer + 0, length + 0, buffer + 0, length + 0); break;
                case 4: RM_Extract_Depth_AHAT(f_ref, buffer + 0, length + 0, buffer + 0, length + 0, buffer + 0, length + 0); break;
                case 5: RM_Extract_Depth_Longthrow(f_ref, buffer + 0, length + 0, buffer + 0, length + 0, buffer + 0, length + 0, buffer + 0, length + 0); break;
                case 6: RM_Extract_IMU_Accelerometer(f_ref, buffer + 0, length + 0, buffer + 0, length + 0); break;
                case 7: RM_Extract_IMU_Gyroscope(f_ref, buffer + 0, length + 0, buffer + 0, length + 0); break;
                case 8: RM_Extract_IMU_Magnetometer(f_ref, buffer + 0, length + 0, buffer + 0, length + 0); break;
                }

                p_s_ref = s_ref;
                ShowMessage("GOT %d FRAME %d %lld", RM_ID, s_ref, t_ref);
            }

            RM_Release(f_ref);
        }
        */
        /*
                pv_captureformat pvcf;
                pvcf.mrcvo.enable = false;
                pvcf.mrcvo.shared = false;
                pvcf.vf.width = 640;
                pvcf.vf.height = 360;
                pvcf.vf.framerate = 30;

                PV_SetFormat(pvcf);
                MC_SetFormat(false);
                EE_SetFormat(2);

                int const RM_ID = 5;
                // 0-3: [, 8732) // MEM
                // 4:   [, 2652) // MEM
                // 5:   [18, 19) // SAME AS PV
                // 6:   [, ) // MEM
                // 7:   [, ) // MEM
                // 8:   [, ) // MEM
                int const RM_BUF_SIZE = 18;
                //int const PV_BUF_SIZE = 18; // [18,19)

                RM_Initialize(RM_ID, RM_BUF_SIZE);
                RM_SetEnable(RM_ID, true);

                //void* f[13];
                //uint64_t t[13];
                //int32_t s[13];
                //int v[13];
                void* f_ref;
                uint64_t t_ref;
                int32_t s_ref;
                int v_ref;
                int p_s_ref = -1;
                void const* buffer[4];
                int32_t length[4];
                */

                /*
                int v = EA_Get(-1, frame, timestamp, framestamp);
                if (v == 0)
                {
                    if (last_fs < framestamp)
                    {
                        last_fs = framestamp;
                        EA_Extract(frame, &buffer, &length, &format_buffer, &format_length);

                        ShowMessage(L"GOT EA %d %lld FRAME %f %d %d %d %d %d %s", framestamp, timestamp, *(float*)buffer, length, ((uint32_t*)format_buffer)[0], ((uint32_t*)format_buffer)[1], ((uint32_t*)format_buffer)[2], ((uint32_t*)format_buffer)[3], (wchar_t*)((uint32_t*)format_buffer + 4));
                    }

                    EA_Release(frame);
                }
                */
                /*
                        MRCAudioOptions cf;
                        void* frame;
                        uint64_t timestamp;
                        int32_t framestamp;
                        int32_t last_fs = -1;
                        void const* buffer;
                        void const* format_buffer;
                        int32_t length;
                        int32_t format_length;

                        cf.loopback_gain = 1.0;
                        cf.microphone_gain = 1.0;
                        cf.mixer_mode = 0 | (2 << 8) | (0 << 16);

                        EA_SetFormat(cf);
                        EA_Initialize(125);
                        EA_SetEnable(true);
                        */