#include <math.h>
#include <DirectXMath.h>


#include "BasicShapes.h"

#include "Mesh.h"

using namespace Learnings;
namespace Math = DirectX;

Mesh Learnings::Triangle(float base, float height, float tipOffset)
{
	float halfHeight = height / 2.0f;
	float halfBase = base / 2.0f;

	float x1 = -halfBase, y1 = -halfHeight,
		x3 = base * tipOffset, y3 = halfHeight,
		x2 = halfBase, y2 = y1;

	float oset = 0.5f + tipOffset;
	return{
		// Vertex List
		{
			{ { x1, y1, 0.0f },{ 0.0f, 1.0f } },
			{ { x2, y2, 0.0f },{ 1.0f, 1.0f } },
			{ { x3, y3, 0.0f },{ oset, 0.0f } },
		},

		// Index List
		{ 0, 1, 2 }
	};
}

Mesh Learnings::Rectangle(float length, float width)
{
	float l = length / 2.0f;
	float w = width / 2.0f;

	return{
		// Vertex List
		{
			{ { -l, -w, 0.0f },{ 0.0f, 0.0f } },
			{ { +l, -w, 0.0f },{ 1.0f, 0.0f } },
			{ { +l, +w, 0.0f },{ 1.0f, 1.0f } },
			{ { -l, +w, 0.0f },{ 0.0f, 1.0f } },
		},

		// Index List
		{ 
			0, 1, 2, 
			0, 2, 3
		}
	};
}

Mesh Learnings::Box(float length, float width, float height)
{
	float l = length / 2.0f;
	float w = width / 2.0f;
	float h = height / 2.0f;

	return{
		// Vertex List
		{
			// Front
			{ { -l, -w, +h },{ 0.0f, 0.0f } },
			{ { +l, -w, +h },{ 1.0f, 0.0f } },
			{ { +l, +w, +h },{ 1.0f, 1.0f } },
			{ { -l, +w, +h },{ 0.0f, 1.0f } },

			// Bottom
			{ { -l, -w, -h },{ 0.0f, 0.0f } },
			{ { +l, -w, -h },{ 1.0f, 0.0f } },
			{ { +l, -w, +h },{ 1.0f, 1.0f } },
			{ { -l, -w, +h },{ 0.0f, 1.0f } },

			// Right
			{ { +l, -w, -h },{ 0.0f, 0.0f } },
			{ { +l, +w, -h },{ 1.0f, 0.0f } },
			{ { +l, +w, +h },{ 1.0f, 1.0f } },
			{ { +l, -w, +h },{ 0.0f, 1.0f } },

			// Left
			{ { -l, -w, -h },{ 0.0f, 0.0f } },
			{ { -l, -w, +h },{ 1.0f, 0.0f } },
			{ { -l, +w, +h },{ 1.0f, 1.0f } },
			{ { -l, +w, -h },{ 0.0f, 1.0f } },

			// Back
			{ { -l, -w, -h },{ 0.0f, 0.0f } },
			{ { -l, +w, -h },{ 1.0f, 0.0f } },
			{ { +l, +w, -h },{ 1.0f, 1.0f } },
			{ { +l, -w, -h },{ 0.0f, 1.0f } },

			// Top
			{ { -l, +w, -h },{ 0.0f, 0.0f } },
			{ { -l, +w, +h },{ 1.0f, 0.0f } },
			{ { +l, +w, +h },{ 1.0f, 1.0f } },
			{ { +l, +w, -h },{ 0.0f, 1.0f } },
		},


		// Index List
		{
			// Front
			0, 1, 2, 0, 2, 3,
			// Bottom
			4, 5, 6, 4, 6, 7,
			// Right
			8, 9, 10, 8, 10, 11,
			// Left
			12, 13, 14, 12, 14, 15,
			// Back
			16, 17, 18, 16, 18, 19,
			// Top
			20, 21, 22, 20, 22, 23,
		}
	};
}

Mesh Learnings::Tetrahedron(float radius)
{
	Mesh shape;
	float angle = -19.471220333f; // Where does this come from??
	float phi = Math::XMConvertToRadians(angle);
	

	shape.vertices.resize(4);

	shape.vertices[0] = { {0.0f, radius, 0.0f}, {0.0f, 0.5f} };

	float theta = 0.0f;
	for (int i = 1; i < 4; i++)
	{
		float x, y, z, v;

		x = radius * std::cosf(theta) * std::cosf(phi);
		y = radius * std::sinf(phi);
		z = radius * std::sinf(theta) * std::cosf(phi);

		v = sinf(theta);

		shape.vertices[i] = {
			{ x, y, z }, {1.0f, v}
		};

		theta += Math::XMConvertToRadians(120.0f);
	}

	shape.indices = {
		0, 2, 1,
		0, 3, 2,
		0, 1, 3,
		1, 2, 3
	};

	return shape;
}

Mesh Learnings::Octahedron(float radius)
{
	return Mesh();
}

Mesh Learnings::Icosahedron(float radius, uint16_t subdivide)
{
	return Mesh();
}

Mesh Learnings::Dodecahedron(float radius, uint16_t subdivide)
{
	return Mesh();
}

Mesh Learnings::Sphere(float radius, uint16_t slices, uint16_t stacks)
{
	return Mesh();
}

Mesh Learnings::Cylinder(float radius, float height)
{
	return Mesh();
}

Mesh Learnings::Grid(float cellSize, uint16_t cellCount)
{
	auto startPos = cellSize * cellCount / 2.0f;

	Mesh grid;
	for (auto i = 0; i <= cellCount; i++)
	{
		uint32_t vertexIdx = (uint32_t)grid.vertices.size();
		grid.indices.insert(grid.indices.end(),
		{ 
			vertexIdx, vertexIdx + 1,		// X direction
			vertexIdx + 2, vertexIdx + 3	// Z direction
		});

		float xx1, xy1, xx2, xy2;	// X direction
		float zx1, zy1, zx2, zy2;	// Z direction

		xx1 = -startPos + (i * cellSize); xy1 = -startPos;
		xx2 = -startPos + (i * cellSize); xy2 = startPos;

		zx1 = -startPos; zy1 = -startPos + (i * cellSize);
		zx2 = startPos; zy2 = -startPos + (i * cellSize);

		grid.vertices.insert(grid.vertices.end(), 
		{
			{ { xx1, 0.0f, xy1 }, { 0.0f, 0.0f } }, 
			{ { xx2, 0.0f, xy2 }, { 0.0f, 0.0f } },

			{ { zx1, 0.0f, zy1 }, { 0.0f, 0.0f } }, 
			{ { zx2, 0.0f, zy2 }, { 0.0f, 0.0f } },
		});
	}

	return grid;
}
