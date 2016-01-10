#pragma once
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <tuple>

#include "Direct3D.h"

namespace Learnings
{
	typedef uint32_t Key;
	typedef unsigned char byte;
	typedef std::vector<byte> Data;

#pragma region Shader Manager
	class ShaderManager
	{
	public:
		typedef std::tuple<GraphicsDevice::VertexShader, GraphicsDevice::PixelShader, GraphicsDevice::InputLayout> Shader;
	public:
		ShaderManager(GraphicsDevice *device);
		~ShaderManager();

		Key Add(const Data &vsf, const Data &psf, uint32_t vertexId);
		
		template <typename T>
		T Get(Key key) { return nullptr; };

		template<>
		GraphicsDevice::VertexShader Get(Key key);

		template<>
		GraphicsDevice::PixelShader Get(Key key);

		template<>
		GraphicsDevice::InputLayout Get(Key key);

	private:
		void CreateVertexShader(const Data &vsf);
		void CreateInputLayout(const Data &vsf, uint32_t vertexId);
		void CreatePixelShader(const Data &psf);

	private:
		GraphicsDevice *m_GfxDev;

		std::vector<GraphicsDevice::VertexShader> m_VertexShaders;
		std::vector<GraphicsDevice::PixelShader> m_PixelShaders;
		std::vector<GraphicsDevice::InputLayout> m_InputLayouts;

		typedef uint32_t VertexId;
		typedef uint32_t InputLayoutId;

		std::unordered_map<VertexId, InputLayoutId> m_Vertex_IL;
		std::unordered_map<Key, InputLayoutId> m_Shader_IL;

		Key m_NextKey;
	};

	template<>
	GraphicsDevice::VertexShader ShaderManager::Get(Key key)
	{
		return m_VertexShaders.at(key);
	}

	template<>
	GraphicsDevice::PixelShader ShaderManager::Get(Key key)
	{
		return m_PixelShaders.at(key);
	}

	template<>
	GraphicsDevice::InputLayout ShaderManager::Get(Key key)
	{
		auto ilId = m_Shader_IL.at(key);
		return m_InputLayouts.at(ilId);
	}
#pragma endregion


}

