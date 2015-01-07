#ifndef BASIC_CONSTRUCTS_H
#define BASIC_CONSTRUCTS_H

class mvector {
	public:
		double x,y,z;
		mvector ();
		mvector (double x, double y, double z);
		mvector operator* (double a) const;
		mvector operator- () const;
		mvector operator+ (const mvector &v) const;
		mvector operator- (const mvector &v) const;
		mvector& operator+= (const mvector &v);
		double operator* (const mvector &v) const;
		mvector cross (const mvector &v) const;
		/* Don't try to normalize a zero vector ! */
		void normalize ();
		void forceNormalize();
	private:
		/*
		 * Is true if normalize was called at least once.
		 * If you then change x,y,z, normalize won't work.
		 * Use forceNormalize().
		 */
		bool isNormalized;
};

class point {
	public:
		double x,y,z;
		point ();
		point(double x, double y, double z);
		static double distanceSq(const point  &p1, const point &p2);
		double operator* (const mvector &n) const;
		point operator+ (const mvector &v) const;
		mvector operator- (const point &p) const;
		point& operator+= (double e);
		point& operator-= (double e);
};


class ray {
	public:
		ray ();
		ray (const point &pt);
		ray (const point &pt, const mvector &dir);
		point evaluate (const double t) const ;
		point p;
		mvector d;
};

class intersection {
	public:
		double t;
		mvector n;
		int mat;
};

/* Wrapper for all triples of color, intensity etc */
class RGB {
	public:
		double r,g,b;
		RGB();
		RGB(double r, double g, double b);
		bool hasNoEnergy();
		RGB& operator+= (const RGB &v);
		RGB& operator*= (const RGB &v);
		RGB& operator*= (double c);
		RGB& operator/= (double c);
};

/* Bounding box class to use for surfaces */
class bbox {
	public:
		point min, max;
		bbox();
		bbox(const point &minp, const point &maxp);
		bool intersect(const ray &r, double start, double end, double &t);
		mvector getNormal(const point &intersection);
	private:
		static const double PUSHOUT = 0.00001;
		static const double PRECISION = 0.0001;
		void pushOut();
};

#endif
