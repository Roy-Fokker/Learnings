#pragma once

#include <memory>
#include <vector>
#include <map>
#include "Direct3D.h"

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
		void SetProjection(const Projection &projection);

	private:
		std::unique_ptr<Learnings::Direct3d> m_d3d;

		std::vector<RenderableMesh> m_Meshes;
		std::map<uint32_t, uint32_t> m_MeshIds;
		std::multimap<uint32_t, Direct3d::Buffer> m_TransformBuffer;

		Direct3d::Buffer m_ProjectionBuffer;

		Direct3d::VertexShader m_VertexShader;
		Direct3d::PixelShader m_PixelShader;
		Direct3d::InputLayout m_InputLayout;

		Direct3d::ShaderResourceView m_ShaderResourceView;
	};
}