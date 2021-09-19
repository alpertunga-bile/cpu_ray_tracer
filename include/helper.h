#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>

#include "ray.h"
#include "vec3.h"

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degrees2radians(double degrees)
{
	return degrees * PI / 180.0;
}

inline double random_double()
{
	return rand() / (RAND_MAX + 1.0);
}

inline int random_int(int min, int max)
{
	return static_cast<int>(random_double(min, max + 1));
}

inline double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}

inline vec3 random_cosine_direction()
{
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * PI * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

inline double clamp(double x, double min, double max)
{
	if (x < min) return min;
	if (x > max) return max;

	return x;
}