#include "oldConsoleGameEngine.h"
#include "utils.h"
#include <iostream>

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
		meshDemo.LoadFromObjFile("assets/monkey.obj");

		// Creating Projection Matrix

		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRation = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRation * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		light_direction = { 0.0f, 0.0f, -1.0f }; 
		NormalizeVector(light_direction);

		return true;
	}

	bool OnUserUpdate(float elapsedTime) override
	{
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);
		
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * elapsedTime;

		// Rotate in Z matrix
		matRotZ = CreateRotationMatrixZ(fTheta);

		// Rotate in X matrix
		matRotX = CreateRotationMatrixX(fTheta * 2.0f);

		// Translate to (0.5, 0.5, 0.5)
		vec3d trans = CreateVector(0, 0, -3);
		mat4x4 matTrans = CreateTranslationMatrix(trans);

		mat4x4 matRotZX = matRotZ * matRotX;

		for (auto tri : meshDemo.tris)
		{
			triangle triProjected, triTranslated, triRotatedZX;

			MultiplyTriangleMatrix(tri, triRotatedZX, matRotZX);

			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			vec3d normal = GetTriangleNormal(triTranslated);
			if(ComputeDotProduct(normal, (triTranslated.p[0] - vCamera)) < 0.0f)
			{
				float light_dp = ComputeDotProduct(normal, light_direction);

				CHAR_INFO c = GetColour(light_dp);
				triTranslated.col = c.Attributes;
				triTranslated.sym = c.Char.UnicodeChar;

				MultiplyTriangleMatrix(triTranslated, triProjected, matProj);
				triProjected.col = c.Attributes;
				triProjected.sym = c.Char.UnicodeChar;

				ScaleToScreenSize(triProjected, (float)ScreenWidth(), (float)ScreenHeight());

				FillTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y, triProjected.sym, triProjected.col);

				// Keeping this method for debugging purposes
				// Allows the user to see the wireframe outline of triangles
				//DrawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y,
				//	triProjected.p[2].x, triProjected.p[2].y, PIXEL_SOLID, FG_BLACK);
			}
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