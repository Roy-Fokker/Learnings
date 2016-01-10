#include "Utility.h"

#include "AssetManagers.h"

#include "Vertex.h"

using namespace Learnings;

#pragma region Shader Manager
ShaderManager::ShaderManager(GraphicsDevice *device)
	: m_NextKey(1),
	m_GfxDev(device)
{
	m_VertexShaders.push_back(nullptr);
	m_PixelShaders.push_back(nullptr);
	m_InputLayouts.push_back(nullptr);
}

ShaderManager::~ShaderManager()
{
}

Key ShaderManager::Add(const Data & vsf, const Data &psf, uint32_t vertexId)
{
	CreateVertexShader(vsf);
	CreateInputLayout(vsf, vertexId);
	CreatePixelShader(psf);

	return m_NextKey++;
}

void ShaderManager::CreateVertexShader(const Data &vsf)
{
	m_VertexShaders.push_back(m_GfxDev->CreateVertexShader(vsf));
}

void ShaderManager::CreateInputLayout(const Data &vsf, uint32_t vertexId)
{
	InputLayoutId ilId = (InputLayoutId) m_InputLayouts.size();

	auto it = m_Vertex_IL.find(vertexId);

	if (it == m_Vertex_IL.end())
	{
		GraphicsDevice::InputLayout il = nullptr;
		if (vertexId == VertexPositionTexture::Id)
		{
			il = m_GfxDev->CreateInputLayout(VertexPositionTexture::ElementsDesc, vsf);
		}

		if (il)
		{
			m_InputLayouts.push_back(il);
			m_Vertex_IL.insert({ vertexId, ilId });
		}
	}
	else
	{
		ilId = it->second;

		if (vertexId == VertexPositionTexture::Id)
		{
			auto isCorrect = m_GfxDev->CheckInputLayout(VertexPositionTexture::ElementsDesc, vsf);
			ThrowIfFailed(isCorrect, "Input layout doesn't match shader");
		}
	}

	m_Shader_IL.insert({ m_NextKey, ilId });
}

void ShaderManager::CreatePixelShader(const Data &psf)
{
	m_PixelShaders.push_back(m_GfxDev->CreatePixelShader(psf));
}

#pragma endregion

#pragma region Texture Manager

#pragma endregion
