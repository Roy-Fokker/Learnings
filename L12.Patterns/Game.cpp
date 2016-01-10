#include <functional>

#include "resource.h"

#include "Utility.h"

#include "Game.h"

#include "RenderTarget.h"
#include "AssetManagers.h"

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
	m_RT->SetView();

	m_Shaders = std::make_unique<ShaderManager>(m_GfxDev.get());
	
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
			m_RT->SetView();
			

			break;
		}
	}

	return false;
}

void Game::Load()
{
	// Vertex Shader, Input layout, Pixel Shader
	{
		auto vsf = ReadBinaryFile(L"VertexShader.cso");
		auto psf = ReadBinaryFile(L"PixelShader.cso");
		
		m_ShaderKey = m_Shaders->Add(vsf, psf, VertexPositionTexture::Id);
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

}

void Game::Update()
{
	m_Window->Update();

	// Get Shader
	{
		auto vs = m_Shaders->Get<GraphicsDevice::VertexShader>(m_ShaderKey);
		auto ps = m_Shaders->Get<GraphicsDevice::PixelShader>(m_ShaderKey);;
		auto il = m_Shaders->Get<GraphicsDevice::InputLayout>(m_ShaderKey);;
		
		m_RT->SetShader(vs, ps);
		m_RT->SetInputType(il, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	m_RT->SetShaderResource({
		std::make_tuple(RenderTarget::Stage::Pixel, srv, 0)
	});
	m_RT->SetMeshData(vb, VertexPositionTexture::Size, ib);
	m_RT->SetStates(nullptr, nullptr, nullptr, nullptr);
	//m_RT->SetView();	// if we didn't do it earlier
}

void Game::Draw()
{
	std::array<float, 4u> color{ 0.75f, 0.5f, 0.25f, 1.0f };
	
	m_RT->Clear(color);

	m_RT->Draw(ic, 0, 0);

	m_GfxDev->Present(true);
}
