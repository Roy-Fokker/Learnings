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

	auto d3d = std::make_shared<GraphicsDevice>(m_Window->m_hWnd);
	m_Services->Add(d3d);
}

Game::~Game()
{
}

int Learnings::Game::Run()
{
	m_Window->Show();

	auto gfxDevice = m_Services->Get<GraphicsDevice>();

	while (!m_Exit)
	{
		m_Window->Update();



		gfxDevice->Present(true);
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
			auto d3d = m_Services->Get<GraphicsDevice>();
			if (d3d) d3d->Resize(lparam, wparam);

			break;
	}

	return false;
}
