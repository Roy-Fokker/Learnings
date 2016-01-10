#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include "Window.h"
#include "Direct3D.h"

namespace Learnings
{
	class RenderTarget;
	class ShaderManager;

	class Game
	{
	public:
		Game(const std::wstring &cmdLine);
		~Game();

		int Run();

	private:
		bool WindowCallback(Window::Message msg, uint16_t lparam, uint16_t wparam);

		// Temporary
		void Load();
		void Update();
		void Draw();

		/*GraphicsDevice::VertexShader vs;
		GraphicsDevice::PixelShader ps;
		GraphicsDevice::InputLayout il;*/
		uint32_t m_ShaderKey;

		GraphicsDevice::ShaderResourceView srv;

		GraphicsDevice::Buffer vb, ib;
		uint32_t ic;

	private:
		std::unique_ptr<Window> m_Window;
		//std::unique_ptr<Services> m_Services;

		std::unique_ptr<GraphicsDevice> m_GfxDev;
		std::unique_ptr<RenderTarget> m_RT;

		std::unique_ptr<ShaderManager> m_Shaders;

		bool m_Exit;
	};

};