#ifndef MATERIAL_H
#define MATERIAL_H

#include "basic_constructs.h"

class material {
	public:
		material ();
		material (double dr, double dg, double db, double sr, double sg,
				  double sb, double r, double ir, double ig, double ib);
		bool isSame (double dr, double dg, double db, double sr, double sg,
					double sb, double r, double ir, double ig, double ib) const ;
		bool isRefractive () const;
		RGB diffuse, specular, ideal_reflective;
		double phong_exponent;
};

#endif
