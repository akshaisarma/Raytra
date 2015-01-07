#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "surface.h"
#include "basic_constructs.h"

class triangle: public surface {
	public:
		point p1, p2, p3;
		triangle (const point p1, const point p2, const point p3);
		bool intersect (const ray &r, double start, double end, intersection &info, bool useBBox);
		mvector getNormal();
		virtual ~triangle();
	private:
		mvector n;
};

#endif
