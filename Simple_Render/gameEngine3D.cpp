#include "oldConsoleGameEngine.h"
#include "utils.h"
#include <iostream>
#include <algorithm>

using namespace std;


class gameEngine3D : public olcConsoleGameEngine
{
public:
	gameEngine3D()
	{
		m_sAppName = L"3D Rendering Engine";
	}

private:
	mesh meshDemo;
	mat4x4 matProj;
	vec3d vCamera; // Simplified version of a camera
	vec3d light_direction; // Simple directional light source
	float fTheta = 0;


public:
	bool OnUserCreate() override
	{
		// Creating a simple unit cube (sides length = 1)
		//vec3d origin = CreateVector(0, 0, 0);
		//vec3d size = CreateVector(1, 1, 1);
		//meshDemo = CreateCuboidMesh(origin, size);

		// Loading a .obj file
		meshDemo.LoadFromObjFile("assets/axis.obj");

		// Creating Projection Matrix

		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();

		matProj = CreateProjectMatrix(fFov, fAspectRatio, fNear, fFar);

		light_direction = { 0.0f, 0.0f, -1.0f }; 
		NormalizeVector(light_direction);

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		
		mat4x4 matRotZ, matRotX;
		//fTheta += 1.0f * elapsedTime;

		// Rotate in Z
		matRotZ = CreateRotationMatrixZ(fTheta);

		// Rotate in X
		matRotX = CreateRotationMatrixX(fTheta * 2.0f);

		// Translate matrix
		mat4x4 matTrans = CreateTranslationMatrix(0.0f, 0.0f, 18.0f);

		mat4x4 matWorld;
		matWorld = CreateIdentityMatrix();
		matWorld = matRotZ * matRotX;
		matWorld = matWorld * matTrans;

		// Defining our UP vector
		vec3d vUp = { 0, 1, 0 };

		vector<triangle> vecTriangleToRaster;

		mat4x4 matRotZX = matRotZ * matRotX;

		for (auto tri : meshDemo.tris)
		{
			triangle triProjected, triTransformed;

			MultiplyTriangleMatrix(tri, triTransformed, matWorld);

			vec3d normal = GetTriangleNormal(triTransformed);
			vec3d vCameraRay = triTransformed.p[0] - vCamera;
			if(ComputeDotProduct(normal, vCameraRay) < 0.0f)
			{
				float light_dp = ComputeDotProduct(normal, light_direction);

				CHAR_INFO c = GetColour(light_dp);
				triTransformed.col = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				MultiplyTriangleMatrix(triTransformed, triProjected, matProj);
				triProjected.col = c.Attributes;
				triProjected.sym = c.Char.UnicodeChar;

				ScaleToScreenSize(triProjected, (float)ScreenWidth(), (float)ScreenHeight());

				vecTriangleToRaster.push_back(triProjected);
			}
		}

		sort(vecTriangleToRaster.begin(), vecTriangleToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});

		for (auto& triProjected : vecTriangleToRaster)
		{
			// Renders the triangle
			FillTriangle(triProjected.p[0].x, triProjected.p[0].y, 
				triProjected.p[1].x, triProjected.p[1].y,
				triProjected.p[2].x, triProjected.p[2].y, 
				triProjected.sym, triProjected.col);

			// Keeping this method for debugging purposes
			// Allows the user to see the wireframe outline of triangles
			//DrawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y,
			//	triProjected.p[2].x, triProjected.p[2].y, PIXEL_QUARTER, FG_BLACK);
		}
		return true;
	}
};

int main()
{
	gameEngine3D engine;
	if (engine.ConstructConsole(256, 240, 4, 4))
		engine.Start();
}