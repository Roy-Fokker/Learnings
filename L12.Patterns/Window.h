#pragma once

#include <string>
#include <cstdint>
#include <functional>
#include <array>

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
			Close,
			Pause,
			Resume,
			Resized
		};

		struct Desc
		{
			uint16_t width;
			uint16_t height;

			std::wstring title;

			Style style;

			int iconResId;
		};

		typedef std::function<bool(Message, uint16_t, uint16_t)> Callback;

	public:
		Window(const Desc &desc, const Callback &callback);
		~Window();

		void Show();

		void SetSize(uint16_t width, uint16_t height);
		std::array<uint16_t, 2> GetSize() const;
		void SetStyle(Style style);
		Style GetStyle() const;

		void Update();

	private:
		Callback m_Callback;

		bool m_IsActive;
		bool m_IsFullscreen;
		bool m_IsResizing;
		Style m_CurrentStyle;

#pragma region ATL Window Specific
	public:
		BEGIN_MSG_MAP(Window)
			MESSAGE_HANDLER(WM_CLOSE, OnClose)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_SYSCOMMAND, OnSystemCommand)
			MESSAGE_RANGE_HANDLER(WM_ENTERSIZEMOVE, WM_EXITSIZEMOVE, OnSizing)
			MESSAGE_HANDLER(WM_SIZE, OnResize)
			MESSAGE_HANDLER(WM_ACTIVATEAPP, OnActivate)
		END_MSG_MAP()

	private:
		LRESULT OnClose(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnDestroy(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnPaint(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnSystemCommand(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnSizing(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnResize(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnActivate(UINT msg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
#pragma endregion

	};
};