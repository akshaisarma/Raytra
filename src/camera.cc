#include "camera.h"
#include "sceneobjects.h"
#include "montecarlo.h"
using namespace std;

camera::camera () {
	eye = point(0, 0, 0);
	u = v = w = mvector(0, 0, 0);
	d = l = r = t = b = 0.0;
	nx = ny = 0;
	pixels = 0;
}

camera::camera (double x, double y, double z, double vx, double vy, double vz,
				double d, double iw, double ih, int pw, int ph) {
	eye = point (x, y, z);

	mvector dv(vx, vy, vz);

	mvector up;
	// Check if camera is pointing up
	if (vx == 0.0 && vz == 0.0) {
		u = mvector(1.0, 0.0, 0.0);
	}
	else {
		up = mvector(0, 1, 0);
		u = dv.cross(up);
	}
	v = u.cross(dv);
	w = -dv;
	u.normalize();
	v.normalize();
	w.normalize();

	this->d = d;

	r = iw/2.0;
	l = -r;
	t = ih/2.0;
	b = -t;

	nx = pw;
	ny = ph;

	pixels = new Rgba[ny * nx];
}

camera::~camera () {
	delete[] pixels;
}

void camera::renderScene(const sceneobjects &objs, int pS, int sS, bool useBBox) {
	int counter = nx*ny/100, tick = 0;
	double ratio = 100.0/(nx*ny);

	/* Do not want to do montecarlo integration here, so sending camera info to montecarlo class */
	camerainfo ci(eye, u, v, w, d, nx, ny, l, r, t, b);
	montecarlo m(objs, ci, pS, sS, useBBox);

	for (int j = 0; j < ny; ++j)
		for (int i = 0; i < nx; ++i, ++counter) {
			if (counter*ratio > tick)
				cout << "Progress : " << tick++ << "%\r" << flush;
			Rgba &px = pixels[nx*j + i];
			m.setPixel(px, i, j);
		}
}

void camera::writeEXR (const char *outFile) {
	Box2i cropped(V2i(0, 0), V2i(nx - 1, ny - 1));
	RgbaOutputFile file(outFile, cropped, cropped, WRITE_RGBA);
	file.setFrameBuffer(pixels, 1, nx);
	file.writePixels(cropped.max.y - cropped.min.y + 1);
}
