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

	shape.vertices.push_back(Vertex{ {0.0f, radius, 0.0f}, {0.0f, 0.0f} });
	
	uint8_t basePointCnt = 3;
	float angle = Math::XM_2PI / basePointCnt; // number of point in base
	float theta = 0.0f;
	for (uint8_t i = 0; i < basePointCnt; i++)
	{
		float x, y, z;
		x = radius * std::cosf(theta); 
		y = radius * std::cosf(angle);
		z = radius * std::sinf(theta); 

		theta += angle;

		shape.vertices.push_back(Vertex{ {x, y, z}, {0.0f, 0.0f} });
	}

	// duplicate points for nicer UV map
	auto v0 = shape.vertices[0],
		v1 = shape.vertices[1];
	shape.vertices.insert(shape.vertices.end(), { v0, v1 });

	// Map UV -> |/\/\/|
	for (uint8_t i = 0; i < 6; i++)
	{
		float u, v = 0.0f;
		
		if (i % 2 == 0)
		{
			v = 1.0f;
		}

		u = i / 6.0f;
		
		shape.vertices[i].texCoord = { u, v };
	}

	shape.indices = {
		0, 2, 1,
		1, 2, 3,
		2, 4, 3, 
		3, 4, 5
	};

	return shape;
}

Mesh Learnings::Octahedron(float radius)
{
	return{
		{
			{ { radius, 0.0f, 0.0f },{ 0.0f, 0.5f } },
			{ { 0.0f, radius, 0.0f },{ 0.5f, 0.0f } },
			{ { 0.0f, 0.0f, radius },{ 0.33f, 0.5f } },
			{ { -radius, 0.0f, 0.0f },{ 0.66f, 0.5f } },
			{ { 0.0f, -radius, 0.0f },{ 0.5f, 1.0f } },
			{ { 0.0f, 0.0f, -radius },{ 1.0f, 0.5f } },
		},
		{
			// Top
			0, 1, 2,
			2, 1, 3,
			3, 1, 5,
			5, 1, 0,
			// Bottom
			2, 4, 0,
			3, 4, 2,
			5, 4, 3,
			0, 4, 5
		}
	};
}

Mesh Learnings::Icosahedron(float radius, uint16_t subdivide)
{
	Mesh shape;

	float sqrt5 = std::sqrtf(5.0f);
	float tau = (1.0f + sqrt5) / 2.0f;

	float edgeRadius = std::sqrtf(10.0f + (2.0f * sqrt5)) / (4.0f * tau);
	float a = (radius / edgeRadius) / 2.0f;
	float b = (radius / edgeRadius) / (2.0f * tau);

	shape.vertices = {
		{ {  0,  b, -a }, { 0.0f, 0.0f } },
		{ {  b,  a,  0 }, { 0.0f, 0.0f } },
		{ { -b,  a,  0 }, { 0.0f, 0.0f } },
		{ {  0,  b,  a }, { 0.0f, 0.0f } },
		{ {  0, -b,  a }, { 0.0f, 0.0f } },
		{ { -a,  0,  b }, { 0.0f, 0.0f } },
		{ {  0, -b, -a }, { 0.0f, 0.0f } },
		{ {  a,  0, -b }, { 0.0f, 0.0f } },
		{ {  a,  0,  b }, { 0.0f, 0.0f } },
		{ { -a,  0, -b }, { 0.0f, 0.0f } },
		{ {  b, -a,  0 }, { 0.0f, 0.0f } },
		{ { -b, -a,  0 }, { 0.0f, 0.0f } },
	};

	shape.indices = {
		1, 0,  2, 	 2,  3,  1, 	  4, 3,  5, 	  8,  3,  4,
		6, 0,  7, 	 9,  0,  6, 	 10, 4, 11, 	 11,  6, 10,
		5, 2,  9, 	 9, 11,  5, 	  7, 1,  8, 	  8, 10,  7,
		5, 3,  2,	 1,  3,  8, 	  2, 0,  9, 	  7,  0,  1,
		9, 6, 11,	10,  6,  7, 	 11, 4,  5, 	  8,  4, 10
	};


	for (auto &vtx : shape.vertices)
	{
		float theta = 0.5f + std::atan2f(vtx.position.y, vtx.position.x) / Math::XM_2PI;
		float phi = 0.5f - std::asinf(vtx.position.z) / Math::XM_PI;

		vtx.texCoord = { theta, phi };
	}



	return shape;
}

Mesh Learnings::Dodecahedron(float radius, uint16_t subdivide)
{
	return Mesh();
}

