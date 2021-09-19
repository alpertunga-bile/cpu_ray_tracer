#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>

#include "include/helper.h"
#include "include/hittableList.h"
#include "include/sphere.h"
#include "include/moving_sphere.h"
#include "include/color.h"
#include "include/camera.h"
#include "include/material.h"
#include "include/texture.h"
#include "include/aarect.h"
#include "include/box.h"
#include "include/bvh.h"
#include "include/constant_medium.h"
#include "include/pdf.h"

hittableList randomScene()
{
	hittableList world;

	auto checker = make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++)
	{
		for (int b = -11; b < 11; b++)
		{
			auto chooseMaterial = random_double();

			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9)
			{
				shared_ptr<material> sphereMaterial;

				if (chooseMaterial < 0.8)
				{
					// diffuse
					auto albedo = color::randomVec3() * color::randomVec3();
					sphereMaterial = make_shared<lambertian>(albedo);
					auto center2 = center + vec3(0, random_double(0, 0.5), 0);
					world.add(make_shared<moving_sphere>(center, center2, 0.0, 1.0, 0.2, sphereMaterial));
				}
				else if (chooseMaterial < 0.95)
				{
					// metal
					auto albedo = color::randomVec3(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphereMaterial = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				}
				else
				{
					// glass
					sphereMaterial = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphereMaterial));
				}
			}
		}
	}

	auto glassMaterial = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, glassMaterial));

	auto lambertMaterial = make_shared<lambertian>(color(0.5, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, lambertMaterial));

	auto metalMaterial = make_shared<metal>(color(0.5, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, metalMaterial));

	return world;
}

hittableList twoSpheres()
{
	hittableList objects;

	auto checker = make_shared<checkerTexture>(color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));

	objects.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	objects.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

	return objects;
}

hittableList twoPerlinSpheres()
{
	hittableList objects;

	auto noiseTex = make_shared<noiseTexture>(4);

	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(noiseTex)));
	objects.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(noiseTex)));

	return objects;
}

hittableList earth()
{
	auto earth_texture = make_shared<imageTexture>("data/earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

	return hittableList(globe);
}

hittableList simpleLight()
{
	hittableList objects;

	auto perlinTexture = make_shared<noiseTexture>(2);
	objects.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(perlinTexture)));
	objects.add(make_shared<sphere>(point3(0,     2, 0),    2, make_shared<lambertian>(perlinTexture)));

	auto diffLight = make_shared<diffuseLight>(color(4, 4, 4));
	objects.add(make_shared<xy_rect>(3, 5, 1, 3, -2, diffLight));

	return objects;
}

