
#ifndef V_H
#define V_H

#include <math.h>


class Vector {
public :
	float x, y, z;

	Vector(float xval = 0, float yval = 0, float zval = 0) {
		x = xval;
		y = yval;
		z = zval;
	}
	void	setX(double val) { x = val; }
	void	setY(double val) { y = val; }
	void	setZ(double val) { z = val; }
	float	getX(void) { return x; }
	float	getY(void) { return y; }
	float   getZ(void) { return z; }

public:
	
	Vector operator +(Vector v) {
		Vector ret = (*this);
		ret.x += v.x;
		ret.y += v.y;
		ret.z += v.z;

		return ret;
	}

	Vector operator -(Vector v) {
		Vector ret = (*this);
		ret.x -= v.x;
		ret.y -= v.y;
		ret.z -= v.z;

		return ret;
	}

	Vector operator /(float val) {
		Vector ret = (*this);
		ret.x /= val;
		ret.y /= val;
		ret.z /= val;

		return ret;
	}
	
	void operator=(Vector &v)
	{
		x = v.x;
		y = v.y;
		z = v.z;

	}
	friend Vector operator *(Vector v, float val) {
		v.x *= val;
		v.y *= val;
		v.z *= val;

		return v;
	}

	friend Vector operator *(float val, Vector v) {
		v.x *= val;
		v.y *= val;
		v.z *= val;

		return v;
	}
	Vector &operator +=(Vector v) {
		Vector ret = (*this);
		ret.x += v.x;
		ret.y += v.y;
		ret.z += v.z;

		return ret;
	}

	Vector &operator -=(Vector v) {
		Vector ret = (*this);
		ret.x -= v.x;
		ret.y -= v.y;
		ret.z -= v.z;

		return ret;
	}

	float Magnitude() {
		return sqrt(x*x + y*y + z*z);
	}
	double dist()
	{
		return sqrt(x*x + y*y + z*z);
	}
	double dist(Vector p)
	{
		return sqrt((x - p.x)*(x - p.x) + (y - p.y)*(y - p.y) + (z - p.z)*(z - p.z));
	}
	float InnerProduct(Vector v) {
		return(x*v.x + y*v.y + z*v.z);
	}

	Vector CrossProduct(Vector v) {
		Vector ret;

		ret.x = y*v.z - z*v.y;
		ret.y = z*v.x - x*v.z;
		ret.z = x*v.y - y*v.x;

		return ret;
	}

	void Normalize() {
		float w = Magnitude();
		if (w < 0.00001) return;
		x /= w;
		y /= w;
		z /= w;
	}
};
#endif