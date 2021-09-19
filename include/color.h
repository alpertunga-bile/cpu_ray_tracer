#pragma once

#include "helper.h"

#include <iostream>
#include <fstream>

void write_color(std::ofstream& out, color pixel_color, int samples_per_pixel)
{
	auto r = pixel_color.x();
	auto g = pixel_color.y();
	auto b = pixel_color.z();

	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	auto scale = 1.0 / samples_per_pixel;
	r = sqrt(r * scale);
	g = sqrt(g * scale);
	b = sqrt(b * scale);

	out << static_cast<int>(255.999 * clamp(r, 0.0, 0.999)) << " "
		<< static_cast<int>(255.999 * clamp(g, 0.0, 0.999)) << " "
		<< static_cast<int>(255.999 * clamp(b, 0.0, 0.999)) << "\n";
}