hittableList cornellBox()
{
	hittableList objects;

	auto red = make_shared<lambertian>(color(0.65, 0.05, 0.05));
	auto green = make_shared<lambertian>(color(0.12, 0.45, 0.15));
	auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
	auto light = make_shared<diffuseLight>(color(15, 15, 15));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
	objects.add(make_shared<flipFace>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
	shared_ptr<hittable> columnBox = make_shared<box>(point3(0, 0, 0),
		point3(165, 330, 165), aluminum);
	columnBox = make_shared<rotate_y>(columnBox, 15);
	columnBox = make_shared<translate>(columnBox, vec3(265, 0, 295));
	objects.add(columnBox);

	auto glass = make_shared<dielectric>(1.5);
	objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

	return objects;
}

hittableList oneWeekFinal()
{
	hittableList boxes1;
	auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

	const int boxes_per_side = 20;
	for (int i = 0; i < boxes_per_side; i++)
	{
		for (int j = 0; j < boxes_per_side; j++)
		{
			auto w = 100.0f;
			auto x0 = -1000.0 + i * w;
			auto z0 = -1000.0 + j * w;
			auto y0 = 0.0;
			auto x1 = x0 + w;
			auto y1 = random_double(1, 101);
			auto z1 = z0 + w;

			boxes1.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
		}
	}

	hittableList objects;

	objects.add(make_shared<bvh>(boxes1, 0, 1));

	auto light = make_shared<diffuseLight>(color(7, 7, 7));
	objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

	auto center1 = point3(400, 400, 200);
	auto center2 = center1 + vec3(30, 0, 0);
	auto moving_sphere_mat = make_shared<lambertian>(color(0.7, 0.3, 0.1));
	objects.add(make_shared<moving_sphere>(center1, center2, 0, 1, 50, moving_sphere_mat));

	objects.add(make_shared<sphere>(point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
	objects.add(make_shared<sphere>(point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)));

	auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
	objects.add(boundary);
	objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
	boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
	objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

	auto earthMat = make_shared<lambertian>(make_shared<imageTexture>("data/earthmap.jpg"));
	objects.add(make_shared<sphere>(point3(400, 200, 400), 100, earthMat));
	auto perlinTexture = make_shared<noiseTexture>(0.1);
	objects.add(make_shared<sphere>(point3(220, 280, 300), 80, make_shared<lambertian>(perlinTexture)));

	hittableList boxes2;
	auto white = make_shared<lambertian>(color(0.73, 0.73, 0.73));
	for (int j = 0; j < 1000; j++)
		boxes2.add(make_shared<sphere>(point3::randomVec3(0, 165), 10, white));

	objects.add(make_shared<translate>(
		make_shared<rotate_y>(
			make_shared<bvh>(boxes2, 0.0, 1.0), 15),
			vec3(-100, 270, 395)
		)
	);

	return objects;
}

color ray_color(const ray& r, const color& background, const hittable& world, 
	shared_ptr<hittable> lights, int depth)
{
	hit_record rec;

	// if we ray's bounce limit return black color
	if (depth <= 0)
		return color(0, 0, 0);

	if (!world.hit(r, 0.001, infinity, rec))
		return background;

	scatter_record srec;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;

	if (srec.isSpecular)
		return srec.attenuation
		* ray_color(srec.specular_ray, background, world, lights, depth - 1);

	auto lightPtr = make_shared<hittablePdf>(lights, rec.p);
	mixturePdf p(lightPtr, srec.pdfPtr);

	ray scattered = ray(rec.p, p.generate(), r.time());
	auto pdfVal = p.value(scattered.direction());

	return emitted + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, lights, depth - 1) / pdfVal;
}

int main()
{
	std::ofstream file("outputs/raytracingrestofyourlifeFinal.ppm");

	///////////////////////////////////////////////////////////////////////////////////////////////
	// VARIABLES
	///////////////////////////////////////////////////////////////////////////////////////////////

	auto aspect_ratio = 16.0 / 9.0;
	int width = 400;
	int height = static_cast<int>(width / aspect_ratio);
	int samples_per_pixel = 100;
	const int max_depth = 50;
	color background = color(0, 0, 0);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// WORLD
	///////////////////////////////////////////////////////////////////////////////////////////////

	hittableList world;
	auto lights = make_shared<hittableList>();
	lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<material>()));
	lights->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<material>()));

	///////////////////////////////////////////////////////////////////////////////////////////////
	// CAMERA
	///////////////////////////////////////////////////////////////////////////////////////////////

	point3 lookfrom;
	point3 lookat;
	auto fov = 40.0f;
	vec3 vectorUp(0, 1, 0);
	auto distance_to_focus = 10.0;
	auto aperture = 0.0;

	switch (6)
	{
	case 1:
		std::cout << "Rendering RayTracingOneWeekendFinal\n";
		world = randomScene();
		background = color(0.7, 0.8, 1.0);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		fov = 20.0f;
		aperture = 0.1;
		break;
		
	case 2:
		world = twoSpheres();
		background = color(0.7, 0.8, 1.0);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		fov = 20.0f;
		break;

	case 3:
		world = twoPerlinSpheres();
		background = color(0.7, 0.8, 1.0);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		fov = 20.0f;
		break;

	case 4:
		world = earth();
		background = color(0.7, 0.8, 1.0);
		lookfrom = point3(13, 2, 3);
		lookat = point3(0, 0, 0);
		fov = 20.0f;
		break;

	case 5:
		world = simpleLight();
		samples_per_pixel = 400;
		background = color(0.0, 0.0, 0.0);
		lookfrom = point3(26, 3, 6);
		lookat = point3(0, 2, 0);
		fov = 20.0;
		break;

	case 6:
		std::cout << "Rendering Cornell Box\n";
		world = cornellBox();
		aspect_ratio = 1.0;
		width = 600;
		height = 600;
		samples_per_pixel = 100;
		background = color(0, 0, 0);
		lookfrom = point3(278, 278, -800);
		lookat = point3(278, 278, 0); 
		fov = 40.0;
		break;

	default:
	case 7:
		std::cout << "Rendering RayTracingOneWeekFinal\n";
		world = oneWeekFinal();
		aspect_ratio = 1.0;
		width = 600;
		height = 600;
		samples_per_pixel = 100;
		background = color(0, 0, 0);
		lookfrom = point3(478, 278, -600);
		lookat = point3(278, 278, 0);
		fov = 40.0;
		break;
	}

	std::cout << "Numbers Of Ray: " << samples_per_pixel << std::endl;

	camera cam(lookfrom, lookat, vectorUp, fov, aspect_ratio, aperture, distance_to_focus, 0.0, 1.0);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// IMAGE PROCESSING
	///////////////////////////////////////////////////////////////////////////////////////////////

	std::cout << width << "x" << height << " image processing..." << std::endl;

	file << "P3\n" << width << " " << height << "\n255\n";

	for (int j = height - 1; j >= 0; --j)
	{
		auto progress = (int)((double)((double)(height - 1 - j) / (height - 1)) * 100);

		std::cout << "\rProgress: %" << progress << " ";

		auto start = std::chrono::system_clock::now();

		for (int i = 0; i < width; ++i)
		{
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s)
			{
				auto u = (i + random_double()) / (width - 1);
				auto v = (j + random_double()) / (height - 1);

				ray r = cam.get_ray(u, v);

				pixel_color += ray_color(r, background, world, lights, max_depth);
			}

			write_color(file, pixel_color, samples_per_pixel);
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsedSeconds = end - start;

		std::cout << std::fixed;
		std::cout << std::setprecision(2);
		std::cout << ((double)(100 - progress)) * elapsedSeconds.count() / 60.0f << " minutes" << std::flush;
	}

	std::cerr << "\nDone!" << std::endl;

	file.close();

	return 0;
}
