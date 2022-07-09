// deklarasikan semua header disini
#include <windows.h>
#include <math.h>
#include <GL/glut.h>

// container untuk membuat tipe data 3D (X, Y, Z)
struct Vec3
{
	float X; float Y; float Z;
	Vec3(float x, float y, float z) { X = x; Y = y; Z = z; }
	//
	Vec3() { }
	~Vec3() { }
};

void markPoint(Vec3 points, Vec3 colors, float width);
bool inverse(float inMat[16], float outMat[16]);
void DotMatrix(float inMat1[16], float inMat2[4], float outMat[4]);

// menggambar setiap titik kontrol kurva
void markPoint(Vec3 points, Vec3 colors, float width)
{
	// tandai setiap titik dengan warna
	glPushMatrix();
	glColor3f(colors.X, colors.Y, colors.Z);

	glBegin(GL_QUADS);
	glVertex3f(points.X - width, points.Y - width, points.Z);
	glVertex3f(points.X + width, points.Y - width, points.Z);
	glVertex3f(points.X + width, points.Y + width, points.Z);
	glVertex3f(points.X - width, points.Y + width, points.Z);
	glEnd();

	glPopMatrix();
}

// fungsi untuk menghitung invers matriks ordo 4x4
bool inverse(float inMat[16], float outMat[16])
{
	float inv[16], det;
	int i;

	inv[0] = 
		inMat[5] * inMat[10] * inMat[15] -
		inMat[5] * inMat[11] * inMat[14] -
		inMat[9] * inMat[6] * inMat[15] +
		inMat[9] * inMat[7] * inMat[14] +
		inMat[13] * inMat[6] * inMat[11] -
		inMat[13] * inMat[7] * inMat[10];

	inv[4] = 
	   -inMat[4] * inMat[10] * inMat[15] +
		inMat[4] * inMat[11] * inMat[14] +
		inMat[8] * inMat[6] * inMat[15] -
		inMat[8] * inMat[7] * inMat[14] -
		inMat[12] * inMat[6] * inMat[11] +
		inMat[12] * inMat[7] * inMat[10];

	inv[8] = 
		inMat[4] * inMat[9] * inMat[15] -
		inMat[4] * inMat[11] * inMat[13] -
		inMat[8] * inMat[5] * inMat[15] +
		inMat[8] * inMat[7] * inMat[13] +
		inMat[12] * inMat[5] * inMat[11] -
		inMat[12] * inMat[7] * inMat[9];

	inv[12] = 
	   -inMat[4] * inMat[9] * inMat[14] +
		inMat[4] * inMat[10] * inMat[13] +
		inMat[8] * inMat[5] * inMat[14] -
		inMat[8] * inMat[6] * inMat[13] -
		inMat[12] * inMat[5] * inMat[10] +
		inMat[12] * inMat[6] * inMat[9];

	inv[1] = 
	   -inMat[1] * inMat[10] * inMat[15] +
		inMat[1] * inMat[11] * inMat[14] +
		inMat[9] * inMat[2] * inMat[15] -
		inMat[9] * inMat[3] * inMat[14] -
		inMat[13] * inMat[2] * inMat[11] +
		inMat[13] * inMat[3] * inMat[10];

	inv[5] = 
		inMat[0] * inMat[10] * inMat[15] -
		inMat[0] * inMat[11] * inMat[14] -
		inMat[8] * inMat[2] * inMat[15] +
		inMat[8] * inMat[3] * inMat[14] +
		inMat[12] * inMat[2] * inMat[11] -
		inMat[12] * inMat[3] * inMat[10];

	inv[9] = 
	   -inMat[0] * inMat[9] * inMat[15] +
		inMat[0] * inMat[11] * inMat[13] +
		inMat[8] * inMat[1] * inMat[15] -
		inMat[8] * inMat[3] * inMat[13] -
		inMat[12] * inMat[1] * inMat[11] +
		inMat[12] * inMat[3] * inMat[9];

	inv[13] = 
		inMat[0] * inMat[9] * inMat[14] -
		inMat[0] * inMat[10] * inMat[13] -
		inMat[8] * inMat[1] * inMat[14] +
		inMat[8] * inMat[2] * inMat[13] +
		inMat[12] * inMat[1] * inMat[10] -
		inMat[12] * inMat[2] * inMat[9];

	inv[2] = 
		inMat[1] * inMat[6] * inMat[15] -
		inMat[1] * inMat[7] * inMat[14] -
		inMat[5] * inMat[2] * inMat[15] +
		inMat[5] * inMat[3] * inMat[14] +
		inMat[13] * inMat[2] * inMat[7] -
		inMat[13] * inMat[3] * inMat[6];

	inv[6] = 
	   -inMat[0] * inMat[6] * inMat[15] +
		inMat[0] * inMat[7] * inMat[14] +
		inMat[4] * inMat[2] * inMat[15] -
		inMat[4] * inMat[3] * inMat[14] -
		inMat[12] * inMat[2] * inMat[7] +
		inMat[12] * inMat[3] * inMat[6];

	inv[10] = 
		inMat[0] * inMat[5] * inMat[15] -
		inMat[0] * inMat[7] * inMat[13] -
		inMat[4] * inMat[1] * inMat[15] +
		inMat[4] * inMat[3] * inMat[13] +
		inMat[12] * inMat[1] * inMat[7] -
		inMat[12] * inMat[3] * inMat[5];

	inv[14] = 
	   -inMat[0] * inMat[5] * inMat[14] +
		inMat[0] * inMat[6] * inMat[13] +
		inMat[4] * inMat[1] * inMat[14] -
		inMat[4] * inMat[2] * inMat[13] -
		inMat[12] * inMat[1] * inMat[6] +
		inMat[12] * inMat[2] * inMat[5];

	inv[3] = 
	   -inMat[1] * inMat[6] * inMat[11] +
		inMat[1] * inMat[7] * inMat[10] +
		inMat[5] * inMat[2] * inMat[11] -
		inMat[5] * inMat[3] * inMat[10] -
		inMat[9] * inMat[2] * inMat[7] +
		inMat[9] * inMat[3] * inMat[6];

	inv[7] = 
		inMat[0] * inMat[6] * inMat[11] -
		inMat[0] * inMat[7] * inMat[10] -
		inMat[4] * inMat[2] * inMat[11] +
		inMat[4] * inMat[3] * inMat[10] +
		inMat[8] * inMat[2] * inMat[7] -
		inMat[8] * inMat[3] * inMat[6];

	inv[11] = 
	   -inMat[0] * inMat[5] * inMat[11] +
		inMat[0] * inMat[7] * inMat[9] +
		inMat[4] * inMat[1] * inMat[11] -
		inMat[4] * inMat[3] * inMat[9] -
		inMat[8] * inMat[1] * inMat[7] +
		inMat[8] * inMat[3] * inMat[5];

	inv[15] = 
		inMat[0] * inMat[5] * inMat[10] -
		inMat[0] * inMat[6] * inMat[9] -
		inMat[4] * inMat[1] * inMat[10] +
		inMat[4] * inMat[2] * inMat[9] +
		inMat[8] * inMat[1] * inMat[6] -
		inMat[8] * inMat[2] * inMat[5];

	det = inMat[0] * inv[0] + inMat[1] * inv[4] + inMat[2] * inv[8] + inMat[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		outMat[i] = inv[i] * det;

	return true;
}

// fungsi untuk perkalian matriks 4x4 dengan 4x1
void DotMatrix(float inMat1[16], float inMat2[4], float outMat[4])
{
	outMat[0] = inMat1[0] * inMat2[0] + inMat1[1] * inMat2[1] +
		inMat1[2] * inMat2[2] + inMat1[3] * inMat2[3];
	outMat[1] = inMat1[4] * inMat2[0] + inMat1[5] * inMat2[1] +
		inMat1[6] * inMat2[2] + inMat1[7] * inMat2[3];
	outMat[2] = inMat1[8] * inMat2[0] + inMat1[9] * inMat2[1] +
		inMat1[10] * inMat2[2] + inMat1[11] * inMat2[3];
	outMat[3] = inMat1[12] * inMat2[0] + inMat1[13] * inMat2[1] +
		inMat1[14] * inMat2[2] + inMat1[15] * inMat2[3];
}
