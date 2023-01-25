#include "oldConsoleGameEngine.h"
#include "utils.h"
#include <iostream>
#include <algorithm>

using namespace std;

struct player {
	vec3d vVelocity;
	float fMaxVelocity = 20.0f;
	float fAccelerationV = 0.1f; //Vertical Velocity
	float fAccelerationH = 1.0f; //Horizontal Velocity
	float fDecceleration = 0.0f;
};

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
	player p;
	float fYaw;
	float fPitch = 0;
	vec3d vCamera = { 0.0f, 10.0f, 0.0f }; // Simplified version of a camera
	vec3d vLookDir = { 0,0,1 }; // Camera's looking direction
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
		meshDemo.LoadFromObjFile("assets/mountains.obj");

		// Creating Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();

		matProj = CreateProjectMatrix(fFov, fAspectRatio, fNear, fFar);
		return true;
	}

	void UpdateCameraOnUserInput(vec3d& vCamera, float fElapsedTime)
	{
		if (GetKey(VK_SPACE).bHeld)
			vCamera.y += 8.0f * fElapsedTime;
		if (GetKey(VK_LSHIFT).bHeld)
			vCamera.y -= 8.0f * fElapsedTime;

		vec3d vForward = vLookDir * (8.0f * fElapsedTime);
		mat4x4 mRotateLeft = CreateRotationMatrixY(-3.14159265f/2.0f);
		mat4x4 mRotateRight = CreateRotationMatrixY(3.14159265f/2.0f);
		vec3d vForwardLeft;
		vec3d vForwardRight;
		MultiplyVectorMatrix(vForward, vForwardLeft, mRotateLeft);
		MultiplyVectorMatrix(vForward, vForwardRight, mRotateRight);
		vForwardLeft.y = 0;
		vForwardRight.y = 0;

		if (GetKey(L'W').bHeld)
			vCamera += vForward;
		if (GetKey(L'S').bHeld)
			vCamera -= vForward;
		if (GetKey(L'A').bHeld)
		{
			vCamera += vForwardLeft;
		}
		if (GetKey(L'D').bHeld)
		{
			vCamera += vForwardRight;
		}
		
		if (GetKey(VK_RIGHT).bHeld)
			fYaw += 2.0f * fElapsedTime;
		if (GetKey(VK_LEFT).bHeld)
			fYaw -= 2.0f * fElapsedTime;
		if (GetKey(VK_UP).bHeld)
			fPitch -= 1.0f * fElapsedTime;
		if (GetKey(VK_DOWN).bHeld)
			fPitch += 1.0f * fElapsedTime;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		UpdateCameraOnUserInput(vCamera, fElapsedTime);
		
		mat4x4 matRotZ, matRotX;
		//fTheta += 1.0f * fElapsedTime;

		// Rotate in Z
		matRotZ = CreateRotationMatrixZ(fTheta);

		// Rotate in X
		matRotX = CreateRotationMatrixX(fTheta * 2.0f);

		// Translate matrix
		mat4x4 matTrans = CreateTranslationMatrix(0.0f, 0.0f, 2.0f);

		mat4x4 matWorld;
		matWorld = CreateIdentityMatrix();
		matWorld = matRotZ * matRotX;
		matWorld = matTrans;

		// Creating Camera matrix
		vec3d vUp = { 0, 1, 0 };
		vec3d vTarget = { 0, 0, 1 };
		mat4x4 matCameraYawRot = CreateRotationMatrixY(fYaw);
		vec3d vCustomAxis = ComputeCrossProduct(vUp, vLookDir);
		NormalizeVector(vCustomAxis);

		//Update Camera properties
		mat4x4 matCameraPitchRot = CreateRotationMatrixAroundCustomAxis(fPitch, vCustomAxis);
		mat4x4 matCameraRot = matCameraYawRot * matCameraPitchRot;
		MultiplyVectorMatrix(vTarget, vLookDir, matCameraRot);
		vTarget = vCamera + vLookDir;
		
		mat4x4 matCamera = CreatePointAtMatrix(vCamera, vTarget, vUp);
		mat4x4 matView = ComputeQuickInverse(matCamera);


		vector<triangle> vecTrianglesToRaster;

		// Rendering view pipeline
		for (auto tri : meshDemo.tris)
		{
			triangle triProjected, triTransformed, triViewed;

			MultiplyTriangleMatrix(tri, triTransformed, matWorld);

			vec3d normal = GetTriangleNormal(triTransformed);
			NormalizeVector(normal);

			vec3d vCameraRay = triTransformed.p[0] - vCamera;
			if(ComputeDotProduct(normal, vCameraRay) < 0.0f)
			{
				// Light 
				light_direction = { 0.0f, 1.0f, -1.0f };
				NormalizeVector(light_direction);

				// Compute light intensity i.e how similar the normal vector is to the light's direction
				float light_dp = ComputeDotProduct(normal, light_direction);

				// Getting console colors
				CHAR_INFO c = GetColour(light_dp);
				triTransformed.col = c.Attributes;
				triTransformed.sym = c.Char.UnicodeChar;

				// Converting from World Space ==> View Space
				MultiplyTriangleMatrix(triTransformed, triViewed, matView);
				triViewed.col = c.Attributes;
				triViewed.sym = c.Char.UnicodeChar;

				// Clip view triangle against near plane
				int nClippedTriangles = 0;
				triangle clipped[2];
				nClippedTriangles = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.1f }, { 0.0f, 0.0f, 1.0f }, triViewed, clipped[0], clipped[1]);

				for (int n = 0; n < nClippedTriangles; n++)
				{
					// Projecting the View Space i.e convert 3D to 2D
					MultiplyTriangleMatrix(clipped[n], triProjected, matProj);
					triProjected.col = c.Attributes;
					triProjected.sym = c.Char.UnicodeChar;

					triProjected.p[0] = triProjected.p[0] * (1.0f / triProjected.p[0].w);
					triProjected.p[1] = triProjected.p[1] * (1.0f / triProjected.p[1].w);
					triProjected.p[2] = triProjected.p[2] * (1.0f / triProjected.p[2].w);

					ScaleToScreenSize(triProjected, (float)ScreenWidth(), (float)ScreenHeight());

					// Store triangle for sorting
					vecTrianglesToRaster.push_back(triProjected);
				}
			}
		}

		// Sorting the triangle to render what's left behind first
		sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](triangle& t1, triangle& t2)
		{
			float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
			float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
			return z1 > z2;
		});


		// Clear Screen
		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		
		for (auto& triToRaster : vecTrianglesToRaster)
		{
			// Clip triangles against all four screen edges
			triangle clipped[2];
			list<triangle> listTriangles;

			// Add initial triangle
			listTriangles.push_back(triToRaster);
			int nNewTriangles = 1;

			for (int p = 0; p < 4; p++)
			{
				int nTrisToAdd = 0;
				while (nNewTriangles > 0)
				{
					// Take triangle from front of queue
					triangle test = listTriangles.front();
					listTriangles.pop_front();
					nNewTriangles--;

					// Clipping it against a plane
					switch (p)
					{
					case 0:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 1:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, (float)ScreenHeight() - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 2:	nTrisToAdd = Triangle_ClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					case 3:	nTrisToAdd = Triangle_ClipAgainstPlane({ (float)ScreenWidth() - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
					}

					// Clipping may yield a variable number of triangles, so
					// add these new ones to the back of the queue for subsequent
					// clipping against next planes
					for (int w = 0; w < nTrisToAdd; w++)
						listTriangles.push_back(clipped[w]);
				}
				nNewTriangles = listTriangles.size();
			}


			// Rendering triangles
			for (auto& t : listTriangles)
			{
				FillTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, t.sym, t.col);
				//DrawTriangle(t.p[0].x, t.p[0].y, t.p[1].x, t.p[1].y, t.p[2].x, t.p[2].y, PIXEL_SOLID, FG_BLACK);
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