#pragma once

#include "hittable.h"
#include "onb.h"
#include "pdf.h"

class sphere : public hittable
{
public:
	sphere() {}
	sphere(point3 cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {};

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
	virtual double pdf_value(const point3& o, const vec3& v) const override
	{
		hit_record rec;
		if (!this->hit(ray(o, v), 0.001, infinity, rec)) return 0;

		auto cosThetaMax = sqrt(1 - radius * radius / (center - o).length_squared());
		auto solidAngle = 2 * PI * (1 - cosThetaMax);

		return 1 / solidAngle;
	}
	virtual vec3 random(const vec3& o) const override
	{
		vec3 direction = center - o;
		auto distanceSquared = direction.length_squared();

		onb uvw;
		uvw.buildFrom(direction);

		return uvw.local(random_to_sphere(radius, distanceSquared));
	}

private:
	static void getSphereUV(const point3& p, double& u, double& v)
	{
		auto theta = acos(-p.y());
		auto phi = atan2(-p.z(), p.x()) + PI;

		u = phi / (2 * PI);
		v = theta / PI;
	}

public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	// sphere equation
	// t^2 * dot(B, B) + 2 * t * dot(B, (A - C)) + dot(A - C, A - C) - R * R = 0

	// A - C
	vec3 oc = r.origin() - center;

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
	vec3 out_normal = (rec.p - center) / radius;
	rec.set_face_normal(r, out_normal);
	getSphereUV(out_normal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool sphere::bounding_box(double time0, double time1, aabb& output_box) const
{
	output_box = aabb(
		center - vec3(radius, radius, radius),
		center + vec3(radius, radius, radius)
	);

	return true;
}