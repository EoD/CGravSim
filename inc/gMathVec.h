#ifndef _INC_GRAVITY_MATHVEC
#define _INC_GRAVITY_MATHVEC

#include <iostream>
#include <typeinfo>
#include <stdexcept>
#include "gDefines.h"

template <typename T> class mv {
private:
	long double toDouble(T x) const {
		if(typeid(T) == typeid(long long) || typeid(T) == typeid(long))
			return (long double)x / (long double)LACCURACY;
		else
			return (long double)x;
	}
	long long toLong(T x) const {
		if(typeid(T) == typeid(long double) || typeid(T) == typeid(double))
			return (long long)floor((x * (long double)LACCURACY)+0.5);
		else
			return (long long)x;
	}
public:
	T x,y,z;
	mv<T>(T tx=0, T ty=0, T tz=0): x(tx), y(ty), z(tz) {}

	typedef mv<long double> mdv;
	typedef mv<long long> mlv;
	
	operator mlv () const {
		if(typeid(T) == typeid(long double) || typeid(T) == typeid(double))
			return mlv(toLong(x), toLong(y), toLong(z));
		
		return mlv((long long)x, (long long)y, (long long)z);
	}

	operator mdv () const {
		if(typeid(T) == typeid(long long) || typeid(T) == typeid(long))
			return mdv(toDouble(x), toDouble(y), toDouble(z));
		
		return mdv((long double)x, (long double)y, (long double)z);
	}

	T operator[](unsigned int n) {
		if(n==0)
			return x;
		else if(n==1)
			return y;
		else if(n==2)
			return z;
		else
			throw std::out_of_range("mv<T>::operator[] - out of range");
	}
};

typedef mv<long double> mdv;
typedef mv<long long> mlv;

template <typename T> T abs(const mv<T>& mv1) {
	return sqrt(mv1.x*mv1.x + mv1.y*mv1.y + mv1.z*mv1.z);
}

template <typename T> T operator*(const mv<T>& mv1, const mv<T>& mv2) {
	return (mv1.x * mv2.x + mv1.y * mv2.y) + mv1.z * mv2.z;
}

template <typename T> mv<T>& operator*=(mv<T>& mv1, const T d) {
	mv1.x *= d; mv1.y *= d; mv1.z *= d;
	return mv1;
}

template <typename T> mv<T> operator*(const mv<T>& mv1, const T d) {
	return mv<T>(mv1.x * d, mv1.y * d, mv1.z * d);
}

template <typename T> mv<T> operator/(const mv<T>& mv1, const T d) {
	return mv<T>(mv1.x / d, mv1.y / d, mv1.z / d);
}

template <typename T> mv<T> operator/=(mv<T>& mv1, const T d) {
	mv1.x /= d; mv1.y /= d; mv1.z /= d;
	return mv1;
}

template <typename T> mv<T> operator+(const mv<T>& mv1, const mv<T>& mv2) {
	return mv<T>(mv1.x + mv2.x, mv1.y + mv2.y, mv1.z + mv2.z); 
}

template <typename T> mv<T> operator-(const mv<T>& mv1, const mv<T>& mv2) {
	return mv<T>(mv1.x - mv2.x, mv1.y - mv2.y, mv1.z - mv2.z); 
}

template <typename T> mv<T>& operator+=(mv<T>& mv1, const mv<T>& mv2) {
	mv1.x += mv2.x;	mv1.y += mv2.y;	mv1.z += mv2.z;
	return mv1;
}

template <typename T> std::ostream& operator<<(std::ostream& stream, mv<T>& mv1) {
	stream << "(" << mv1.x << ", " << mv1.y << ", " << mv1.z << ")";
	return stream;
}

template <typename T> bool operator==(const mv<T>& mv1, const mv<T>& mv2) {
	if(mv1.x == mv2.x && mv1.y == mv2.y && mv1.z == mv2.z)
		return true;
	
	return false;
}
template <typename T> bool operator==(const mv<T>& mv1, const int d) {
	//this should work for the 0 only at the moment!
	if(d == 0)
		if(abs(mv1) == d)
			return true;

	return false;
}

#endif
