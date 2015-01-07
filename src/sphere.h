#ifndef SPHERE_H
#define SPHERE_H

#include "surface.h"
#include "basic_constructs.h"

class sphere : public surface {
	public:
		point o;
		double r;
		sphere (const point &origin, double radius);
		bool intersect (const ray &r, double start, double end, intersection &info, bool useBBox);
		virtual ~sphere();
};

#endif
