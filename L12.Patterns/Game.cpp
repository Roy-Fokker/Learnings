#include <functional>

#include "resource.h"

#include "Game.h"



using namespace Learnings;

Game::Game(const std::wstring &cmdLine)
	: m_Exit (false)
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
	}

	return false;
}
