#include "sphere.h"
#include <cmath>
#include <iostream>
using namespace std;

sphere::sphere (const point &o, double r) {
	this->r = r;
	this->o = o;
	point min = o, max = o;
	min -= r;
	max += r;
	box = bbox(min, max);
}

sphere::~sphere() {

}

bool sphere::intersect (const ray &r, double start, double end, intersection &info, bool useBBox) {
	double intersectionT;
	if (!box.intersect(r, start, end, intersectionT))
		return false;
	if (useBBox) {
		info.n = box.getNormal(r.evaluate(intersectionT));
		info.t = intersectionT;
		info.mat = mat;
		return true;
	}
	/* discriminant formula
	 * (d.(e-c))^2 - (d.d) ((e-c).(e-c) - R^2)
	 */
	mvector ec = r.p - this->o;
	double dec = r.d * ec;
	double dec2 = dec * dec;

	double dd = r.d * r.d;
	double ecec = ec * ec;
	double ececr2 = ecec - this->r * this->r;
	double ddececr2 = dd * ececr2;

	double disc = dec2 - ddececr2;
	double s_disc = sqrt(disc);
	if (disc >= 0) {
		double t1 = (-dec + s_disc) / dd;
		double t2 = (-dec - s_disc) / dd;

		if (t1 <= start || t2 <= start || (t1 >= end && t2 >= end))
			return false;

		/* Set intersection point to closest. t1 if t1 == t2. Set Normal and material */
		info.mat = mat;
		if (t2 >= t1) {
			info.t = t1;
			info.n = (r.evaluate(t1) - o) * (1.0/this->r);
		}
		else {
			info.t = t2;
			info.n = (r.evaluate(t2) - o) * (1.0/this->r);
		}
		return true;
	}
	return false;
}
