#ifndef SURFACE_H
#define SURFACE_H

#include "basic_constructs.h"

class surface {
	public:
		virtual bool intersect (const ray &r, double start, double end, intersection &info, bool useBBox) =0;
		void setMaterial (int m) { mat = m; }
		virtual ~surface() {}
		int mat;
		bbox box;
};

#endif
