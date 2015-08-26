#include <memory>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "Main.h"

#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"

std::vector<byte> ReadBinaryFile(const std::wstring &fileName)
{
	std::vector<byte> buffer;

	std::ifstream inFile(fileName, std::ios::in | std::ios::binary);

	if (!inFile.is_open())
	{
		std::runtime_error("Cannot open file");
	}

	buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

	return buffer;
}

Learnings::Mesh TriangleMesh (float base, float height, float delta)
{
	float halfHeight = height / 2.0f;
	float halfBase = base / 2.0f;

	float x1 = -halfBase, y1 = -halfHeight,
		x3 = base * delta, y3 = halfHeight,
		x2 = halfBase, y2 = y1;

	float oset = 0.5f + delta;
	return {
		// Vertex List
		{
			{ { x1, y1, +0.5f }, { 0.0f, 1.0f } },
			{ { x3, y3, +0.5f }, { oset, 0.0f } },
			{ { x2, y2, +0.5f }, { 1.0f, 1.0f } },
		},

		// Index List
		{ 0, 1, 2 }
	};
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	std::unique_ptr<Learnings::Window> wnd;
	std::unique_ptr<Learnings::Renderer> rndr;

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
				rndr->Resize();
				break;
			case Window::Maximized:
				wnd->Restyle(Window::Style::Fullscreen);
				rndr->Resize();
				break;
			case Learnings::Window::Close:
				return true;
				break;
		}

		return false;
	};


	wnd = std::make_unique<Learnings::Window>(800, 500, L"L2.Direct 3D", Learnings::Window::Style::Windowed, callback);
	rndr = std::make_unique<Learnings::Renderer>(wnd->m_hWnd);
	

	auto triangle = TriangleMesh(1.0f, 1.0f, 0.0f);
	rndr->AddGeometry(triangle);

	auto vs = ReadBinaryFile(L"VertexShader.cso");
	auto ps = ReadBinaryFile(L"PixelShader.cso");
	rndr->AddShader(vs, ps);

	auto texture = ReadBinaryFile(L"uv_grid.dds");
	rndr->AddTexture(texture);

	wnd->Show(nCmdShow);
	// Main Loop
	while (wnd->ProcessMessages())
	{
		rndr->Draw();
	}

	CoUninitialize();
	return 0;
}