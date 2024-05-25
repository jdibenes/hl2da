
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

        m_init = true;
    }

    void Run()
    {
        CoreWindow window = CoreWindow::GetForCurrentThread();

        window.Activate();

        while (!m_windowClosed)
		{
		window.Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
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
