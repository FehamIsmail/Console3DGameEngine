#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <strstream>

struct vec3d // 3D vector
{
	float x, y, z;
	vec3d operator+ (const vec3d& v)
	{
		return CreateVector(this->x + v.x, this->y + v.y, this->z + v.z);
	}

	vec3d operator- (const vec3d& v)
	{
		return CreateVector(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	vec3d operator* (const float& s)
	{
		return CreateVector(this->x * s, this->y * s, this->z * s);
	}

	vec3d operator+= (const float& s)
	{
		return CreateVector(this->x + s, this->y + s, this->z + s);
	}

private:
	vec3d CreateVector(float x, float y, float z)
	{
		vec3d v;
		v.x = x;
		v.y = y;
		v.z = z;
		return v;
	}
};

struct triangle // Simple triangle containing exactly 3 3D vectors
{
	vec3d p[3];

	// Storing the shading
	wchar_t sym;
	short col;
};

struct mesh // Object containing a vector of triangles
{
	std::vector<triangle> tris;

	bool LoadFromObjFile(std::string sFilename)
	{
		std::ifstream f(sFilename);
		if (!f.is_open())
			return false;

		// Local cache of verts
		std::vector<vec3d> verts;

		while (!f.eof())
		{
			char line[128];
			f.getline(line, 128);

			std::strstream s;
			s << line;

			char junk;

			if (line[0] == 'v')
			{
				vec3d v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}

			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		return true;
	};
};

struct mat4x4 //4x4 Matrix
{
	float m[4][4] = { 0 };

	mat4x4 operator+ (const mat4x4& m)
	{
		mat4x4 result;
		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				result.m[r][c] = this->m[r][c] + m.m[r][c];
			}
		}
		return result;
	}

	mat4x4 operator* (const mat4x4& m)
	{
		mat4x4 result;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				result.m[i][j] = 0.0f;
				for (int k = 0; k < 4; k++)
				{
					result.m[i][j] += this->m[i][k] * m.m[k][j];
				}
			}
		}
		return result;
	}
	
};

void NormalizeVector(vec3d& v)
{
	float l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	v.x /= l; v.y /= l; v.z /= l;
}

void MultiplyVectorMatrix(vec3d& i, vec3d& o, mat4x4& m)
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

void MultiplyTriangleMatrix(triangle& i, triangle& o, mat4x4& m)
{
	MultiplyVectorMatrix(i.p[0], o.p[0], m);
	MultiplyVectorMatrix(i.p[1], o.p[1], m);
	MultiplyVectorMatrix(i.p[2], o.p[2], m);
}

vec3d CreateVector(float x, float y, float z)
{
	vec3d v;
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

mat4x4 CreateTranslationMatrix(vec3d& t)
{
	mat4x4 m;
	m.m[0][0] = 1;
	m.m[0][3] = t.x;
	m.m[1][1] = 1;
	m.m[1][3] = t.y;
	m.m[2][2] = 1;
	m.m[2][3] = t.z;
	m.m[3][3] = 1;
	return m;
}

mat4x4 CreateRotationMatrixZ(float fTheta)
{
	mat4x4 m;
	m.m[0][0] = cosf(fTheta);
	m.m[0][1] = sinf(fTheta);
	m.m[1][0] = -sinf(fTheta);
	m.m[1][1] = cos(fTheta);
	m.m[2][2] = 1;
	m.m[3][3] = 1;
	return m;
}

mat4x4 CreateRotationMatrixX(float fTheta)
{
	mat4x4 m;
	m.m[0][0] = 1;
	m.m[1][1] = cosf(fTheta * 0.5f);
	m.m[1][2] = sinf(fTheta * 0.5f);
	m.m[2][1] = -sinf(fTheta * 0.5f);
	m.m[2][2] = cosf(fTheta * 0.5f);
	m.m[3][3] = 1;
	return m;
}

vec3d GetLineFromPoints(vec3d p1, vec3d p2)
{
	vec3d line;
	line.x = p2.x - p1.x;
	line.y = p2.y - p1.y;
	line.z = p2.z - p1.z;
	return line;
}

vec3d ComputeCrossProduct(vec3d v1, vec3d v2)
{
	vec3d result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float ComputeDotProduct(vec3d v1, vec3d v2)
{
	float result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

vec3d GetTriangleNormal(triangle tri)
{
	vec3d normal, line1, line2;
	line1 = GetLineFromPoints(tri.p[0], tri.p[1]);
	line2 = GetLineFromPoints(tri.p[0], tri.p[2]);
	normal = ComputeCrossProduct(line1, line2);
	NormalizeVector(normal);
	return normal;
}

mesh CreateCuboidMesh(vec3d& origin, vec3d& size)
{
	mesh m;
	m.tris = {
		// SOUTH
		{ origin.x, origin.y, origin.z,    origin.x, size.y, origin.z,    size.x, size.y, origin.z },
		{ origin.x, origin.y, origin.z,    size.x, size.y, origin.z,    size.x, origin.y, origin.z },

		// EAST                                                      
		{ size.x, origin.y, origin.z,    size.x, size.y, origin.z,    size.x, size.y, size.z },
		{ size.x, origin.y, origin.z,    size.x, size.y, size.z,    size.x, origin.y, size.z },

		// NORTH                                                     
		{ size.x, origin.y, size.z,    size.x, size.y, size.z,    origin.x, size.y, size.z },
		{ size.x, origin.y, size.z,    origin.x, size.y, size.z,    origin.x, origin.y, size.z },

		// WEST                                                      
		{ origin.x, origin.y, size.z,    origin.x, size.y, size.z,    origin.x, size.y, origin.z },
		{ origin.x, origin.y, size.z,    origin.x, size.y, origin.z,    origin.x, origin.y, origin.z },

		// TOP                                                       
		{ origin.x, size.y, origin.z,    origin.x, size.y, size.z,    size.x, size.y, size.z },
		{ origin.x, size.y, origin.z,    size.x, size.y, size.z,    size.x, size.y, origin.z },

		// BOTTOM                                                    
		{ size.x, origin.y, size.z,    origin.x, origin.y, size.z,    origin.x, origin.y, origin.z },
		{ size.x, origin.y, size.z,    origin.x, origin.y, origin.z,    size.x, origin.y, origin.z },
	};
	return m;
}

void ScaleToScreenSize(triangle& v, float width, float height)
{
	v.p[0].x += 1.0f; v.p[0].y += 1.0f;
	v.p[1].x += 1.0f; v.p[1].y += 1.0f;
	v.p[2].x += 1.0f; v.p[2].y += 1.0f;

	v.p[0].x *= 0.5f * width;
	v.p[1].x *= 0.5f * width;
	v.p[2].x *= 0.5f * width;
	v.p[0].y *= 0.5f * height;
	v.p[1].y *= 0.5f * height;
	v.p[2].y *= 0.5f * height;
}


#endif
