#include <memory>

#include "Main.h"

#include "Window.h"
#include "Direct3D.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	std::unique_ptr<Learnings::Window> wnd;
	std::unique_ptr<Learnings::Direct3d> d3d;

	auto callback = [&](Learnings::Window::Message msg, uint16_t v1, uint16_t v2) -> bool
	{
		if (!wnd)
			return false;

		using namespace Learnings;

		switch (msg)
		{
			case Window::Activate:
				break;
			case Window::Deactivate:
				break;
			case Window::Pause:
				break;
			case Window::Resume:
				break;
			case Window::Resized:
				wnd->Restyle(Window::Style::Windowed);
				d3d->Resize();
				break;
			case Window::Maximized:
				wnd->Restyle(Window::Style::Fullscreen);
				d3d->Resize();
				break;
			case Learnings::Window::Close:
				return true;
				break;
		}

		return false;
	};


	wnd = std::make_unique<Learnings::Window>(800, 500, L"L2.Direct 3D", Learnings::Window::Style::Windowed, callback);

	d3d = std::make_unique<Learnings::Direct3d>(wnd->m_hWnd);

	wnd->Show(nCmdShow);
	// Main Loop
	while (wnd->ProcessMessages())
	{
		d3d->Draw();
	}

	CoUninitialize();
	return 0;
}