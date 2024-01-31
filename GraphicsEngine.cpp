// GraphicsEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


struct vec3d {
	float x, y, z;
};

struct triangle {
	vec3d p[3];
};

struct mesh {
	std::vector<triangle> tris;
};

struct mat4x4 {
	float m[4][4] = { 0 };
};

class olcEngine3D : public olc::PixelGameEngine {
public:
	olcEngine3D() {
		sAppName = "3D Engine";
	}


private:
	mesh meshCube;
	mat4x4 matProj;

	float fTheta;

	void MultiplyMatrixVector(vec3d& i, vec3d& o, mat4x4& mat)
	{
		o.x = i.x * mat.m[0][0] + i.y * mat.m[1][0] + i.z * mat.m[2][0] + mat.m[3][0];
		o.y = i.x * mat.m[0][1] + i.y * mat.m[1][1] + i.z * mat.m[2][1] + mat.m[3][1];
		o.z = i.x * mat.m[0][2] + i.y * mat.m[1][2] + i.z * mat.m[2][2] + mat.m[3][2];
		float w = i.x * mat.m[0][3] + i.y * mat.m[1][3] + i.z * mat.m[2][3] + mat.m[3][3];

		if (w != 0.0f)
		{
			o.x /= w; 
			o.y /= w; 
			o.z /= w;
		}
	}

public:
	bool OnUserCreate() override
	{
		meshCube.tris = {

			//SOUTH
			{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

			//EAST
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

			//NORTH
			{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

			//WEST
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

			//TOP
			{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
			{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

			//BOTTOM
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
			{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		};

		//Projection Matrix
		float fNear = 0.1f;
		float fFar = 1000.0f;
		float fFov = 90.0f;
		float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
		float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

		matProj.m[0][0] = fAspectRatio * fFovRad;
		matProj.m[1][1] = fFovRad;
		matProj.m[2][2] = fFar / (fFar - fNear);
		matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
		matProj.m[2][3] = 1.0f;
		matProj.m[3][3] = 0.0f;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//Fill Screen to Black
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		//Set up Z and X rotation matrices
		mat4x4 matRotZ, matRotX;
		fTheta += 1.0f * fElapsedTime;

		//Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		//Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		//Draw Triangles
		for (auto t : meshCube.tris)
		{
			triangle tProjected, tTranslated, tRotatedZ, tRotatedZX;

			//Z-axis rotation
			MultiplyMatrixVector(t.p[0], tRotatedZ.p[0], matRotZ);
			MultiplyMatrixVector(t.p[1], tRotatedZ.p[1], matRotZ);
			MultiplyMatrixVector(t.p[2], tRotatedZ.p[2], matRotZ);

			//X-axis rotation
			MultiplyMatrixVector(tRotatedZ.p[0], tRotatedZX.p[0], matRotX);
			MultiplyMatrixVector(tRotatedZ.p[1], tRotatedZX.p[1], matRotX);
			MultiplyMatrixVector(tRotatedZ.p[2], tRotatedZX.p[2], matRotX);

			//Translate into screen
			tTranslated = tRotatedZX;
			tTranslated.p[0].z = tRotatedZX.p[0].z + 3.0f;
			tTranslated.p[1].z = tRotatedZX.p[1].z + 3.0f;
			tTranslated.p[2].z = tRotatedZX.p[2].z + 3.0f;

			//Project 3D Images into a 2D Space
			MultiplyMatrixVector(tTranslated.p[0], tProjected.p[0], matProj);
			MultiplyMatrixVector(tTranslated.p[1], tProjected.p[1], matProj);
			MultiplyMatrixVector(tTranslated.p[2], tProjected.p[2], matProj);

			//Scale object into view
			tProjected.p[0].x += 1.0f; tProjected.p[0].y += 1.0f;
			tProjected.p[1].x += 1.0f; tProjected.p[1].y += 1.0f;
			tProjected.p[2].x += 1.0f; tProjected.p[2].y += 1.0f;
			tProjected.p[0].x *= 0.5f * (float)ScreenWidth();
			tProjected.p[0].y *= 0.5f * (float)ScreenHeight();
			tProjected.p[1].x *= 0.5f * (float)ScreenWidth();
			tProjected.p[1].y *= 0.5f * (float)ScreenHeight();
			tProjected.p[2].x *= 0.5f * (float)ScreenWidth();
			tProjected.p[2].y *= 0.5f * (float)ScreenHeight();

			//Draw triangle
			DrawTriangle(tProjected.p[0].x, tProjected.p[0].y,
				tProjected.p[1].x, tProjected.p[1].y,
				tProjected.p[2].x, tProjected.p[2].y,
				olc::WHITE);

		}


		return true;
	}

};




int main()
{
	olcEngine3D demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}