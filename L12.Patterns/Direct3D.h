#pragma once

#include <cstdint>

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <atlbase.h>

#include "Services.h"

namespace Learnings
{
	class Direct3D : public Service
	{
	public:
		Direct3D(HWND hWnd);
		~Direct3D();

		void Resize(uint16_t width, uint16_t height);

	private:
		HWND m_hWnd;
	};
}