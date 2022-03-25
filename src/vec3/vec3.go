package vec3

import (
	"math"
)

type Vec3 struct {
	X float64
	Y float64
	Z float64
}

func(vec Vec3) Length() float64{
	return math.Sqrt(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z)
}

func(vec Vec3) SquaredLength() float64 {
	return vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z
}

func(vec *Vec3) Reverse() {
	vec.X *= -1
	vec.Y *= -1
	vec.Z *= -1
}

func(vec Vec3) Index(i int) float64 {
	switch i {
	case 0:
		return vec.X
	case 1:
		return vec.Y
	case 2:
		return vec.Z
	default:
		return 0.0
	}
}

func(vec *Vec3) MakeUnitVector() {
	k := float64( 1.0 / math.Sqrt(vec.X * vec.X + vec.Y * vec.Y + vec.Z * vec.Z) )
	vec.X *= k
	vec.Y *= k
	vec.Z *= k
}

func AddVectors(vec ...Vec3) Vec3 {
	temp := Vec3{0, 0, 0}
	
	for _, v := range vec {
		temp.X += v.X
		temp.Y += v.Y
		temp.Z += v.Z
	}

	return temp
}

func SubVectors(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z}
}

func MulVectors(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1.X * v2.X, v1.Y * v2.Y, v1.Z * v2.Z}
}

func DivVectors(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3{v1.X / v2.X, v1.Y / v2.Y, v1.Z / v2.Z}
}

func MulConstant(t float64, v Vec3) Vec3{
	return Vec3{t * v.X, t * v.Y, t * v.Z}
}

func DivConstant(t float64, v Vec3) Vec3{
	return Vec3{v.X / t, v.Y / t, v.Z / t}
}

func Dot(v1 Vec3, v2 Vec3) float64 {
	return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z
}

func Cross(v1 Vec3, v2 Vec3) Vec3 {
	return Vec3 {
		v1.Y * v2.Z - v1.Z * v2.Y, 
		-1 * (v1.X * v2.Z - v1.Z * v2.X), 
		v1.X * v2.Y - v1.Y * v2.X }
}

func(vec *Vec3) VectorAdd(v Vec3) {
	vec.X += v.X
	vec.Y += v.Y
	vec.Z += v.Z
}

func(vec *Vec3) VectorSub(v Vec3) {
	vec.X -= v.X
	vec.Y -= v.Y
	vec.Z -= v.Z
}

func(vec *Vec3) VectorMul(v Vec3) {
	vec.X *= v.X
	vec.Y *= v.Y
	vec.Z *= v.Z
}

func(vec *Vec3) VectorDiv(v Vec3) {
	vec.X /= v.X
	vec.Y /= v.Y
	vec.Z /= v.Z
}

func(vec *Vec3) ConstantMul(i float64) {
	vec.X *= i
	vec.Y *= i
	vec.Z *= i
}

func(vec *Vec3) ConstantDiv(i float64) {
	k := float64(1.0 / i)

	vec.X *= k
	vec.Y *= k
	vec.Z *= k
}

func UnitVector(v Vec3) Vec3 {
	return DivConstant(v.Length(), v)
}

