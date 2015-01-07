#ifndef LIGHT_H
#define LIGHT_H

#include "basic_constructs.h"
#include <cstdlib>
using namespace std;

/* Simple definitions of lights with definitions */

class light {
	protected:
		point loc;
		RGB rgb;
	public:
		enum lightType {POINT, AREA};
		virtual ~light() {}
		virtual RGB spectralD() const {
			return rgb;
		}
		virtual point getPosition() const {
			return loc;
		}
		virtual lightType getLightType() =0;
};


class p_light : public light {
	public:
		p_light (const point &l, const RGB &ir) {
			loc = l;
			rgb = ir;
		}
		virtual ~p_light () {}
		virtual lightType getLightType() { return POINT;}
};

class s_light : public light {
	public:
		double len;
		mvector dir;
		mvector u, v;
		virtual ~s_light () {}
		s_light (const point &l, mvector dir, mvector u, double len, const RGB &ir) {
			loc = l;
			rgb = ir;
			this->dir = dir;
			this->u = u;
			this->v = u.cross(dir);
			this->len = len;
			dir.normalize();
			u.normalize();
			v.normalize();
		}
		virtual lightType getLightType() { return AREA;}

		void getSample (point &toFill, int p, int q, int gridWidth) const {
			double pR = rand()/(double)RAND_MAX;
			double qR = rand()/(double)RAND_MAX;
			mvector u_s = u * (len * (-0.5 + (p + pR)/gridWidth));
			mvector v_s = v * (len * (-0.5 + (q + qR)/gridWidth));
			u_s += v_s;
			toFill.x = loc.x + u_s.x;
			toFill.y = loc.y + u_s.y;
			toFill.z = loc.z + u_s.z;
		}

		RGB getWeightedSpectralD (const ray &r) {
			mvector toObj = -r.d;
			toObj.normalize();

			double wLight = max(0.0, dir*toObj);

			double distanceSq = point::distanceSq(r.evaluate(1.0), r.p);
			return (spectralD() *= wLight) /= distanceSq;
		}
};

/* Not really a light. */
class a_light {
	public:
		double r, g, b;
		a_light () {
			r = g = b = 0.0;
		}
		RGB intensity () const {
			return RGB (r, g, b);
		}
		void set(double r, double g, double b) {
			this->r = r;
			this->g = g;
			this->b = b;
		}
};

#endif
