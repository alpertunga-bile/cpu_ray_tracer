#pragma once

#include "helper.h"
#include "hittable.h"

class moving_sphere : public hittable
{
public:
	moving_sphere() {}
	moving_sphere(point3 cen, point3 cen1, double _time, double _time1, double r, shared_ptr<material> m)
		: center(cen), center1(cen1), time(_time), time1(_time1), radius(r), mat_ptr(m)
	{}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double _time0, double _time1, aabb& output_box) const override;

	point3 getCenter(double time) const
	{
		return center + ((time - this->time) / (time1 - this->time)) * (center1 - center);
	}

public:
	point3 center, center1;
	double time, time1;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	// sphere equation
	// t^2 * dot(B, B) + 2 * t * dot(B, (A - C)) + dot(A - C, A - C) - R * R = 0

	// A - C
	vec3 oc = r.origin() - getCenter(r.time());

	// we can say b is even because if you dot with itself it is equal to length of vector
	// in discriminant if we put 2h to b we can see that b and 4 can be divided

	// dot(B, B)
	auto a = r.direction().length_squared();
	// dot(B, A - C)
	auto half_b = dot(oc, r.direction());
	// dot(A - C, A - C) - R * R
	auto c = oc.length_squared() - radius * radius;

	// in discriminant if we put 2h to b we can see that b and 4 can be divided
	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0)
		return false;
	auto dis_sqrt = sqrt(discriminant);

	auto root = (-half_b - dis_sqrt) / a;
	if (root < t_min || root > t_max)
	{
		// be sure it is on not sphere
		root = (-half_b + dis_sqrt) / a;
		if (root < t_min || root > t_max)
			return false;
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	auto out_normal = (rec.p - getCenter(r.time())) / radius;
	rec.set_face_normal(r, out_normal);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool moving_sphere::bounding_box(double _time0, double _time1, aabb& output_box) const
{
	aabb box(
		getCenter(_time0) - vec3(radius, radius, radius),
		getCenter(_time0) + vec3(radius, radius, radius)
	);

	aabb box1(
		getCenter(_time1) - vec3(radius, radius, radius),
		getCenter(_time1) + vec3(radius, radius, radius)
	);

	output_box = surrounding_box(box, box1);

	return true;
}