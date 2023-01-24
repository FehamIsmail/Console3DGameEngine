#pragma once

#ifndef UTILS_H
#define UTILS_H

struct vec3d // 3D vector
{
	float x, y, z;
	vec3d operator+ (const vec3d& v)
	{
		vec3d result;
		result.x = v.x + this->x;
		result.y = v.y + this->y;
		result.z = v.z + this->z;
		return result;
	}

	vec3d operator* (const float& s)
	{
		vec3d result;
		result.x = s * this->x;
		result.y = s * this->y;
		result.z = s * this->z;
		return result;
	}

	vec3d operator+= (const float& s)
	{
		vec3d result;
		result.x += s;
		result.y += s;
		result.z += s;
		return result;
	}
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
