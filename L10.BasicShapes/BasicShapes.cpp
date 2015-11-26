#include <math.h>
#include <DirectXMath.h>


#include "BasicShapes.h"

#include "Mesh.h"

using namespace Learnings;
namespace Math = DirectX;

static Mesh SubDivideMesh(Mesh mesh, uint16_t level)
{
	if (level < 1)
	{
		return mesh;
	}

	if (level > 1)
	{
		mesh = SubDivideMesh(mesh, level - 1);
	}

	Mesh subdMesh;
	auto numTriangles = mesh.indices.size() / 3u;

	for (decltype(numTriangles) i = 0; i < numTriangles; i++)
	{
		Vertex v0 = mesh.vertices[mesh.indices[i * 3]],
			v1 = mesh.vertices[mesh.indices[i * 3 + 1]],
			v2 = mesh.vertices[mesh.indices[i * 3 + 2]];

		Vertex m0, m1, m2;
		m0 = {
			{
				0.5f * (v0.position.x + v1.position.x),
				0.5f * (v0.position.y + v1.position.y),
				0.5f * (v0.position.z + v1.position.z)
			},
			{
				0.5f * (v0.texCoord.x + v1.texCoord.x),
				0.5f * (v0.texCoord.y + v1.texCoord.y)
			}
		};

		m1 = {
			{
				0.5f * (v1.position.x + v2.position.x),
				0.5f * (v1.position.y + v2.position.y),
				0.5f * (v1.position.z + v2.position.z)
			},
			{
				0.5f * (v1.texCoord.x + v2.texCoord.x),
				0.5f * (v1.texCoord.y + v2.texCoord.y)
			}
		};

		m2 = {
			{
				0.5f * (v0.position.x + v2.position.x),
				0.5f * (v0.position.y + v2.position.y),
				0.5f * (v0.position.z + v2.position.z)
			},
			{
				0.5f * (v0.texCoord.x + v2.texCoord.x),
				0.5f * (v0.texCoord.y + v2.texCoord.y)
			}
		};

		subdMesh.vertices.insert(subdMesh.vertices.end(), {
			v0, v1, v2, m0, m1, m2
		});

		uint32_t n = i * 6u;
		subdMesh.indices.insert(subdMesh.indices.end(), {
			n + 0, n + 3, n + 5,
			n + 3, n + 4, n + 5,
			n + 5, n + 4, n + 2,
			n + 3, n + 1, n + 4
		});
	}

	return subdMesh;
}

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

	float a = 1.0f;
	float b = a * tau;

	float du = 5.5f; // number of points horizontally
	float dv = 3.0f; // number of points vertically
	/*
	float edgeRadius = std::sqrtf(10.0f + (2.0f * sqrt5)) / (4.0f * tau);
	float a = (1.0f / edgeRadius) / 2.0f;
	float b = (1.0f / edgeRadius) / (2.0f * tau);
	*/
	shape.vertices = {
		{ {-a,  b, 0}, {0.5f / du, 0.0f} }, // 0
		{ { a,  b, 0}, {2.0f / du, 1.0f / dv} }, // 1
		{ {-a, -b, 0}, {4.5f / du, 2.0f / dv} }, // 2
		{ { a, -b, 0}, {1.0f / du, 1.0f} }, // 3

		{ {0, -a,  b}, {0.5f / du, 2.0f / dv} }, // 4
		{ {0,  a,  b}, {1.0f / du, 1.0f / dv} }, // 5
		{ {0, -a, -b}, {3.5f / du, 2.0f / dv} }, // 6
		{ {0,  a, -b}, {3.0f / du, 1.0f / dv } }, // 7

		{ { b, 0, -a}, {2.5f / du, 2.0f / dv} }, // 8
		{ { b, 0,  a}, {1.5f / du, 2.0f / dv} }, // 9
		{ {-b, 0, -a}, {4.0f / du, 1.0f / dv } }, // 10
		{ {-b, 0,  a}, {0.0f, 1.0f / dv} }, // 11
		

		// duplicates
		{ { -a,  b,  0 }, { 1.5f / du, 0.0f } }, // 12 - 0 vertex
		{ { -a,  b,  0 }, { 2.5f / du, 0.0f } }, // 13 - 0 vertex
		{ { -a,  b,  0 }, { 3.5f / du, 0.0f } }, // 14 - 0 vertex
		{ { -a,  b,  0 }, { 4.5f / du, 0.0f } }, // 15 - 0 vertex
		{ { -b,  0,  a }, { 5.0f / du, 1.0f / dv } }, // 16 -  11 vertex
		{ {  0, -a,  b }, { 1.0f, 2.0f / dv } }, // 17 - 4 vertex
		{ {  a, -b,  0 }, { 2.0f / du, 1.0f } }, // 18 - 3 vertex
		{ {  a, -b,  0 }, { 3.0f / du, 1.0f } }, // 19 - 3 vertex
		{ {  a, -b,  0 }, { 4.0f / du, 1.0f } }, // 20 - 3 vertex
		{ {  a, -b,  0 }, { 5.0f / du, 1.0f } }, // 21 - 3 vertex
		/*
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
		*/
	};

	shape.indices = {
		 0, 11,  5,
		12,  5,  1,  // 0 5 1
		13,  1,  7,  // 0 1 7
		14,  7, 10,  // 0 7 10
		15, 10, 16,  // 0 10 11

		 1,  5, 9,
		 5, 11, 4, 
		16, 10, 2, // 11 10 2
		10,  7, 6,
		 7,  1, 8,

		3,  9, 4, // 3, 9, 4,
		21, 17, 2, // 3, 4, 2,
		20,  2, 6, // 3, 2, 6,
		19,  6, 8, // 3, 6, 8,
		18,  8, 9, // 3, 8, 9,

		4, 9, 5, 
		2, 4, 16, // 2 4 11
		6, 2, 10,
		8, 6, 7,
		9, 8, 1
		 /*
		 1,  0,  2,
		 2,  3,  1,
		 4,  3,  5,
		 8,  3,  4,
		 6,  0,  7,
		 9,  0,  6, 
		10,  4, 11,
		11,  6, 10,
		 5,  2,  9,
		 9, 11,  5, 
		 7,  1,  8,
		 8, 10,  7,
		 5,  3,  2,
		 1,  3,  8,
		 2,  0,  9,
		 7,  0,  1,
		 9,  6, 11,
		10,  6,  7,
		11,  4,  5,
		 8,  4, 10
		 */
	};

	shape = SubDivideMesh(shape, subdivide);


	using namespace Math;
	for (auto &vtx : shape.vertices)
	{
		/*
		float theta = 1.0f - std::atan2f(vtx.position.x, vtx.position.z) / XM_2PI;
		float phi = std::acosf(vtx.position.y) / XM_PI;

		vtx.texCoord = { theta, phi };
		*/

		XMVECTOR n = Math::XMVector3Normalize(XMLoadFloat3(&vtx.position));
		XMVECTOR p = radius * n;

		XMStoreFloat3(&vtx.position, p);
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