Mesh Learnings::Sphere(float radius, uint16_t slices, uint16_t stacks)
{
	Mesh shape;

	float r = radius;
	float dp = Math::XM_PI / stacks;
	float dt = Math::XM_2PI / slices;

	float du = 1.0f / slices;
	float dv = 1.0f / stacks;

	shape.vertices.push_back(Vertex{
		{0.0f, r, 0.0f}, { 0.0f, 0.0f }
	});

	for (uint32_t stack = 1; stack < stacks; stack++)
	{
		float v = dv * stack;
		float phi = stack * dp;

		for (uint32_t slice = 0; slice < slices; slice++)
		{
			float theta = slice * dt;

			float x, y, z;
			x = r * std::sinf(phi) * std::cosf(theta);
			y = r * std::cosf(phi);
			z = r * std::sinf(phi) * std::sinf(theta);

			
			float u;
			u = du * slice;

			shape.vertices.push_back(Vertex{
				{ x, y, z },{ u, v }
			});
		}
	}
	shape.vertices.push_back(Vertex{
		{ 0.0f, -r, 0.0f },{ 0.0f, 1.0f }
	});

	// Faces for North Pole
	for (uint32_t i = 1; i <= slices; i++)
	{
		shape.indices.insert(shape.indices.end(), {
			0, 
			(i == slices) ? 1 : i + 1,
			i
		});
	}

	// Faces for Stuff inbetween
	for (uint32_t j = 0; j < stacks - 2u; j++)
	{
		uint32_t offset = j * slices + 1;
		for (uint32_t i = 0; i < slices; i++)
		{
			bool lastSlice = !((offset + i) < ((j + 1) * slices));
			shape.indices.insert(shape.indices.end(), {
				offset + i,
				(lastSlice) ? offset : offset + i + 1,
				offset + i + slices
			});

			shape.indices.insert(shape.indices.end(), {
				offset + i + slices,
				(lastSlice) ? offset : offset + i + 1,
				(lastSlice) ? offset + slices : offset + i + 1 + slices
			});
		}
	}

	// Faces for South Pole
	uint32_t spIdx = shape.vertices.size() - 1;
	uint32_t spOffset = spIdx - slices - 1;
	for (uint32_t i = 1; i <= slices; i++)
	{
		uint32_t idx = spOffset + i;
		shape.indices.insert(shape.indices.end(), {
			spIdx,
			(i == slices) ? idx : idx,
			(i == slices) ? spOffset + 1 : idx + 1
		});
	}

	return shape;
}

Mesh Learnings::Cylinder(float radiusTop, float radiusBottom, float height, uint16_t slices, bool cap)
{
	Mesh shape;

	float h = height / 2.0f;
	float angle = Math::XM_2PI / slices;
	
	uint32_t cnt = 0;
	// Cap
	if (cap)
	{
		// Top
		for (uint16_t i = 0; i < slices; i++)
		{
			float x, y, z;
			float u, v;

			x = radiusTop * std::cosf(i * angle);
			z = radiusTop * std::sinf(i * angle);
			y = h;

			u = 0.25f * std::cosf(i * angle) + 0.25f;
			v = 0.25f * std::sinf(i * angle) + 0.25f;

			shape.vertices.push_back(Vertex{ { x, y, z },{ u, v } });
		}

		for (uint16_t i = 1; i < slices - 1; i++)
		{
			uint32_t n = i;
			shape.indices.insert(shape.indices.end(), {
				n + 1, n, 0
			});
		}
		

		cnt = shape.vertices.size();
		
		// Bottom
		for (uint16_t i = 0; i < slices; i++)
		{
			float x, y, z;
			float u, v;

			x = radiusBottom * std::cosf(i * angle);
			z = radiusBottom * std::sinf(i * angle);
			y = -h;

			u = 0.25f * std::cosf(i * angle) + 0.75f;
			v = 0.25f * std::sinf(i * angle) + 0.25f;

			shape.vertices.push_back(Vertex{ { x, y, z },{ u, v } });
		}

		for (uint16_t i = 1; i < slices - 1; i++)
		{
			uint32_t n = i + cnt;
			shape.indices.insert(shape.indices.end(), {
				cnt, n, n + 1
			});
		}

		cnt = shape.vertices.size();
	}

	// Body
	for (uint16_t i = 0; i <= slices; i++)
	{
		float x, y, z;
		float u = 1.0f / slices * i;
		float v = cap ? 0.5f : 0.0f;

		// Top
		x = radiusTop * std::cosf(i * angle);
		z = radiusTop * std::sinf(i * angle);
		y = h;

		shape.vertices.push_back(Vertex{ {x, y, z}, {u, v} });

		// Bottom
		x = radiusBottom * std::cosf(i * angle);
		z = radiusBottom * std::sinf(i * angle);
		y = -h;

		shape.vertices.push_back(Vertex{ { x, y, z },{ u, 1.0f } });
	}

	for (uint16_t i = 0; i < slices; i++)
	{
		uint32_t n = i * 2 + cnt;
		shape.indices.insert(shape.indices.end(), {
			n, n + 2, n + 1,
			n + 1, n + 2, n + 3
		});
	}

	return shape;
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
