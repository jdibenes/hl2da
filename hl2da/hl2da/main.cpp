
#include <windows.h>
#include "research_mode.h"
#include "personal_video.h"
#include "locator.h"
#include "stream_rm.h"
#include "stream_pv.h"
#include "stream_mc.h"
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

        RM_InitializeDepthLock();

        m_init = true;
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();

        window.Activate();

        MC_Initialize(20);
        MC_SetFormat(false);
        MC_SetEnable(true);

        mc_frame* f;
        uint64_t t;
        int32_t s;
        int32_t ps = -1;

        while (!m_windowClosed)
		{
		window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        int v = MC_Get(-1, f, t, s);
        if (v == 0)
        {
            if (ps < s)
            { 
                ps = s;
                ShowMessage("GOT MC FRAME %lld %p %d %d", t, f->buffer, f->length, s);
            }
            
            f->Release();
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
