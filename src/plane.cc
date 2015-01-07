#include "plane.h"

plane::plane (mvector &norm, double dist)  {
	n = norm;
	d = dist;
	n.normalize();
}

bool plane::intersect (const ray &r, double start, double end, intersection &info, bool useBBox) {
	double dn = r.d * n;
	if (dn == 0.0)
		return false;
	double t = (r.p*n + d)/-dn;
	if (t <= start || t >= end)
		return false;
	info.mat = mat;
	info.t = t;
	info.n = n;
	return true;
}

plane::~plane() {

}
