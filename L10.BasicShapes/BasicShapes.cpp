#include "BasicShapes.h"

#include "Mesh.h"

using namespace Learnings;

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
	float x1 = -l, y1 = -w,
		x2 = l, y2 = -w,
		x3 = l, y3 = w,
		x4 = -l, y4 = w;

	return{
		// Vertex List
		{
			{ { x1, y1, 0.0f },{ 0.0f, 0.0f } },
			{ { x2, y2, 0.0f },{ 1.0f, 0.0f } },
			{ { x3, y3, 0.0f },{ 1.0f, 1.0f } },
			{ { x4, y4, 0.0f },{ 0.0f, 1.0f } },
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
	return Mesh();
}

Mesh Learnings::Tetrahedron(float radius)
{
	return Mesh();
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
