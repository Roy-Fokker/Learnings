#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "Window.h"
#include "Services.h"

namespace Learnings
{
	class Game
	{
	public:
		Game(const std::wstring &cmdLine);
		~Game();

		int Run();

	private:
		bool WindowCallback(Window::Message msg, uint16_t lparam, uint16_t wparam);

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Services> m_Services;

		bool m_Exit;
	};

};