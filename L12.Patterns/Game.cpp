#include <functional>

#include "resource.h"

#include "Game.h"

#include "Services.h"
#include "Direct3D.h"

using namespace Learnings;

Game::Game(const std::wstring &cmdLine)
	: m_Exit (false),
	m_Services(new Services())
{
	Window::Desc desc{
		800, 600,
		L"L12.Patterns",
		Window::Style::Windowed,
		IDI_ICON1
	};

	using namespace std::placeholders;
	auto callback = std::bind(&Game::WindowCallback, this, _1, _2, _3);

	m_Window = std::make_unique<Window>(desc, callback);

	auto d3d = std::make_shared<Direct3D>(m_Window->m_hWnd);
	m_Services->Add(d3d);
}

Game::~Game()
{
}

int Learnings::Game::Run()
{
	m_Window->Show();

	while (!m_Exit)
	{
		m_Window->Update();
	}

	return 0;
}

bool Game::WindowCallback(Window::Message msg, uint16_t lparam, uint16_t wparam)
{
	if (!m_Window)
		return false;

	using WM = Window::Message;
	switch (msg)
	{
		case WM::Close:
			m_Exit = true;
			return true;
			break;
		case WM::Pause:
			OutputDebugStringA("Application is paused\n");
			break;
		case WM::Resume:
			OutputDebugStringA("Application has resumed\n");
			break;
		case WM::Resized:
			OutputDebugStringA("Application has resized - ");
			
			if (m_Window->GetStyle() == Window::Style::Fullscreen)
				OutputDebugStringA("\tFullscreen Mode\n");
			else if (m_Window->GetStyle() == Window::Style::Windowed)
				OutputDebugStringA("\tWindowed Mode\n");

			auto d3d = m_Services->Get<Direct3D>();
			if (d3d) d3d->Resize(lparam, wparam);

			break;
	}

	return false;
}
