// deklarasikan semua header disini
#include <windows.h>
#include <GL/glut.h>

// header untuk praktikum 07
#include "praktikum07.h"
#include <cmath>

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 480

// inisialisasi variabel untuk transformasi seperti translasi, rotasi atau scaling
float angle = 0.0f;					// sudut transformasi kamera
float posX = 0.0f, rotX = 0.0f;	// posisi kamera di sumbu X
float posY = 0.0f, rotY = 0.0f;	// posisi kamera di sumbu Y
float posZ = 5.0f, rotZ = -1.0f;	// posisi kamera di sumbu Z

float objectAngleX = 0.0f;			// sudut tranformasi obyek di sumbu X
float objectAngleY = 0.0f;			// sudut tranformasi obyek di sumbu Y
float objectAngleZ = 0.0f;			// sudut tranformasi obyek di sumbu Z

// fungsi untuk membuat kurva spline cubic dari 4 titik kontrol
// point1 sampai point4 = titik kontrol
// nPoint = jumlah titik interpolasi antara point1 sampai point4
void drawSplineCubic(Vec3 point1, Vec3 point2, Vec3 point3, Vec3 point4,
	int nPoint)
{
	// hitung bobot jarak u di masing-masing titik
	float utotal = (abs(point2.X - point1.X) + abs(point3.X - point2.X) +
		abs(point4.X - point3.X));
	float u1 = 0;
	float u2 = abs(point2.X - point1.X) / utotal;
	float u3 = (abs(point2.X - point1.X) + abs(point3.X - point2.X)) / utotal;
	float u4 = 1;
	// hitung inverse matriks dari koefisien u (lihat slide kuliah)
	float inverseMat[16];
	float coeffMat[16] = {
	1.00f, 0.00f, 0.00f, 0.00f,
	1.00f, u2, pow(u2, 2), pow(u2, 3),
	1.00f, u3, pow(u3, 2), pow(u3, 3),
	1.00f, 1.00f, 1.00f, 1.00f };
	bool status = inverse(coeffMat, inverseMat);
	// hitung koefisien cubic au^3 + bu^2 + cu + d
	if (status == true)
	{
		float outMatX[4], outMatY[4], outMatZ[4];
		float inMatX[4] = { point1.X, point2.X, point3.X, point4.X };
		float inMatY[4] = { point1.Y, point2.Y, point3.Y, point4.Y };
		float inMatZ[4] = { point1.Z, point2.Z, point3.Z, point4.Z };
		DotMatrix(inverseMat, inMatX, outMatX);
		DotMatrix(inverseMat, inMatY, outMatY);
		DotMatrix(inverseMat, inMatZ, outMatZ);
		// gambar kurva cubic spline dengan titik kontrol diatas hitung
		// posisi y untuk setiap x di setiap point dengan persamaan diatas
		for (int i = 0; i < nPoint; i++)
		{
			// jeda setiap titik pd bobot u
			float step = 1.0f / nPoint;
			// titik awal
			float pX = point1.X, pY = point1.Y, pZ = point1.Z;
			//
			float u = 0.0f;
			for (int i = 0; i < nPoint; i++)
			{
				// segment kurva cubic spline sebanyak nPoint
				u = u + step;
				glVertex3f(pX, pY, pZ); // gambar titik awal
				// koordinat X pada kurva
				pX = outMatX[3] * pow(u, 3) + outMatX[2] *
					pow(u, 2) + outMatX[1] * u + outMatX[0];
				// koordinat Y pada kurva
				pY = outMatY[3] * pow(u, 3) + outMatY[2] *
					pow(u, 2) + outMatY[1] * u + outMatY[0];
				// koordinat Z pada kurva
				pZ = outMatZ[3] * pow(u, 3) + outMatZ[2] *
					pow(u, 2) + outMatZ[1] * u + outMatZ[0];
				glVertex3f(pX, pY, pZ); // gambar titik akhir
			}
		}
	}
}

