#include <functional>

#include "resource.h"

#include "Utility.h"

#include "Game.h"

#include "RenderTarget.h"
#include "Vertex.h"

using namespace Learnings;

Game::Game(const std::wstring &cmdLine)
	: m_Exit (false)/*,
	m_Services(new Services())*/
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

	m_GfxDev = std::make_unique<GraphicsDevice>(m_Window->m_hWnd);
	

	m_RT = std::make_unique<RenderTarget>(m_GfxDev->GetImmediateContext(),
										  m_GfxDev->CreateRenderTargetView(0),
										  m_GfxDev->CreateDepthStencilView(0),
										  m_GfxDev->GetViewportDesc());
	
}

Game::~Game()
{
}

int Learnings::Game::Run()
{
	Load();

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
			m_RT->ReleaseViewBuffers();

			m_GfxDev->Resize(lparam, wparam); // Resize swap chain

			m_RT->UpdateViewBuffers(m_GfxDev->CreateRenderTargetView(0),
									m_GfxDev->CreateDepthStencilView(0),
									m_GfxDev->GetViewportDesc());
			

			break;
		}
	}

	return false;
}

void Game::Load()
{
	// Vertex Shader and Input layout
	{
		auto vsf = ReadBinaryFile(L"VertexShader.cso");
		vs = m_GfxDev->CreateVertexShader(vsf);
		il = m_GfxDev->CreateInputLayout(VertexPositionTexture::ElementsDesc, vsf);
	}
	// Pixel Shader
	{
		auto psf = ReadBinaryFile(L"PixelShader.cso");
		ps = m_GfxDev->CreatePixelShader(psf);
	}
	// Texture/DDS file
	{
		auto texf = ReadBinaryFile(L"uv_grid.dds");
		srv = m_GfxDev->CreateShaderResourceView(texf);
	}
	// Rectangle mesh
	{
		float x = 0.5f, y = 0.5f;
		Mesh mesh{
			{
				{ { -x, -y, +0.5f },{ 0.0f, 1.0f } },
				{ { -x, +y, +0.5f },{ 0.0f, 0.0f } },
				{ { +x, +y, +0.5f },{ 1.0f, 0.0f } },
				{ { +x, -y, +0.5f },{ 1.0f, 1.0f } },
			},
			{
				0, 1, 2,
				0, 2, 3
			}
		};

		vb = m_GfxDev->CreateBuffer(mesh.vertices.data(),
									mesh.VertexListSize(),
									D3D11_BIND_VERTEX_BUFFER,
									D3D11_USAGE_DEFAULT,
									NULL);
		ib = m_GfxDev->CreateBuffer(mesh.indices.data(),
									mesh.IndexListSize(),
									D3D11_BIND_INDEX_BUFFER,
									D3D11_USAGE_DEFAULT,
									NULL);
		ic = (uint32_t)mesh.indices.size();
	}

	m_Window->Show();

	m_RT->SetStates(nullptr, nullptr, nullptr, nullptr);
}

void Game::Update()
{
	m_Window->Update();

	m_RT->SetShader(vs, ps);
	m_RT->SetInputType(il, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_RT->SetShaderResource({
		std::make_tuple(RenderTarget::Stage::Pixel, srv, 0)
	});
	m_RT->SetMeshData(vb, VertexPositionTexture::Size, ib);
}

void Game::Draw()
{
	std::array<float, 4u> color{ 0.75f, 0.5f, 0.25f, 1.0f };
	
	m_RT->Clear(color);

	m_RT->Draw(ic, 0, 0);

	m_GfxDev->Present(true);
}
