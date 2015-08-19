#include "Main.h"

#include "Window.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	Learnings::Window wnd(800, 500, L"Test Window", Learnings::Window::Style::Windowed);

	wnd.Show(true);

	while (wnd.ProcessMessages())
	{

	}

	CoUninitialize();
	return 0;
}