// fungsi untuk membuat kurva spline bezier dari 4 titik kontrol
// point1 dan point4 = titik kontrol awal dan akhir
// point2 dan point3 = titik kontrol pembentuk kurva
// nPoint = jumlah titik interpolasi antara point1 sampai point4
void drawSplineBezier(Vec3 point1, Vec3 point2, Vec3 point3,
	Vec3 point4, int nPoint)
{
	// hitung bobot jarak u di masing-masing titik
	float utotal = (abs(point2.X - point1.X) + abs(point3.X - point2.X) +
		abs(point4.X - point3.X));
	float u1 = 0;
	float u2 = abs(point2.X - point1.X) / utotal;
	float u3 = (abs(point2.X - point1.X) + abs(point3.X - point2.X)) /
		utotal;
	float u4 = 1;
	// hitung inverse matriks dari koefisien u (lihat slide kuliah)
	float inverseMat[16];
	float coeffMat[16] = {
	1.0000f, 0.0000f, 0.0000f, 0.0000f,
	1.0000f, 1.0000f, 1.0000f, 1.0000f,
	0.0000f, 1.0000f, 0.0000f, 0.0000f,
	0.0000f, 1.0000f, 2.0000f, 3.0000f };
	bool status = inverse(coeffMat, inverseMat);
	// hitung koefisien
	if (status == true)
	{
		float outMatX[4], outMatY[4], outMatZ[4];
		float inMatX[4] = { point1.X, point4.X,
		1.0f / (u2 - u1) * (point2.X - point1.X),
		1.0f / (u4 - u3) * (point4.X - point3.X) };
		float inMatY[4] = { point1.Y, point4.Y,
		1.0f / (u2 - u1) * (point2.Y - point1.Y),
		1.0f / (u4 - u3) * (point4.Y - point3.Y) };
		float inMatZ[4] = { point1.Z, point4.Z,
		1.0f / (u2 - u1) * (point2.Z - point1.Z),
		1.0f / (u4 - u3) * (point4.Z - point3.Z) };
		DotMatrix(inverseMat, inMatX, outMatX);
		DotMatrix(inverseMat, inMatY, outMatY);
		DotMatrix(inverseMat, inMatZ, outMatZ);
		// gambar kurva cubic spline dengan titik kontrol diatas hitung
		// posisi y untuk setiap x di setiap point dengan persamaan diatas
		for (int i = 0; i < nPoint; i++)
		{
			// jeda setiap titik pd bobot u
			float step = 1.0f / nPoint;
			// titik awal
			float pX = point1.X, pY = point1.Y, pZ = point1.Z;
			//
			float u = 0.0f;
			for (int i = 0; i < nPoint; i++)
			{
				// bentuk segment kurva spline sebanyak nPoint
				u = u + step;
				glVertex3f(pX, pY, pZ); // gambar titik awal
				// koordinat X pada kurva
				pX = outMatX[3] * pow(u, 3) + outMatX[2] *
					pow(u, 2) + outMatX[1] * u + outMatX[0];
				// koordinat Y pada kurva
				pY = outMatY[3] * pow(u, 3) + outMatY[2] *
					pow(u, 2) + outMatY[1] * u + outMatY[0];
				// koordinat Z pada kurva
				pZ = outMatZ[3] * pow(u, 3) + outMatZ[2] *
					pow(u, 2) + outMatZ[1] * u + outMatZ[0];
				glVertex3f(pX, pY, pZ); // gambar titik akhir
			}
		}
	}
}

