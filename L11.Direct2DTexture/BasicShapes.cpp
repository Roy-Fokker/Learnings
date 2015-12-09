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

		uint32_t n = static_cast<uint32_t>(i) * 6u;
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
			{ { x3, y3, 0.0f },{ oset, 0.0f } },
			{ { x2, y2, 0.0f },{ 1.0f, 1.0f } },
			{ { x1, y1, 0.0f },{ 0.0f, 1.0f } },
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
			{ { -l, +w, 0.0f },{ 0.0f, 0.0f } },
			{ { +l, +w, 0.0f },{ 1.0f, 0.0f } },
			{ { +l, -w, 0.0f },{ 1.0f, 1.0f } },
			{ { -l, -w, 0.0f },{ 0.0f, 1.0f } },
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
	
	float phi = 0, theta = 0;
	float dphi = Math::XM_PI / 3;
	float dtheta = Math::XM_2PI / 5;

	auto points = [&](int n) {
		for (int i = 0; i < n; i++)
		{
			float x, y, z;
			float u, v;

			x = radius * cos(theta) * sin(phi);
			z = radius * sin(theta) * sin(phi);
			y = radius * cos(phi);

			u = theta / Math::XM_2PI;
			v = phi / Math::XM_PI;


			theta += dtheta;
			shape.vertices.push_back({
				{ x, y, z },{ u, v }
			});

		}
	};

	// Pole
	phi = 0;
	theta = dtheta / 2.0f;
	points(5);

	// 1st Ring
	phi = dphi;
	theta = 0;
	points(6);
	
	// 2st Ring
	phi += dphi;
	theta = dtheta / 2.0f;
	points(6);
	
	// Pole
	phi += dphi;
	theta = dtheta;
	points(5);

	// Indices 
	for (int i = 5; i < 10; i++)
	{
		uint32_t n1, n2, n3, n4, n5, n6;
		n1 = i;
		n2 = i - 5;
		n3 = i + 1;
		n4 = i + 6;
		n5 = n4 + 1;
		n6 = n4 + 6;

		shape.indices.insert(shape.indices.end(), {
			n1, n2, n3,
			n1, n3, n4,
			n4, n3, n5,
			n4, n5, n6
		});
	}

	shape = SubDivideMesh(shape, subdivide);

	using namespace Math;
	for (auto &vtx : shape.vertices)
	{
		XMVECTOR n = Math::XMVector3Normalize(XMLoadFloat3(&vtx.position));
		XMVECTOR p = radius * n;

		XMStoreFloat3(&vtx.position, p);
	}


	return shape;
}

Mesh Learnings::Dodecahedron(float radius, uint16_t subdivide)
{
	Mesh shape;

	float phi = 0, theta = 0;
	float dphi_a = Math::XMConvertToRadians(52.62263590f);  // Where do these numbers come from?
	float dphi_b = Math::XMConvertToRadians(10.81231754f);  // Where do these numbers come from?
	float dtheta = Math::XM_2PI / 5;

	auto points = [&](int n) {
		for (int i = 0; i < n; i++)
		{
			float x, y, z;
			float u, v;

			x = radius * cos(theta) * cos(phi);
			z = radius * sin(theta) * cos(phi);
			y = radius * sin(phi);

			u = atan2(x, z) / (-Math::XM_2PI);
			if (u < 0.0f) {
				u += 1.0f;
			}
			v = asin(y) / Math::XM_PI + 0.5f;

			theta += dtheta;

			shape.vertices.push_back({
				{ x, y, z },{ u, v }
			});

		}
	};

	// 1st Ring
	phi = dphi_a;
	theta = 0;
	points(6);

	// 2nd Ring
	phi = dphi_b;
	theta = 0;
	points(6);

	// 3rd Ring
	phi = -dphi_b;
	theta = dtheta / 2.0f;
	points(6);

	// 4th Ring
	phi = -dphi_a;
	theta = dtheta / 2.0f;
	points(6);

	// Indices 
	shape.indices = {
		1, 0, 2, // North Cap Pentagon
		2, 0, 3, // North Cap Pentagon
		3, 0, 4, // North Cap Pentagon

		7, 0, 1,	 7, 6, 0,	 7, 12, 6, // Ring 1 Pentagons
		8, 1, 2,	 8, 7, 1,	 8, 13, 7, // Ring 1 Pentagons
		9, 2, 3,	 9, 8, 2,	 9, 14, 8, // Ring 1 Pentagons
		10, 3, 4,	 10, 9, 3,	 10, 15, 9, // Ring 1 Pentagons
		11, 4, 5,	 11, 10, 4,	 11, 16, 10, // Ring 1 Pentagons
		
		12, 7, 13,	12, 13, 18,	 18, 13, 19, // Ring 2 Pentagons
		13, 8, 14,	13, 14, 19,	 19, 14, 20, // Ring 2 Pentagons
		14, 9, 15,	14, 15, 20,	 20, 15, 21, // Ring 2 Pentagons
		15, 10, 16,	15, 16, 21,	 21, 16, 22, // Ring 2 Pentagons
		16, 11, 17,	16, 17, 22,	 22, 17, 23, // Ring 2 Pentagons
		
		18, 19, 20, // South Cap Pentagon
		18, 20, 21, // South Cap Pentagon
		18, 21, 22, // South Cap Pentagon
	};

	
	shape = SubDivideMesh(shape, subdivide);

	using namespace Math;
	for (auto &vtx : shape.vertices)
	{
		XMVECTOR n = Math::XMVector3Normalize(XMLoadFloat3(&vtx.position));
		XMVECTOR p = radius * n;

		XMStoreFloat3(&vtx.position, p);
	}
	

	return shape;
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
	uint32_t spIdx = static_cast<uint32_t>(shape.vertices.size() - 1);
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
		

		cnt = static_cast<uint32_t>(shape.vertices.size());
		
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

		cnt = static_cast<uint32_t>(shape.vertices.size());
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
