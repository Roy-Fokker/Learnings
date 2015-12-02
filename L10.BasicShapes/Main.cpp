#include <memory>
#include <iostream>
#include <fstream>
#include <Windows.h>

#include "Main.h"

#include "Window.h"
#include "Renderer.h"
#include "Mesh.h"
#include "BasicShapes.h"

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
		auto viewFrom = DirectX::XMMatrixLookAtLH({ {1.0f, -1.0f, 1.0f, 0.0f} },
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


	wnd = std::make_unique<Learnings::Window>(800, 500, L"L10. Basic Shapes", Learnings::Window::Style::Windowed, callback);
	rndr = std::make_unique<Learnings::Renderer>(wnd->m_hWnd);
	
	//auto shape = Learnings::Triangle(1.0f, 1.0f, 0.0f);
	//auto shape = Learnings::Rectangle(1.0f, 1.0f);
	//auto shape = Learnings::Box(1.0f, 1.0f, 1.0f);
	//auto shape = Learnings::Tetrahedron(1.0f);
	//auto shape = Learnings::Octahedron(1.0f);
	//auto shape = Learnings::Cylinder(0.5f, 0.5f, 1.0f, 10, true);
	//auto shape = Learnings::Sphere(0.5f, 120, 120);
	auto shape = Learnings::Icosahedron(1.0f, 2);
	uint32_t shapeIdx = 0;
	rndr->AddGeometry(shapeIdx, shape);

	auto grid = Learnings::Grid(0.5f, 10);
	uint32_t gridIdx = 1;
	rndr->AddGeometry(gridIdx, grid);
	rndr->SetTopology(gridIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
	auto ms = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	Learnings::Transform transform{ DirectX::XMMatrixTranspose(ms) };
	//rndr->SetTopology(shapeIdx, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	rndr->SetTransforms(shapeIdx, 0, transform);

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
		static float i = 0.0f;
		if (i > 360.0f) i = 0.0f;
		float a = DirectX::XMConvertToRadians(i);
		DirectX::XMFLOAT3 ra{ 0.0f, 1.0f, 0.0f };
		ms = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&ra), a);
		transform = { DirectX::XMMatrixTranspose(ms) };
		rndr->SetTransforms(shapeIdx, 0, transform);

		rndr->Draw();

		i += 0.01f;
	}

	CoUninitialize();
	return 0;
}