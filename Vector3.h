#pragma once
#ifndef  _VECTOR3_H
#define _VECTOR3_H
class Vector3
{
public:
	double x, y, z;
	Vector3() { x = 0.0; y = 0.0; z = 0.0; }
	Vector3(double xx, double yy, double zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}
	inline double length()
	{
		return sqrt(x * x + y * y + z * z);
	}
	Vector3 normalize()
	{
		double len = length();
		if (len == 0)
			return Vector3(0, 0, 1);
		x = x / len; y = y / len; z = z / len;
		Vector3 normal(x, y, z);
		return normal;
	}
	double dotProduct(const Vector3& v)
	{
		return (x * v.x + y * v.y + z * v.z);
	}
	Vector3 crossProduct(const Vector3& v)
	{
		Vector3 vec;
		vec.x = y * v.z - z * v.y;
		vec.y = z * v.x - x * v.z;
		vec.z = x * v.y - y * v.x;
		if (vec.x > 0) {
			vec.x *= -1; vec.y *= -1; vec.z *= -1;
		}
		if (vec.x < 0.01&&vec.y < 0.01&&vec.z < 0.01) {
			//printf("cross warning\n");
			return Vector3(vec.x, 0, vec.z);
		}
		return vec.normalize();
	}
	Vector3 operator +(const Vector3& v)
	{
		Vector3 vec;
		vec.x = x + v.x;
		vec.y = y + v.y;
		vec.z = z + v.z;

		return vec;
	}
	Vector3 operator -(const Vector3& v)
	{
		Vector3 vec;
		vec.x = x - v.x;
		vec.y = y - v.y;
		vec.z = z - v.z;

		return vec;
	}
};
#endif // ! _VECTOR3_H
