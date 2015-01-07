#include "material.h"

material::material() {
	phong_exponent = 0.0;
	diffuse.r = diffuse.b = diffuse.g = 0.0;
	specular.r = specular.b = specular.g = 0.0;
	ideal_reflective.r = ideal_reflective.b = ideal_reflective.g = 0.0;
}

material::material (double dr, double dg, double db, double sr, double sg,
					double sb, double r, double ir, double ig, double ib) {
	diffuse.r = dr;
	diffuse.g = dg;
	diffuse.b = db;
	specular.r = sr;
	specular.g = sg;
	specular.b = sb;
	ideal_reflective.r = ir;
	ideal_reflective.g = ig;
	ideal_reflective.b = ib;
	phong_exponent = r;
}

bool material::isSame (double dr, double dg, double db, double sr, double sg,
						double sb, double r, double ir, double ig, double ib) const {
	return  diffuse.r == dr && diffuse.g == dg && diffuse.b == db &&
			specular.r == sr && specular.g == sg && specular.b == sb &&
			ideal_reflective.r == ir && ideal_reflective.g == ig && ideal_reflective.b == ib &&
			phong_exponent == r;
}
