#pragma once

#ifndef UTILS_H
#define UTILS_H

struct vec3d // 3D vector
{
	float x, y, z;
};

struct triangle // Simple triangle containing exactly 3 3D vectors
{
	vec3d p[3];
};

struct mesh // Object containing a vector of triangles
{
	std::vector<triangle> tris;
};

struct mat4x4 //4x4 Matrix
{
	float m[4][4] = { 0 };
};

void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f)
	{
		o.x /= w; o.y /= w; o.z /= w;
	}
}



#endif
