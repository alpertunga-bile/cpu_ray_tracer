package hitablelist

import (
	"hitable"
	"vec3"
	"ray"
)

type HitableList struct {
	ListSize int
	List []hitable.HitRecord
	T float64
	P vec3.Vec3
	Normal vec3.Vec3
}

func(hl HitableList) Hit(r ray.Ray, t_min float64, t_max float64, rec *hitable.HitRecord) bool {
	
	temp_rec := hitable.HitRecord{T: 0, P: vec3.Vec3{X: 0, Y: 0, Z: 0}, Normal: vec3.Vec3{X: 0, Y: 0, Z: 0}}

	hit_anything := false
	closest_so_far := t_max

	for i:=0; i < hl.ListSize; i++ {
		if hl.List[i].Hit(r, t_min, closest_so_far, &temp_rec) {
			hit_anything = true
			closest_so_far = temp_rec.T
			rec = &temp_rec
		}
	}

	return hit_anything
} 