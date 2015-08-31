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
			{ { x1, y1, 0.0f }, { 0.0f, 1.0f } },
			{ { x3, y3, 0.0f }, { oset, 0.0f } },
			{ { x2, y2, 0.0f }, { 1.0f, 1.0f } },
		},

		// Index List
		{ 0, 1, 2 }
	};
}

Learnings::Mesh GridMesh(float cellSize, uint32_t cellCount)
{
	using namespace Learnings;

	auto startxy = cellSize * cellCount / 2.0f;
	Vertex vStart{ {-startxy, 0.0f, -startxy}, {0.0f, 0.0f} };
	Vertex vEnd{ { -startxy, 0.0f, startxy },{ 0.0f, 0.0f } };

	Mesh grid;

	// X Direction
	for (uint32_t i = 0; i <= cellCount; i++)
	{
		uint32_t vertexCount = (uint32_t)grid.vertices.size();
		grid.indices.insert(grid.indices.end(), { vertexCount, vertexCount + 1 });

		Vertex v1 = vStart;
		Vertex v2 = vEnd;

		v1.position.x += i * cellSize;
		v2.position.x += i * cellSize;

		grid.vertices.insert(grid.vertices.end(), { v1, v2 });
	}

	vEnd = { { startxy, 0.0f, -startxy },{ 0.0f, 0.0f } };
	// Z Direction
	for (uint32_t i = 0; i <= cellCount; i++)
	{
		uint32_t vertexCount = (uint32_t)grid.vertices.size();
		grid.indices.insert(grid.indices.end(), { vertexCount, vertexCount + 1 });

		Vertex v1 = vStart;
		Vertex v2 = vEnd;

		v1.position.z += i * cellSize;
		v2.position.z += i * cellSize;

		grid.vertices.insert(grid.vertices.end(), { v1, v2 });
	}

	return grid;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);

	std::unique_ptr<Learnings::Window> wnd;
	std::unique_ptr<Learnings::Renderer> rndr;

	auto updateProjection = [&](float width, float height, float dFov) -> void
	{
		if (!rndr)
			return;

		float aspectRatio = width / height;
		float hFov = DirectX::XMConvertToRadians(dFov);
		float vFov = 2 * atan(tan(hFov / 2.0f) * aspectRatio);
		auto prespective = DirectX::XMMatrixPerspectiveFovLH(vFov, 
															aspectRatio, 
															0.1f, 
															1000.0f);
		auto viewFrom = DirectX::XMMatrixLookAtLH({ {1.0f, 1.0f, -1.0f, 0.0f} },
												  { {0.0f, 0.0f, 0.0f, 0.0f} },
												  { { 0.0f, 1.0f, 0.0f, 0.0f } });
		auto projection = viewFrom * prespective;
		Learnings::Projection proj{ DirectX::XMMatrixTranspose(projection) };
		rndr->SetProjection(proj);
	};

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
				updateProjection(v1, v2, 60.0f);
				break;
			case Window::Maximized:
				wnd->Restyle(Window::Style::Fullscreen);
				rndr->Resize();
				updateProjection(v1, v2, 60.0f);
				break;
			case Learnings::Window::Close:
				return true;
				break;
		}

		return false;
	};


	wnd = std::make_unique<Learnings::Window>(800, 500, L"L9. Grid", Learnings::Window::Style::Windowed, callback);
	rndr = std::make_unique<Learnings::Renderer>(wnd->m_hWnd);
	
	auto triangle = TriangleMesh(1.0f, 1.0f, 0.0f);
	uint32_t triangleIdx = 0;
	rndr->AddGeometry(triangleIdx, triangle);

	auto grid = GridMesh(0.5f, 10);
	uint32_t gridIdx = 1;
	rndr->AddGeometry(gridIdx, grid);
	rndr->SetTopology(gridIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	auto ms = DirectX::XMMatrixIdentity();
	ms = DirectX::XMMatrixRotationAxis({{0.0f, 0.0f, 1.0f}}, 
									   DirectX::XMConvertToRadians(90.0f));
	Learnings::Transform transform{ DirectX::XMMatrixTranspose(ms) };
	rndr->SetTransforms(triangleIdx, 0, transform);

	ms = DirectX::XMMatrixIdentity();
	ms = DirectX::XMMatrixRotationAxis({ { 0.0f, 0.0f, 1.0f } },
									   DirectX::XMConvertToRadians(-90.0f));
	transform = { DirectX::XMMatrixTranspose(ms) };
	rndr->SetTransforms(triangleIdx, 1, transform);

	ms = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	transform = { DirectX::XMMatrixTranspose(ms) };
	rndr->SetTransforms(gridIdx, 0, transform);

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