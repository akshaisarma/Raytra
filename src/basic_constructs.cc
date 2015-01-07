#include "basic_constructs.h"
#include <cmath>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>
using namespace std;

mvector::mvector () {
	x = y = z = 0;
	this->isNormalized = false;
}

mvector::mvector (double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->isNormalized = false;
}

mvector mvector::operator* (const double a) const{
	mvector ret(0, 0, 0);
	ret.x = x * a;
	ret.y = y * a;
	ret.z = z * a;
	return ret;
}

double mvector::operator* (const mvector &v) const {
	return x*v.x + y*v.y + z*v.z;
}

mvector mvector::operator- () const {
	mvector ret(0, 0, 0);
	ret.x = -x;
	ret.y = -y;
	ret.z = -z;
	return ret;
}

mvector mvector::operator+ (const mvector &v) const {
	mvector ret(0, 0, 0);
	ret.x = x + v.x;
	ret.y = y + v.y;
	ret.z = z + v.z;
	return ret;
}

mvector mvector::operator- (const mvector &v) const {
	mvector ret(0, 0, 0);
	ret.x = x - v.x;
	ret.y = y - v.y;
	ret.z = z - v.z;
	return ret;
}

mvector& mvector::operator+= (const mvector &v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

mvector mvector::cross (const mvector &v) const {
	mvector ret(0, 0, 0);
	ret.x = y*v.z - z*v.y;
	ret.y = z*v.x - x*v.z;
	ret.z = x*v.y - y*v.x;
	return ret;
}

void mvector::normalize () {
	if (isNormalized)
		return;
	forceNormalize();
	isNormalized = true;
}

void mvector::forceNormalize() {
	double squared = (*this) * (*this);
	double magnitude = sqrt(squared);
	assert (magnitude != 0);
	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;
}

point::point () {
	x = y = z = 0;
}

point::point (double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

point point::operator+ (const mvector &v) const {
	point ret(0, 0, 0);
	ret.x = v.x + x;
	ret.y = v.y + y;
	ret.z = v.z + z;
	return ret;
}

mvector point::operator- (const point &p) const {
	mvector ret(0, 0, 0);
	ret.x = x - p.x;
	ret.y = y - p.y;
	ret.z = z - p.z;
	return ret;
}

double point::operator* (const mvector &p) const {
	return x*p.x + y*p.y + z*p.z;
}

point& point::operator+= (double e) {
	x += e;
	y += e;
	z += e;
	return *this;
}

point& point::operator-= (double e) {
	x -= e;
	y -= e;
	z -= e;
	return *this;
}

double point::distanceSq (const point &p1, const point &p2) {
	double xD = p2.x - p1.x;
	double yD = p2.y - p1.y;
	double zD = p2.z - p1.z;
	return xD*xD + yD*yD + zD*zD;
}

ray::ray () {
	p = point();
	d = mvector();
}

ray::ray (const point &p) {
	this->p = p;
	this->d = mvector();
}

ray::ray (const point &p, const mvector &d) {
	this->p = p;
	this->d = d;
}

point ray::evaluate (const double t) const {
	return p + (d*t);
}

RGB::RGB() {
	r = g = b = 0.0;
}

RGB::RGB(double r, double g, double b) {
	this->r = r;
	this->g = g;
	this->b = b;
}

bool RGB::hasNoEnergy() {
	return r == 0.0 && g == 0.0 && b == 0.0;
}

RGB& RGB::operator *= (double v) {
	r *= v;
	g *= v;
	b *= v;
	return *this;
}

RGB& RGB::operator*= (const RGB& o) {
	r *= o.r;
	g *= o.g;
	b *= o.b;
	return (*this);
}

RGB& RGB::operator+= (const RGB& o) {
	r += o.r;
	g += o.g;
	b += o.b;
	return *this;
}

RGB& RGB::operator /= (double c) {
	r /= c;
	g /= c;
	b /= c;
	return *this;
}

bbox::bbox() {
	min = point();
	max = point();
}

bbox::bbox(const point &min, const point &max) {
	this->min = min;
	this->max = max;
	pushOut();
}

bool bbox::intersect(const ray &r, double start, double end, double &t) {
	double tminx, tmaxx, tminy, tmaxy, tminz, tmaxz, a;
	a = 1/r.d.x;
	if (a >= 0) {
		tminx = a * (min.x - r.p.x);
		tmaxx = a * (max.x - r.p.x);
	} else {
		tminx = a * (max.x - r.p.x);
		tmaxx = a * (min.x - r.p.x);
	}
	a = 1/r.d.y;
	if (a >= 0) {
		tminy = a * (min.y - r.p.y);
		tmaxy = a * (max.y - r.p.y);
	} else {
		tminy = a * (max.y - r.p.y);
		tmaxy = a * (min.y - r.p.y);
	}
	a = 1/r.d.z;
	if (a >= 0) {
		tminz = a * (min.z - r.p.z);
		tmaxz = a * (max.z - r.p.z);
	} else {
		tminz = a * (max.z - r.p.z);
		tmaxz = a * (min.z - r.p.z);
	}

	if (tminx > tmaxy || tminy > tmaxx ||
			tminy > tmaxz || tminz > tmaxy ||
				tminx > tmaxz || tminz > tmaxx)
		return false;
	/* Nearest intersection t is the largest of the mins */
	t = std::max(std::max(tminx, tminy), tminz);
	if (t < start || t > end)
		return false;
	return true;
}

/* Given the absolute xyz intersection point, returns normal */
mvector bbox::getNormal(const point &i) {
	mvector ret;
	if (abs(i.x - min.x) < PRECISION) {
		ret.x = -1;
		ret.y = ret.z = 0;
	} else if (abs(i.x - max.x) < PRECISION) {
		ret.x = 1;
		ret.y = ret.z = 0;
	} else if (abs(i.y - min.y) < PRECISION) {
		ret.y = -1;
		ret.x = ret.z = 0;
	} else if (abs(i.y - max.y) < PRECISION) {
		ret.y = 1;
		ret.x = ret.z = 0;
	} else if (abs(i.z - min.z) < PRECISION) {
		ret.z = 1;
		ret.x = ret.y = 0;
	} else if (abs(i.z - max.z) < PRECISION) {
		ret.z = -1;
		ret.x = ret.y = 0;
	} else {
		assert(false);
	}
	return ret;
}

void bbox::pushOut() {
	min -= PUSHOUT;
	max += PUSHOUT;
}
