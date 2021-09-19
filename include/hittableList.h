#pragma once

#include <memory>
#include <vector>

#include "hittable.h"

using std::shared_ptr;
using std::make_shared;

class hittableList : public hittable
{
public:
	hittableList() {}
	hittableList(shared_ptr<hittable> object) { add(object); }

	void clear() { objects.clear(); }

	void add(shared_ptr<hittable> object) { objects.push_back(object); }

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override;
	virtual double pdf_value(const point3& o, const vec3& v) const override
	{
		auto weight = 1.0 / objects.size();
		auto sum = 0.0;

		for (const auto& object : objects)
			sum += weight * object->pdf_value(o, v);

		return sum;
	}
	
	virtual vec3 random(const vec3& o) const override
	{
		auto intSize = static_cast<int>(objects.size());
		return objects[random_int(0, intSize - 1)]->random(o);
	}

public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittableList::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
	hit_record temp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& object : objects)
	{
		if (object->hit(r, t_min, closest_so_far, temp_rec))
		{
			hit_anything = true;
			closest_so_far = temp_rec.t;
			rec = temp_rec;
		}
	}

	return hit_anything;
}

bool hittableList::bounding_box(double time0, double time1, aabb& output_box) const
{
	if (objects.empty()) return false;

	aabb temp_box;
	bool first_box = true;

	for (const auto& object : objects)
	{
		if (!object->bounding_box(time0, time1, temp_box)) return false;
		output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
		first_box = false;
	}

	return true;
}
