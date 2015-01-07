#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "basic_constructs.h"
#include <limits>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <ImfRgbaFile.h>
#include <ImfStringAttribute.h>
#include <ImfMatrixAttribute.h>
#include <ImfArray.h>
#include "light.h"
using namespace Imf;
using namespace Imath;

/* Wrapper for transferring camera information over */
class camerainfo {
public:
	camerainfo(const point &e, const mvector &u, const mvector &v, const mvector &w,
			double d, int nx, int ny, double l, double r, double t, double b)
	: eye(e), u(u), v(v), w(w) {
		this->d = d;
		this->nx = nx;
		this->ny = ny;
		this->l = l;
		this->r = r;
		this->t = t;
		this->b = b;
	}
	const point &eye;
	const mvector &u;
	const mvector &v;
	const mvector &w;
	double d;
	int nx, ny;
	double l, r, t, b;
};

class montecarlo {
private:
	enum rayType {VIEWING_RAY, SHADOW_RAY, REFLECTION_RAY, REFRACTION_RAY};
	RGB L (const ray &r, rayType rt, double min_t, double max_t, unsigned int rel_light, int recursionC, int rayID);
	inline bool getClosestIntersection (const ray &r, double min_t, double max_t, intersection &i);
	inline bool isOccluded (const ray &r, double min_t, double max_t);
	inline ray getRay(int i, int j, int p, int q);
	inline ray getRegularRay(int i, int j);
	inline void createMapping();
	inline RGB getLightSpectralDensity(const ray &r, double min_t, double max_t, int rel_light);
	void blinn_phong (const ray &r, mvector &norm, mvector &l, material *mat, RGB &l_spd, RGB &ret);
	bool singleShadowRay, singlePrimaryRay, useBBox;
	int pixelSamples, shadowSamples;
	vector<int> correlatedShadows;
public:
	const sceneobjects &objs;
	const camerainfo &caminfo;
	int pSampleSq, sSampleSq;
	montecarlo(const sceneobjects &objs, const camerainfo &ci, int pSamples, int sSamples, bool bbox);
	void setPixel (Rgba &pixel, int i, int j);
	static const double precision = 0.00001;
	static const int recursionLimit = 5;
	const double infinity;

};

montecarlo::montecarlo(const sceneobjects &o, const camerainfo &ci, int pSamples, int sSamples, bool bbox)
: objs(o), caminfo(ci), infinity(numeric_limits<double>::infinity()){
	pSampleSq = pSamples;
	sSampleSq = sSamples;
	pixelSamples = pSamples * pSamples;
	shadowSamples = sSamples * sSamples;
	srand(time(0));
	singlePrimaryRay = pSampleSq == 1 ? true : false;
	singleShadowRay = sSampleSq == 1 ? true : false;
	useBBox = bbox;
	/* Use the correlated shuffling only if p, s where p > 1 and s == 1 */
	if (!singlePrimaryRay && singleShadowRay)
		for (int p = 0; p < pixelSamples; p++)
			correlatedShadows.push_back(p);

}

ray montecarlo::getRay(int i, int j, int p, int q) {
	double r = rand()/(double) RAND_MAX;
	double s = rand()/(double) RAND_MAX;
	double u_s = caminfo.l + (caminfo.r - caminfo.l)*(i - 0.5 + (p + r)/pSampleSq)/caminfo.nx;
	double v_s = caminfo.t - (caminfo.t - caminfo.b)*(j - 0.5 + (q + s)/pSampleSq)/caminfo.ny;
	mvector raydir = (caminfo.u * u_s) + (caminfo.v * v_s) + (caminfo.w * -caminfo.d);
	ray ret(caminfo.eye, raydir);
	return ret;
}

/*
 * Populates is with closest intersection info if has one and returns true,
 * else false and is is unchanged
 */
bool montecarlo::getClosestIntersection (const ray &r, double min_t, double max_t, intersection &is) {
	const vector<surface *> &sfs = objs.surfaces;
	size_t size = sfs.size();
	bool *isSurfIsect = new bool[size];
	intersection *surfIsect = new intersection[size];
	bool hit = false;
	for (unsigned int s = 0; s < size; ++s)
		hit |= (isSurfIsect[s] = (sfs[s])->intersect(r, min_t, max_t, surfIsect[s], useBBox));

	/* If hit, we must have a closest surface */
	if (hit) {
		double closest = infinity;
		int closest_i = -1;
		for (unsigned int iter = 0; iter < size; ++iter)
			if (isSurfIsect[iter])
				if (surfIsect[iter].t < closest) {
					closest = surfIsect[iter].t;
					closest_i = iter;
				}
		assert (closest_i >= 0);
		is = surfIsect[closest_i];
	}
	delete[] surfIsect;
	delete[] isSurfIsect;
	return hit;
}

bool montecarlo::isOccluded (const ray &r, double min_t, double max_t) {
	const vector<surface *> &sfs = objs.surfaces;
	intersection dummy;
	size_t size = sfs.size();
	bool hit = false;
	for (unsigned int s = 0; s < size && !hit; ++s)
		hit |= sfs[s]->intersect(r, min_t, max_t, dummy, useBBox);
	return hit;
}

/* If we have 1 Shadow Ray per Primary Ray, create a mapping from stratified points to light points */
void montecarlo::createMapping() {
	for (int p = 0; p < pixelSamples; p++) {
		int temp = correlatedShadows[p];
		int swapee = rand()%pixelSamples;
		correlatedShadows[p] = correlatedShadows[swapee];
		correlatedShadows[swapee] =  temp;
	}
}

