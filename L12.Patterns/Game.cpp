#include <functional>

#include "resource.h"

#include "Game.h"

#include "Services.h"
#include "Direct3D.h"
#include "RenderTarget.h"

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

	auto rt = std::make_shared<RenderTarget>(d3d->GetImmediateContext(),
											 d3d->CreateRenderTargetView(0),
											 d3d->CreateDepthStencilView(0),
											 d3d->GetViewportDesc());
	m_Services->Add(rt);
}

Game::~Game()
{
}

int Learnings::Game::Run()
{
	m_Window->Show();

	while (!m_Exit)
	{
		Update();

		Draw();
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
		{
			auto rt = m_Services->Get<RenderTarget>();

			rt->ReleaseViewBuffers();

			auto d3d = m_Services->Get<GraphicsDevice>();
			d3d->Resize(lparam, wparam); // Resize swap chain

			rt->UpdateViewBuffers(d3d->CreateRenderTargetView(0),
								  d3d->CreateDepthStencilView(0),
								  d3d->GetViewportDesc());
			

			break;
		}
	}

	return false;
}

void Learnings::Game::Update()
{
	m_Window->Update();

}

void Learnings::Game::Draw()
{
	std::array<float, 4u> color{ 0.75f, 0.5f, 0.25f, 1.0f };

	auto rt = m_Services->Get<RenderTarget>();
	rt->Clear(color);



	auto gfxDevice = m_Services->Get<GraphicsDevice>();
	gfxDevice->Present(true);
}
