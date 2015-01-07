#ifndef PLANE_H_
#define PLANE_H_

#include "surface.h"
#include "basic_constructs.h"

class plane : public surface {
	public:
		mvector n;
		double d;
		plane (mvector &norm, double dist);
		virtual bool intersect (const ray &r, double start, double end, intersection &info, bool useBBox);
		virtual ~plane();
};

#endif