RGB montecarlo::getLightSpectralDensity(const ray &r, double min_t, double max_t, int rel_lgt) {
	if (isOccluded(r, min_t, max_t))
		return RGB();
	light *l = objs.lights[rel_lgt];
	if (l->getLightType() == light::POINT) {
		return l->spectralD() /= (point::distanceSq(l->getPosition(), r.p));
	}
	/* Area light */
	s_light *sl = static_cast<s_light*>(l);
	return sl->getWeightedSpectralD(r);
}

void montecarlo::blinn_phong (const ray &r, mvector &norm, mvector &l, material *mat, RGB &l_rgb, RGB &ret) {
	l.normalize();
	/* Lambertian Shading */
	RGB lambertian = l_rgb;
	double lamb_const =  max((double)0.0, norm * l);
	lambertian *= lamb_const;
	lambertian *= mat->diffuse;
	ret += lambertian;

	/* Specular shading */
	RGB phong = l_rgb;
	mvector v = r.d * -1.0;
	v.normalize();
	mvector h = v + l;
	h.normalize();
	double bp_const = pow( max((double)0.0, norm * h), mat->phong_exponent);
	phong *= bp_const;
	phong *= mat->specular;
	ret += phong;
}

/*
 * rel_lgt is a valid light index if rt is shadow ray, else it is -1
 * IMPORTANT!! Uses blinn_phong shading to calculate the luminescence on a ray
 * Should possible move that out if other shaders are going to be used.
 *
 * rayID is the ray's correlated location on light
 */
RGB montecarlo::L(const ray &r, rayType rt, double min_t, double max_t, unsigned int rel_lgt, int recursionC, int rayID) {
	if (recursionC == 0)
		return RGB();

	/* If shadow ray, rel_light is just the relevant light index */
	if (rt == SHADOW_RAY)
		return getLightSpectralDensity(r, min_t, max_t, rel_lgt);

	/* Whatever type of ray, see if there is an intersection. Return if none */
	intersection closest;
	bool hasIsect = getClosestIntersection(r, min_t, max_t, closest);
	if (!hasIsect)
		return RGB();

	/* We have an intersection. closest has been populated */
	point isection = r.evaluate(closest.t);
	closest.n.normalize();
	material *mat = objs.materials[closest.mat];
	RGB ret;
	/*
	 * Want to change normal if we hit the backside of a surface for regular rays.
	 * Matters for triangles and planes sitting in space.
	 */
	mvector norm = (closest.n * r.d) >= 0.0 ? -closest.n : closest.n;

	size_t lt_size = objs.lights.size();
	for (unsigned int s = 0; s < lt_size; ++s) {
		light *lt = objs.lights[s];
		if (lt->getLightType() == light::POINT) {
			mvector l = lt->getPosition() - isection;
			ray sr(isection, l);
			RGB l_rgb = L(sr, SHADOW_RAY, precision, 1.0, s, 1, rayID);
			if (l_rgb.hasNoEnergy())
				continue;
			blinn_phong(r, norm, l, mat, l_rgb, ret);
		}
		else {
			/* Area Light */
			s_light *sl = static_cast<s_light*>(lt);
			point sample;
			if (singleShadowRay && !singlePrimaryRay) {
				int correlatedShadow = correlatedShadows[rayID];
				int p = correlatedShadow/pSampleSq;
				int q = correlatedShadow%pSampleSq;
				sl->getSample(sample, p, q, pSampleSq);
				mvector toLight = sample - isection;
				ray sr(isection, toLight);
				RGB l_rgb = L(sr, SHADOW_RAY, precision, 1.0, s, 1, rayID);
				if (l_rgb.hasNoEnergy())
					continue;
				blinn_phong(r, norm, toLight, mat, l_rgb, ret);
			} else {
				RGB temp;
				for (int p = 0; p < sSampleSq; p++)
					for (int q = 0; q < sSampleSq; q++) {
						sl->getSample(sample, p, q, sSampleSq);
						mvector toLight = sample - isection;
						ray sr(isection, toLight);
						RGB l_rgb = L(sr, SHADOW_RAY, precision, 1.0, s, 1, rayID);
						if (l_rgb.hasNoEnergy())
							continue;
						blinn_phong (r, norm, toLight, mat, l_rgb, temp);
					}
				temp /= (double) shadowSamples;
				ret += temp;
			}
		}
	}

	/* Add ambient if camera ray and we have an intersection */
	if (rt == VIEWING_RAY) {
		RGB ambient = objs.al.intensity();
		ambient *= mat->diffuse;
		ret += ambient;
	}

	/* Add reflections if we have reflective */
	if (mat->ideal_reflective.hasNoEnergy())
		return ret;
	RGB reflective = mat->ideal_reflective;
	mvector reflect_direction = r.d + norm*((r.d * norm)*-2.0);
	ray reflected(isection, reflect_direction);
	return ret +=
			(reflective *=
					L(reflected, REFLECTION_RAY, precision, infinity, -1, recursionC-1, rayID));
}

void montecarlo::setPixel(Rgba &pixel, int i, int j) {
	if (singleShadowRay && !singlePrimaryRay)
		createMapping();

	RGB irradiance;
	for (int p = 0; p < pSampleSq; p++)
		for (int q = 0; q < pSampleSq; q++) {
			ray viewing = getRay(i, j, p, q);
			irradiance += L(viewing, VIEWING_RAY, 0.0, infinity, -1, recursionLimit, p*pSampleSq+q);
		}
	irradiance /= (double) pixelSamples;

	pixel.r = irradiance.r;
	pixel.g = irradiance.g;
	pixel.b = irradiance.b;
	pixel.a = 1.0;
}

#endif
