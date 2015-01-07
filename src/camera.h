#ifndef CAMERA_H
#define CAMERA_H

#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
#include <vector>
#include <iostream>
#include "surface.h"
#include "basic_constructs.h"
#include "light.h"

using namespace std;
using namespace Imf;
using namespace Imath;

class sceneobjects;
class intersection;

class camera {
		point eye;
		mvector u, v, w;
		double d;
		int nx, ny;
		double l, r, t, b;
		Rgba *pixels;
		void shade(Rgba &pixel, intersection &isect_info, ray &r, sceneobjects &objs);

	public:
		camera ();
		camera (double x, double y, double z, double vx, double vy, double vz,
				double d, double iw, double ih, int pw, int ph);
		~camera ();
		void renderScene(const sceneobjects &s, int pS, int sS, bool useBBox);
		void writeEXR (const char *outFile);
};

#endif
