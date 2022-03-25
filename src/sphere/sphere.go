package sphere

import (
	"hitable"
	"vec3"
	"ray"
	"math"
)

type Sphere struct {
	Center vec3.Vec3
	Radius float64
}

func(s Sphere) Hit(r ray.Ray, t_min float64, t_max float64, rec *hitable.HitRecord) bool {
	oc := vec3.SubVectors(r.Origin(), s.Center)

	a := vec3.Dot(r.Direction(), r.Direction())
	b := 2.0 * vec3.Dot(oc, r.Direction())
	c := vec3.Dot(oc, oc) - s.Radius * s.Radius
	discriminant := b * b - a * c 

	if discriminant > 0 {
		temp := (-b - math.Sqrt(b * b - a * c)) / a
		if temp < t_max && temp > t_min {
			rec.T = temp
			rec.P = r.PointAtParameter(rec.T)
			rec.Normal = vec3.DivConstant(s.Radius, vec3.SubVectors(rec.P, s.Center))
			return true
		}

		temp = (-b + math.Sqrt(b * b - a * c)) / a
		if temp < t_max && temp > t_min {
			rec.T = temp
			rec.P = r.PointAtParameter(rec.T)
			rec.Normal = vec3.DivConstant(s.Radius, vec3.SubVectors(rec.P, s.Center))
			return true
		}
	}

	return false
}