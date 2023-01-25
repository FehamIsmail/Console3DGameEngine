#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <strstream>

struct vec3d // 3D vector
{
	float x, y, z, w;
	vec3d operator+ (const vec3d& v)
	{
		return CreateVector(this->x + v.x, this->y + v.y, this->z + v.z);
	}

	vec3d& operator+= (const vec3d& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	vec3d& operator-= (const vec3d& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		return *this;
	}

	vec3d& operator*= (const float s)
	{
		this->x *= s;
		this->y *= s;
		this->z *= s;
		return *this;
	}

	vec3d operator- (const vec3d& v)
	{
		return CreateVector(this->x - v.x, this->y - v.y, this->z - v.z);
	}

	vec3d operator* (const float& s)
	{
		return CreateVector(this->x * s, this->y * s, this->z * s);
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

bool CheckIfSameSign(float x1, float x2)
{
	return (x1 < 0.0f && x2 < 0.0f) || (x1 >= 0.0f && x2 >= 0.0f);
}

float GetVectorLength(vec3d& v)
{
	float l = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
	return l;
}

void NormalizeVector(vec3d& v)
{
	float l = GetVectorLength(v);
	v.x /= l; v.y /= l; v.z /= l;
}

void MultiplyVectorMatrix(vec3d& i, vec3d& o, mat4x4& m)
{
	o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	o.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
}

mat4x4 CreateIdentityMatrix()
{
	mat4x4 matrix;
	matrix.m[0][0] = 1.0f;
	matrix.m[1][1] = 1.0f;
	matrix.m[2][2] = 1.0f;
	matrix.m[3][3] = 1.0f;
	return matrix;
}

mat4x4 CreateProjectMatrix(float fFovDegrees, float fAspectRatio, float fNear, float fFar)
{
	float fFovRad = 1.0f / tanf(fFovDegrees * 0.5f / 180.0f * 3.14159f);
	mat4x4 matProj;
	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;
	return matProj;
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


mat4x4 CreateTranslationMatrix(float x, float y, float z)
{
	mat4x4 m;
	m.m[0][0] = 1;
	m.m[1][1] = 1;
	m.m[2][2] = 1;
	m.m[3][3] = 1;
	m.m[3][0] = x;
	m.m[3][1] = y;
	m.m[3][2] = z;
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

mat4x4 CreateRotationMatrixY(float fTheta)
{
	mat4x4 m;
	m.m[0][0] = cosf(fTheta);
	m.m[0][2] = sinf(fTheta);
	m.m[2][0] = -sinf(fTheta);
	m.m[1][1] = 1.0f;
	m.m[2][2] = cosf(fTheta);
	m.m[3][3] = 1.0f;
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

vec3d GetLineFromPoints(vec3d& p1, vec3d& p2)
{
	vec3d line;
	line.x = p2.x - p1.x;
	line.y = p2.y - p1.y;
	line.z = p2.z - p1.z;
	return line;
}

vec3d ComputeCrossProduct(vec3d& v1, vec3d& v2)
{
	vec3d result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

float ComputeDotProduct(vec3d& v1, vec3d& v2)
{
	float result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	return result;
}

vec3d GetTriangleNormal(triangle& tri)
{
	vec3d normal, line1, line2;
	line1 = GetLineFromPoints(tri.p[0], tri.p[1]);
	line2 = GetLineFromPoints(tri.p[0], tri.p[2]);
	normal = ComputeCrossProduct(line1, line2);
	NormalizeVector(normal);
	return normal;
}

mat4x4 CreatePointAtMatrix(vec3d& pos, vec3d& target, vec3d& up)
{
	// Calculate new forward direction
	vec3d newForward = target - pos;
	NormalizeVector(newForward);

	// Calculate new Up direction
	vec3d a = newForward * ComputeDotProduct(up, newForward);
	vec3d newUp = up - a;
	NormalizeVector(newUp);

	// New Right direction is easy, its just cross product
	vec3d newRight = ComputeCrossProduct(newUp, newForward);

	// Construct Dimensioning and Translation Matrix	
	mat4x4 matrix;
	matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;

}

vec3d Vector_IntersectPlane(vec3d& plane_p, vec3d& plane_n, vec3d& lineStart, vec3d& lineEnd)
{
	NormalizeVector(plane_n);
	float plane_d = -ComputeDotProduct(plane_n, plane_p);
	float ad = ComputeDotProduct(lineStart, plane_n);
	float bd = ComputeDotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	vec3d lineStartToEnd = lineEnd - lineStart;
	vec3d lineToIntersect = lineStartToEnd * t;
	return lineStart + lineToIntersect;
}

int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n, triangle& in_tri, triangle& out_tri1, triangle& out_tri2)
{
	// Make sure plane normal is indeed normal
	NormalizeVector(plane_n);

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](vec3d& p)
	{
		vec3d n;
		n.x = p.x;
		n.y = p.y;
		n.z = p.z;
		NormalizeVector(n);
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - ComputeDotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	vec3d* inside_points[3];  int nInsidePointCount = 0;
	vec3d* outside_points[3]; int nOutsidePointCount = 0;

	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[0]; }
	if (d1 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[1]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[1]; }
	if (d2 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[2]; }
	else { outside_points[nOutsidePointCount++] = &in_tri.p[2]; }

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle

		// Copy appearance info to new triangle
		out_tri1.col = in_tri.col;
		out_tri1.sym = in_tri.sym;

		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		out_tri1.p[1] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1]);

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles

		// Copy appearance info to new triangles
		out_tri1.col = in_tri.col;
		out_tri1.sym = in_tri.sym;

		out_tri2.col = in_tri.col;
		out_tri2.sym = in_tri.sym;

		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0]);

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.p[2] = Vector_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0]);

		return 2; // Return two newly formed triangles which form a quad
	}
}

mat4x4 ComputeQuickInverse(mat4x4& m) // Works only for Rotation and Translation Matrices and our Camera matrix
{
	mat4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
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
	vec3d vOffsetView = { 1.0f, 1.0f, 0.0f };
	
	// Inverting X and Y
	v.p[0].x *= -1.0f;
	v.p[1].x *= -1.0f;
	v.p[2].x *= -1.0f;
	v.p[0].y *= -1.0f;
	v.p[1].y *= -1.0f;
	v.p[2].y *= -1.0f;

	v.p[0] += vOffsetView;
	v.p[1] += vOffsetView;
	v.p[2] += vOffsetView;

	v.p[0].x *= 0.5f * width;
	v.p[1].x *= 0.5f * width;
	v.p[2].x *= 0.5f * width;
	v.p[0].y *= 0.5f * height;
	v.p[1].y *= 0.5f * height;
	v.p[2].y *= 0.5f * height;
}


#endif
