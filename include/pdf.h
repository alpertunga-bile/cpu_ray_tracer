#pragma once

#include "onb.h"

class pdf
{
public:
	virtual ~pdf() {}

	virtual double value(const vec3& direction) const = 0;
	virtual vec3 generate() const = 0;
};

class cosinePdf : public pdf
{
public:
	cosinePdf(const vec3& w) { uvw.buildFrom(w); }

	virtual double value(const vec3& direction) const override
	{
		auto cosine = dot(unit_vector(direction), uvw.w());

		return (cosine <= 0) ? 0 : cosine / PI;
	}

	virtual vec3 generate() const override
	{
		return uvw.local(random_cosine_direction());
	}

public:
	onb uvw;
};

class hittablePdf : public pdf
{
public:
	hittablePdf(shared_ptr<hittable> p, const point3& origin)
		: ptr(p), o(origin) {}

	virtual double value(const vec3& direction) const override
	{
		return ptr->pdf_value(o, direction);
	}

	virtual vec3 generate() const override
	{
		return ptr->random(o);
	}

public:
	point3 o;
	shared_ptr<hittable> ptr;
};

class mixturePdf : public pdf
{
public:
	mixturePdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1)
	{
		p[0] = p0;
		p[1] = p1;
	}

	virtual double value(const vec3& direction) const override
	{
		return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
	}

	virtual vec3 generate() const override
	{
		if (random_double() < 0.5)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

public:
	shared_ptr<pdf> p[2];
};

inline vec3 random_to_sphere(double radius, double distanceSquared)
{
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distanceSquared) - 1);

	auto phi = 2 * PI * r1;
	auto x = cos(phi) * sqrt(1 - z * z);
	auto y = sin(phi) * sqrt(1 - z * z);

	return vec3(x, y, z);
}