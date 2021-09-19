#pragma once

#include "helper.h"

class perlin
{
public:
	perlin()
	{
		randomVec = new vec3[pointCount];

		for (int i = 0; i < pointCount; ++i)
			randomVec[i] = unit_vector(vec3::randomVec3(-1, 1));

		permX = perlinGeneratePerm();
		permY = perlinGeneratePerm();
		permZ = perlinGeneratePerm();
	}

	~perlin()
	{
		delete[] randomVec;
		delete[] permX;
		delete[] permY;
		delete[] permZ;
	}

	double noise(const point3& p) const
	{
		auto u = p.x() - floor(p.x());
		auto v = p.y() - floor(p.y());
		auto w = p.z() - floor(p.z());

		auto i = static_cast<int>(floor(p.x()));
		auto j = static_cast<int>(floor(p.y()));
		auto k = static_cast<int>(floor(p.z()));

		vec3 c[2][2][2];

		for (int di = 0; di < 2; di++)
		{
			for (int dj = 0; dj < 2; dj++)
			{
				for (int dk = 0; dk < 2; dk++)
				{
					c[di][dj][dk] = randomVec[
						permX[(i + di) & 255] ^
						permX[(j + dj) & 255] ^
						permX[(k + dk) & 255]
					];
				}
			}
		}

		return perlinInterp(c, u, v, w);
	}

	double turbulence(const point3& p, int depth = 7) const
	{
		auto accum = 0.0;
		auto tempP = p;
		auto weight = 1.0;

		for (int i = 0; i < depth; i++)
		{
			accum += weight * noise(tempP);
			weight *= 0.5;
			tempP *= 2;
		}

		return fabs(accum);
	}

private:
	static int* perlinGeneratePerm()
	{
		auto p = new int[pointCount];

		for (int i = 0; i < pointCount; i++)
			p[i] = i;

		permute(p, pointCount);

		return p;
	}

	static void permute(int* p, int n)
	{
		for (int i = n - 1; i > 0; i--)
		{
			int target = random_int(0, i);
			
			int tmp = p[i];
			p[i] = p[target];
			p[target] = tmp;
		}
	}

	static double trilinearInterp(double c[2][2][2], double u, double v, double w)
	{
		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					accum += (i * u + (1 - i) * (1 - u)) *
						(j * v + (1 - j) * (1 - v)) *
						(k * w + (1 - k) * (1 - w)) * c[i][j][k];
				}
			}
		}

		return accum;
	}

	static double perlinInterp(vec3 c[2][2][2], double u, double v, double w)
	{
		auto uu = u * u * (3 - 2 * u);
		auto vv = v * v * (3 - 2 * v);
		auto ww = w * w * (3 - 2 * w);
		
		auto accum = 0.0;

		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					vec3 weightV(u - i, v - j, w - k);
					accum += (i * uu + (1 - i) * (1 - uu))
						* (j * vv + (1 - j) * (1 - vv))
							* (k * ww + (1 - k) * (1 - ww))
							* dot(c[i][j][k], weightV);
				}
			}
		}

		return accum;
	}

private:
	static const int pointCount = 256;
	vec3* randomVec;
	int* permX;
	int* permY;
	int* permZ;
};