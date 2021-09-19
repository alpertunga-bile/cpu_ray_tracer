#pragma once

#include "texture.h"
#include "onb.h"
#include "pdf.h"

struct hit_record;

struct scatter_record
{
    ray specular_ray;
    bool isSpecular;
    color attenuation;
    shared_ptr<pdf> pdfPtr;
};

class material
{
public:
    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const
    {
        return color(0, 0, 0);
    }

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const
    {
        return false;
    }

    virtual double scattering_pdf
    (
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const 
    {
        return 0;
    }
};


class lambertian : public material
{
public:
	lambertian(const color& a) : albedo(make_shared<solid_color>(a)) {}
    lambertian(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override 
    {
        srec.isSpecular = false;
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);
        srec.pdfPtr = make_shared<cosinePdf>(rec.normal);

        return true;
    }

    virtual double scattering_pdf
    (
        const ray& r_in, const hit_record& rec, const ray& scattered
    ) const override
    {
        auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
        return cosine < 0 ? 0 : cosine / PI;
    }

public:
	shared_ptr<texture> albedo;
};

class metal : public material
{
public:
    metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);

        srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
        srec.attenuation = albedo;
        srec.isSpecular = true;
        srec.pdfPtr = 0;

        return true;
    }

public:
    color albedo;
    double fuzz;
};

class dielectric : public material
{
public:
    dielectric(double index_of_refraction) : ir(index_of_refraction) {}

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override
    {
        srec.isSpecular = true;
        srec.pdfPtr = nullptr;
        srec.attenuation = color(1.0, 1.0, 1.0);

        double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;

        vec3 unit_direction = unit_vector(r_in.direction());
        double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

        bool isReflect = refraction_ratio * sin_theta > 1.0;
        vec3 direction;

        if (isReflect || reflectance(cos_theta, refraction_ratio) > random_double())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        srec.specular_ray = ray(rec.p, direction, r_in.time());

        return true;
    }

private:
    static double reflectance(double cosine, double ref_idx)
    {
        // Schlick approximation
        auto r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;

        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }

public:
    double ir;
};

// simulate light
class diffuseLight : public material
{
public:
    diffuseLight(shared_ptr<texture> a) : emit(a) {}

    diffuseLight(color c) : emit(make_shared<solid_color>(c)) {}

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
    ) const override
    {
        return false;
    }

    virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override
    {
        if (rec.front_face)
            return emit->value(u, v, p);
        else
            return color(0, 0, 0);
    }

public:
    shared_ptr<texture> emit;
};

class isotropic : public material
{
public:
    isotropic(color c) : albedo(make_shared<solid_color>(c)) {}
    isotropic(shared_ptr<texture> a) : albedo(a) {}

    virtual bool scatter
    (
        const ray& r_in, const hit_record& rec, scatter_record& srec
	) const override
    {
        srec.isSpecular = false;
        srec.pdfPtr = nullptr;
        srec.specular_ray = ray(rec.p, random_in_unit_sphere(), r_in.time());
        srec.attenuation = albedo->value(rec.u, rec.v, rec.p);

        return true;
    }

public:
    shared_ptr<texture> albedo;
};