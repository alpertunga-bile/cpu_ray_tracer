#pragma once

#include "hittable.h"
#include "material.h"
#include "texture.h"

class constant_medium : public hittable
{
public:
    constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a)
    : boundary(b), negativeInverseDensity(-1 / d), phaseFunction(make_shared<isotropic>(a)) {}

    constant_medium(shared_ptr<hittable> b, double d, color c)
    : boundary(b), negativeInverseDensity(-1 / d), phaseFunction(make_shared<isotropic>(c)) {}

    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override
    {
        return boundary->bounding_box(time0, time1, output_box);
    }

public:
    shared_ptr<hittable> boundary;
    shared_ptr<material> phaseFunction;
    double negativeInverseDensity;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec) const
{
    hit_record rec1, rec2;

    if(!boundary->hit(r, -infinity, infinity, rec1)) return false;

    if(!boundary->hit(r, rec1.t + 0.0001, infinity, rec2)) return false;

    if(rec1.t < t_min) rec1.t = t_min;
    if(rec2.t > t_max) rec2.t = t_max;

    if(rec1.t >= rec2.t) return false;

    if(rec1.t < 0) rec1.t = 0;

    const auto rayLength = r.direction().length();
    const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
    const auto hitDistance = negativeInverseDensity * log(random_double());

    if(hitDistance > distanceInsideBoundary) return false;

    rec.t = rec1.t + hitDistance / rayLength;
    rec.p = r.at(rec.t);

    rec.normal = vec3(1, 0, 0);
    rec.front_face = true;
    rec.mat_ptr = phaseFunction;

    return true;
}