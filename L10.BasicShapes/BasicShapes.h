#pragma once

#include <cstdint>

namespace Learnings
{
	struct Mesh;

	Mesh Triangle(float base, float height, float tipOffset);
	Mesh Rectangle(float length, float width);
	Mesh Box(float length, float width, float height);
	Mesh Tetrahedron(float radius);
	Mesh Octahedron(float radius);
	Mesh Icosahedron(float radius, uint16_t subdivide);
	Mesh Dodecahedron(float radius, uint16_t subdivide);
	Mesh Sphere(float radius, uint16_t slices, uint16_t stacks);
	Mesh Cylinder(float radiusTop, float radiusBottom, float height, uint16_t slices, bool cap);
	Mesh Grid(float cellSize, uint16_t cellCount);
}