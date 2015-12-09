#pragma once

#include <memory>
#include <vector>
#include <map>
#include "Direct3D.h"
#include "Direct2D.h"


namespace Learnings
{
	struct Mesh;
	struct Transform;
	struct Projection;

	struct RenderableMesh
	{
		Direct3d::Buffer vertexBuffer;
		Direct3d::Buffer indexBuffer;
		uint32_t indexCount;
		uint32_t id;
	};

	class Renderer
	{
	public:
		Renderer(HWND hWnd);
		~Renderer();

		void Draw();
		void Resize();

		void AddGeometry(uint32_t meshId, const Mesh &mesh);
		void AddShader(const std::vector<byte> &vs, const std::vector<byte> &ps);
		void AddTexture(const std::vector<byte> &tex);
		void SetTransforms(uint32_t meshId, uint32_t instanceId, const Transform &transform);
		void SetTopology(uint32_t meshId, D3D11_PRIMITIVE_TOPOLOGY topology);
		void SetProjection(const Projection &projection);

		void AddText(const std::wstring &text);

	private:
		void CreateStates();
		void DeleteStates();

	private:
		std::unique_ptr<Learnings::Direct3d> m_d3d;
		std::unique_ptr<Learnings::Direct2d> m_d2d;

		std::vector<RenderableMesh> m_Meshes;
		std::map<uint32_t, uint32_t> m_MeshIds;
		std::multimap<uint32_t, Direct3d::Buffer> m_TransformBuffer;
		std::map<uint32_t, D3D11_PRIMITIVE_TOPOLOGY> m_TopologyRules;

		Direct3d::Buffer m_ProjectionBuffer;

		Direct3d::VertexShader m_VertexShader;
		Direct3d::PixelShader m_PixelShader;
		Direct3d::InputLayout m_InputLayout;

		Direct3d::ShaderResourceView m_ShaderResourceView;

		Direct3d::BlendState m_BlendState;
		Direct3d::DepthStencilState m_DepthStencilState;
		Direct3d::RasterizerState m_RasterizerState;
		Direct3d::SamplerState m_SampleState;
	};
}