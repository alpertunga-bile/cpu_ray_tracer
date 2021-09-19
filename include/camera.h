#pragma once

#include "helper.h"

class camera
{
public:
	camera(point3 lookfrom, point3 lookat, vec3 vectorUp, double vertical_fov, 
		double aspect_ratio, double aperture, double focus_distance, double _time = 0, double _time1 = 0)
	{
		auto theta = degrees2radians(vertical_fov);
		auto h = tan(theta / 2);

		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = unit_vector(lookfrom - lookat);
		u = unit_vector(cross(vectorUp, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = focus_distance * viewport_width * u;
		vertical = focus_distance * viewport_height * v;
		lower_left_corner = origin - (horizontal / 2) - (vertical / 2) - focus_distance * w;

		lens_radius = aperture / 2;

		time = _time;
		time1 = _time1;
	}

	ray get_ray(double s, double t)
	{
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x() + v * rd.y();

		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, random_double(time, time1));
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double lens_radius;
	double time, time1;
};