// fungsi untuk membuat kurva spline catmull-rom dari 4 titik kontrol
// point1 dan point4 = titik kontrol awal dan akhir
// point2 dan point3 = titik kontrol pembentuk kurva
// nPoint = jumlah titik interpolasi antara point1 sampai point4
void drawSplineCatmullRom(Vec3 point1, Vec3 point2, Vec3 point3,
	Vec3 point4, int nPoint)
{
	// hitung bobot jarak u di masing-masing titik
	float utotal = (abs(point2.X - point1.X) + abs(point3.X - point2.X) +
		abs(point4.X - point3.X));
	float u1 = 0;
	float u2 = abs(point2.X - point1.X) / utotal;
	float u3 = (abs(point2.X - point1.X) + abs(point3.X - point2.X)) /
		utotal;
	float u4 = 1;
	// hitung inverse matriks dari koefisien u (lihat slide kuliah)
	float inverseMat[16];
	float coeffMat[16] = {
	1.0000f, 0.0000f, 0.0000f, 0.0000f,
	1.0000f, 1.0000f, 1.0000f, 1.0000f,
	0.0000f, 1.0000f, 0.0000f, 0.0000f,
	0.0000f, 1.0000f, 2.0000f, 3.0000f };
	bool status = inverse(coeffMat, inverseMat);
	// hitung koefisien
	if (status == true)
	{
		float outMatX[4], outMatY[4], outMatZ[4];
		float inMatX[4] = { point1.X, point4.X,
		1.0f / (u3 - u1) * (point3.X - point1.X),
		1.0f / (u4 - u2) * (point4.X - point2.X) };
		float inMatY[4] = { point1.Y, point4.Y,
		1.0f / (u3 - u1) * (point3.Y - point1.Y),
		1.0f / (u4 - u2) * (point4.Y - point2.Y) };
		float inMatZ[4] = { point1.Z, point4.Z,
		1.0f / (u3 - u1) * (point3.Z - point1.Z),
		1.0f / (u4 - u2) * (point4.Z - point2.Z) };
		DotMatrix(inverseMat, inMatX, outMatX);
		DotMatrix(inverseMat, inMatY, outMatY);
		DotMatrix(inverseMat, inMatZ, outMatZ);
		// gambar kurva spline dengan titik kontrol diatas hitung posisi
		// y untuk setiap x di setiap point dengan persamaan diatas
		for (int i = 0; i < nPoint; i++)
		{
			// jeda setiap titik pd bobot u
			float step = 1.0f / nPoint;
			// titik awal
			float pX = point1.X, pY = point1.Y, pZ = point1.Z;
			//
			float u = 0.0f;
			for (int i = 0; i < nPoint; i++)
			{
				// bentuk segment kurva spline sebanyak nPoint
				u = u + step;
				glVertex3f(pX, pY, pZ); // gambar titik awal
				// koordinat X pada kurva
				pX = outMatX[3] * pow(u, 3) + outMatX[2] *
					pow(u, 2) + outMatX[1] * u + outMatX[0];
				// koordinat Y pada kurva
				pY = outMatY[3] * pow(u, 3) + outMatY[2] *
					pow(u, 2) + outMatY[1] * u + outMatY[0];
				// koordinat Z pada kurva
				pZ = outMatZ[3] * pow(u, 3) + outMatZ[2] *
					pow(u, 2) + outMatZ[1] * u + outMatZ[0];
				glVertex3f(pX, pY, pZ); // gambar titik akhir
			}
		}
	}
}

