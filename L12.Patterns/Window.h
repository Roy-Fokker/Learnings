#pragma once

#include <string>
#include <cstdint>
#include <functional>

#include <Windows.h>
#include <atlbase.h>
#include <atlwin.h>

namespace Learnings
{
	class Window : public CWindowImpl<Window>
	{
	public:
		enum class Style
		{
			Windowed,
			Borderless,
			Fullscreen
		};

		enum class Message
		{
			Close
		};

		struct Desc
		{
			uint16_t width;
			uint16_t height;

			std::wstring title;

			Style style;
		};

		typedef std::function<bool(Message, uint16_t, uint16_t)> Callback;

	public:
		Window(const Desc &desc, const Callback &callback);
		~Window();

		void Show();
		void Restyle(Style style);
		void Resize(uint16_t width, uint16_t height);

		void Update();

	private:
		Callback m_Callback;

		bool m_IsActive;
		bool m_IsFullscreen;

#pragma region ATL Window Specific
	public:
		BEGIN_MSG_MAP(Window)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		END_MSG_MAP()

	private:
		LRESULT OnClose(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
#pragma endregion

	};
};