package ray

import (
	"vec3"
)

type Ray struct {
    A vec3.Vec3
	B vec3.Vec3
}

func(r Ray) Origin() vec3.Vec3{
	return r.A
}

func(r Ray) Direction() vec3.Vec3 {
	return r.B
}

func(r Ray) PointAtParameter(t float64) vec3.Vec3 {
	return vec3.AddVectors(r.A, vec3.MulConstant(t, r.B))
}