// fungsi untuk menggambar spline
void drawObject()
{
	glPushMatrix();
	// operasi transformasi rotasi obyek ke arah kanan-kiri
	glRotatef(objectAngleY, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	// operasi transformasi rotasi obyek ke arah atas-bawah
	glRotatef(objectAngleX, 1.0f, 0.0f, 0.0f);
	// membuat 4 titik kontrol kurva
	Vec3 point1 = Vec3(-150.0f, 20.0f, 0.0f);
	Vec3 point2 = Vec3(-200.0f, 50.0f, 0.0f);
	Vec3 point3 = Vec3(200.0f, 50.0f, 0.0f);
	Vec3 point4 = Vec3(150.0f, -50.0f, 0.0f);
	// tandai setiap titik kontrol kurva dengan warna
	markPoint(point1, Vec3(0.0f, 1.0f, 0.0f), 5.0f);
	markPoint(point2, Vec3(1.0f, 0.0f, 0.0f), 5.0f);
	markPoint(point3, Vec3(1.0f, 0.0f, 1.0f), 5.0f);
	markPoint(point4, Vec3(1.0f, 1.0f, 0.0f), 5.0f);
	// mengatur warna obyek menjadi berwarna putih
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	// membuat kurva spline cubic dari titik kontrol diatas
	drawSplineCubic(point1, point2, point3, point4, 30);

	//// membuat kurva spline bezier dari titik kontrol diatas
	//drawSplineBezier(point1, point2, point3, point4, 30);

	//// membuat kurva spline catmullrom dari titik kontrol diatas
	//drawSplineCatmullRom(point1, point2, point3, point4, 30);

	glEnd();
	glPopMatrix();
	glPopMatrix();
}

// taruh semua obyek yang akan digambar di fungsi display()
void display()
{
	// bersihkan dan reset layar dan buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	// posisikan kamera pandang
	// dalam hal ini sumbu Y ada diatas dan posisi kamera pandang di (posX, posY, posZ)
	gluLookAt(posX, posY, posZ, posX + rotX, posY + rotY, posZ + rotZ, 0.0f, 1.0f, 0.0f);

	// panggil fungsi untuk menggambar obyek
	drawObject();

	// tampilkan obyek ke layar
	// gunakan glFlush() bila memakai single buffer
	// gunakan glutSwapBuffers() bila memakai double buffer
	glutSwapBuffers();
}

// inisialisasikan variabel, pencahayaan, tekstur dan pengaturan kamera pandang di fungsi init()
void init(void)
{
	// inisialisasi warna latar belakang layar dalam hal ini warna hitam (0.0, 0.0, 0.0, 0.0)
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);				// mengaktifkan depth buffer
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45.0, 1.0, 1.0, 100.0);	// set proyeksi ke perspektif
	// set proyeksi ke orthogonal
	glOrtho((GLfloat)-SCREEN_WIDTH / 2, (GLfloat)SCREEN_WIDTH / 2,
		(GLfloat)-SCREEN_HEIGHT / 2, (GLfloat)SCREEN_HEIGHT / 2, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// inisialisasi kamera pandang
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

// fungsi ini digunakan bila layar akan diresize (default)
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(45, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
	// set proyeksi ke orthogonal
	glOrtho((GLfloat)-SCREEN_WIDTH / 2, (GLfloat)SCREEN_WIDTH / 2,
		(GLfloat)-SCREEN_HEIGHT / 2, (GLfloat)SCREEN_HEIGHT / 2, 1.0, 100.0);
	glMatrixMode(GL_MODELVIEW);
}

// fungsi untuk mengatur masukan dari keyboard 
// untuk arah kiri, kanan, atas, bawah, PgUp, dan PgDn
void keyboard(int key, int x, int y)
{
	float fraction = 0.1f;

	switch (key)
	{
		// masukkan perintah disini bila tombol kiri ditekan
	case GLUT_KEY_LEFT:
		// dalam hal ini perintah rotasi obyek ke kiri sebanyak 1 derajat 
		objectAngleY -= 1.0f;
		glutPostRedisplay();	// update obyek
		break;
		// masukkan perintah disini bila tombol kanan ditekan
	case GLUT_KEY_RIGHT:
		// dalam hal ini perintah rotasi obyek ke kanan sebanyak 1 derajat 
		objectAngleY += 1.0f;
		glutPostRedisplay();	// update obyek
		break;
		// masukkan perintah disini bila tombol atas ditekan
	case GLUT_KEY_UP:
		// dalam hal ini perintah rotasi obyek ke atas sebanyak 1 derajat 
		objectAngleX -= 1.0f;
		glutPostRedisplay();	// update obyek
		break;
		// masukkan perintah disini bila tombol bawah ditekan
	case GLUT_KEY_DOWN:
		// dalam hal ini perintah rotasi obyek ke bawah sebanyak 1 derajat 
		objectAngleX += 1.0f;
		glutPostRedisplay();	// update obyek
		break;
		// zoom in
	case GLUT_KEY_PAGE_UP:
		// masukkan perintah disini bila tombol PgUp ditekan
		posX += rotX * fraction;
		posZ += rotZ * fraction;
		glutPostRedisplay();	// update obyek
		break;
		// zoom out
	case GLUT_KEY_PAGE_DOWN:
		// masukkan perintah disini bila tombol PgDn ditekan
		posX -= rotX * fraction;
		posZ -= rotZ * fraction;
		glutPostRedisplay();	// update obyek
		break;
	}
}

// timer untuk animasi (gunakan bila perlu)
void timer(int value)
{
	glutPostRedisplay();
	glutTimerFunc(55, timer, 0);
}

// program utama
int main(int argc, char** argv)
{
	// inisialisasi jendela OpenGL
	// GLUT_SINGLE berarti memakai single buffer
	// GLUT_DOUBLE berarti memakai double buffer
	// GLUT_RGB berarti mode tampilan yang dipakai RGB
	// GLUT_RGBA berarti mode tampilan yang dipakai RGBA
	// GLUT_DEPTH berarti memakai depth buffer
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// set ukuran jendela tampilan
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);		// besarnya jendela dalam piksel dalam hal ini 480X480
	glutInitWindowPosition(100, 100);	// posisi jendela dilayar komputer dalam piksel
	// judul jendela (wajib diubah dengan informasi NAMA / NIM - JUDUL PRAKTIKUM masing-masing)
	glutCreateWindow("Muhammad Fakhrul Amin / 2000018277 - Post 07 Kurva Spline");

	// panggil fungsi init untuk inisialisasi awal
	init();

	// event handler untuk display, reshape dan keyboard
	glutDisplayFunc(display);   // display
	glutReshapeFunc(reshape);   // reshape
	glutSpecialFunc(keyboard);  // keyboard
	//glutTimerFunc(0, timer, 0); // aktifkan timer bila perlu

	// looping
	glutMainLoop();

	return 0;
}