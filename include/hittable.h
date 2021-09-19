#pragma once

#include "aabb.h"

class material;

struct hit_record
{
	point3 p;
	vec3 normal;
	double u;
	double v;
	shared_ptr<material> mat_ptr;
	double t;
	bool front_face;

	inline void set_face_normal(const ray& r, const vec3& out_normal)
	{
		front_face = dot(r.direction(), out_normal) < 0;
		normal = front_face ? out_normal : -out_normal;
	}
};

class hittable
{
public:
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const = 0;
	virtual double pdf_value(const point3& o, const vec3& v) const
	{
		return 0.0;
	}
	virtual vec3 random(const vec3& o) const
	{
		return vec3(1, 0, 0);
	}
};

class translate : public hittable
{
public:
	translate(shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
	{
		ray moved_r(r.origin() - offset, r.direction(), r.time());

		if (!ptr->hit(moved_r, t_min, t_max, rec)) return false;

		rec.p += offset;
		rec.set_face_normal(moved_r, rec.normal);

		return true;
	}

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
	{
		if (!ptr->bounding_box(time0, time1, output_box)) return false;

		output_box = aabb(output_box.min() + offset, output_box.max() + offset);

		return true;
	}

public:
	shared_ptr<hittable> ptr;
	vec3 offset;
};

class rotate_y : public hittable
{
public:
	rotate_y(shared_ptr<hittable> p, double angle);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
	{
		output_box = bbox;

		return hasBox;
	}

public:
	shared_ptr<hittable> ptr;
	double sinTheta;
	double cosTheta;
	bool hasBox;
	aabb bbox;
};

rotate_y::rotate_y(shared_ptr<hittable> p, double angle) : ptr(p)
{
	auto radians = degrees2radians(angle);
	sinTheta = sin(radians);
	cosTheta = cos(radians);
	hasBox = ptr->bounding_box(0, 1, bbox);

	point3 min(infinity, infinity, infinity);
	point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				auto x = i * bbox.max().x() + (1 - i) * bbox.min().x();
				auto y = j * bbox.max().y() + (1 - j) * bbox.min().y();
				auto z = k * bbox.max().z() + (1 - k) * bbox.min().z();

				// rotate around y axis
				auto newX = cosTheta * x + sinTheta * z;
				auto newZ = -sinTheta * x + cosTheta * z;

				vec3 tester(newX, y, newZ);

				for (int c = 0; c < 3; c++)
				{
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}
		}
	}

	bbox = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	auto origin = r.origin();
	auto direction = r.direction();

	origin[0] = cosTheta * r.origin()[0] - sinTheta * r.origin()[2];
	origin[2] = sinTheta * r.origin()[0] + cosTheta * r.origin()[2];

	direction[0] = cosTheta * r.direction()[0] - sinTheta * r.direction()[2];
	direction[2] = sinTheta * r.direction()[0] + cosTheta * r.direction()[2];

	ray rotated_r(origin, direction, r.time());

	if (!ptr->hit(rotated_r, t_min, t_max, rec)) return false;

	auto p = rec.p;
	auto normal = rec.normal;

	p[0] = cosTheta * rec.p[0] + sinTheta * rec.p[2];
	p[2] = -sinTheta * rec.p[0] + cosTheta * rec.p[2];

	normal[0] = cosTheta * rec.normal[0] + sinTheta * rec.normal[2];
	normal[2] = -sinTheta * rec.normal[0] + cosTheta * rec.normal[2];

	rec.p = p;
	rec.set_face_normal(rotated_r, normal);

	return true;
}

class flipFace : public hittable
{
public:
	flipFace(shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override
	{
		if (!ptr->hit(r, t_min, t_max, rec))
			return false;
		
		rec.front_face = !rec.front_face;
		return true;
	}

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
		return ptr->bounding_box(time0, time1, output_box);
	}

public:
	shared_ptr<hittable> ptr;
};