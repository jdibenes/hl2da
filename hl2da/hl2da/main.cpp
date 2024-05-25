
#include <windows.h>
#include "research_mode.h"
#include "locator.h"
#include "stream_rm.h"
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
        RM_Initialize(0, 30);
        RM_SetEnable(0, true);

        m_init = true;
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();
        rm_frame* f;
        uint64_t t;
        int32_t s;
        int32_t index = 0;

        window.Activate();

        while (!m_windowClosed)
		{
		window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

        memset(&f, 0, sizeof(f));
        int v = RM_Get(0, index, f, t, s);
        if (v == 0)
        {
            rm_data_vlc d;
            d.buffer = 0;
            d.length = 0;
            RM_Extract(f->rmsf, d);

            ULONG count = f->Release();
            ShowMessage("RM_Get %d %lld, %d, (%d) %p", v, t, s, count, d.buffer);
            index++;
        }
        else
        {
            //ShowMessage("RM_Get %d", v);
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
