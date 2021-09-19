#pragma once

#include "perlin.h"
#include "stb_image.h"

#include <iostream>

class texture
{
public:
	virtual color value(double u, double v, const point3& p) const = 0;
};

class imageTexture : public texture
{
public:
	imageTexture() : data(nullptr), width(0), height(0), bytesPerScanline(0) {}

	imageTexture(const char* path)
	{
		auto componentsPerPixel = bytesPerPixel;

		data = stbi_load(path, &width, &height, &componentsPerPixel, componentsPerPixel);

		if (!data)
		{
			std::cerr << "::ERROR::STBI_IMAGE::\nCan't Load " << path << " Image\n";
			width = height = 0;
		}

		bytesPerScanline = bytesPerPixel * width;
	}

	~imageTexture()
	{
		delete data;
	}

	virtual color value(double u, double v, const point3& p) const override
	{
		if (data == nullptr) return color(0, 1, 1);

		u = clamp(u, 0.0, 1.0);
		// flip V to image coordinates
		v = 1.0 - clamp(v, 0.0, 1.0);

		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);

		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		const auto colorScale = 1.0f / 255.0f;
		auto pixel = data + j * bytesPerScanline + i * bytesPerPixel;

		return color(colorScale * pixel[0], colorScale * pixel[1], colorScale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height;
	int bytesPerScanline;

public:
	const static int bytesPerPixel = 3;
};

class solid_color : public texture
{
public:
	solid_color() {}

	solid_color(color c) : color_value(c) {}

	solid_color(double red, double green, double blue)
		: solid_color(color(red, green, blue)) {}

	virtual color value(double u, double v, const point3& p) const override
	{
		return color_value;
	}

private:
	color color_value;
};

class checkerTexture : public texture
{
public:
	checkerTexture() {}

	checkerTexture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
		: even(_even), odd(_odd) {}

	checkerTexture(color c1, color c2)
		: even(make_shared<solid_color>(c1)), odd(make_shared<solid_color>(c2)) {}

	virtual color value(double u, double v, const point3& p) const override
	{
		auto sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());

		if (sines < 0)
			return odd->value(u, v, p);
		else
			return even->value(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};

class noiseTexture : public texture
{
public:
	noiseTexture() {}
	noiseTexture(double sc) : scale(sc) {}

	virtual color value(double u, double v, const point3& p) const override
	{
		return color(1, 1, 1) * 0.5 * (1 + sin(scale * p.z() + 10 * noise.turbulence(p)));
	}

public:
	perlin noise;

	// control frequency
	double scale;
};

