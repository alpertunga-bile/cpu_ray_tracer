package hitable

import (
	"vec3"
	"ray"
)

type HitRecord struct {
	T float64
	P vec3.Vec3
	Normal vec3.Vec3
}

type hitable interface {
	Hit(r ray.Ray, t_min float64, t_max float64, rec *HitRecord) bool
}
func (hr HitRecord) {
	return true
}