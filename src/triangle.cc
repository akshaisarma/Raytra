#include "triangle.h"
#include <cmath>
#include <algorithm>
#include <iostream>
using namespace std;

triangle::triangle (const point p1, const point p2, const point p3) {
	this->p1 = p1;
	this->p2 = p2;
	this->p3 = p3;
	n = (p2 - p1).cross(p3 - p1);
	n.normalize();

	double bbminx, bbminy, bbminz, bbmaxx, bbmaxy, bbmaxz;
	bbminx = min((double)min(p1.x, p2.x), p3.x);
	bbminy = min((double)min(p1.y, p2.y), p3.y);
	bbminz = min((double)min(p1.z, p2.z), p3.z);
	bbmaxx = max((double)max(p1.x, p2.x), p3.x);
	bbmaxy = max((double)max(p1.y, p2.y), p3.y);
	bbmaxz = max((double)max(p1.z, p2.z), p3.z);
	point min(bbminx, bbminy, bbminz);
	point max(bbmaxx, bbmaxy, bbmaxz);
	box = bbox(min, max);
}

mvector triangle::getNormal() {
	return n;
}

bool triangle::intersect (const ray &r, double start, double end, intersection &info, bool useBBox) {
	double intersectionT;
	if (!box.intersect(r, start, end, intersectionT))
		return false;
	if (useBBox) {
		info.n = box.getNormal(r.evaluate(intersectionT));
		info.t = intersectionT;
		info.mat = mat;
		return true;
	}
/*
 * System of equations to solve. e coefficient is r.p. a,b,c are triangle points.
 * xa-xb xa-xc xd		B		xa-xe
 * ya-yb ya-yc yd	X	y	= 	ya-ye
 * za-zb za-zc zd		t		za-ze
 *
 * a     d     g		B		j
 * b     e     h	X	y	= 	k
 * c     f     i		y		l
 *
 * Using Cramer,
 * B * M =   j(ei - hf) + k(gf - di) + l(dh - eg)
 * y * M =   i(ak - jb) + h(jc - al) + g(bl - kc)
 * t * M = - f(ak - jb) + e(jc - al) + d(bl - kc)
 * M 	 =   a(ei - hf) + b(gf - di) + c(dh - eg)
 */
	double a = p1.x - p2.x;
	double b = p1.y - p2.y;
	double c = p1.z - p2.z;
	double d = p1.x - p3.x;
	double e = p1.y - p3.y;
	double f = p1.z - p3.z;
	double g = r.d.x;
	double h = r.d.y;
	double i = r.d.z;
	double j = p1.x - r.p.x;
	double k = p1.y - r.p.y;
	double l = p1.z - r.p.z;

	double eihf = e*i - h*f;
	double gfdi = g*f - d*i;
	double dheg = d*h - e*g;

	double M = a*eihf + b*gfdi + c*dheg;
	if (M == 0)
		return false;

	double akjb = a*k - j*b;
	double jcal = j*c - a*l;
	double blkc = b*l - k*c;

	double t = (f*akjb + e*jcal + d*blkc)/-M;
	if (t <= start || t >= end)
		return false;

	double y = (i*akjb + h*jcal + g*blkc)/M;
	if (y < 0 || y > 1)
		return false;

	double B = (j*eihf + k*gfdi + l*dheg)/M;
	if (B < 0 || B > 1-y)
		return false;

	info.mat = mat;
	info.t = t;
	info.n = n;
	return true;
}

triangle::~triangle () {}

