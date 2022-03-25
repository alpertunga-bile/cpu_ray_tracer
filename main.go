package main

import (
	"fmt"
	"os"
	"strconv"
	"time"
	"math"
	"vec3"
	"ray"
	"hitable"
	"hitablelist"
	"sphere"
)

func color(r ray.Ray, world hitable.HitRecord) vec3.Vec3 {
	rec := hitable.HitRecord{}

	if world.Hit(r, 0.0, math.MaxFloat64, &rec) {
		return vec3.MulConstant(0.5, vec3.Vec3{X: rec.Normal.X + 1, Y: rec.Normal.Y + 1, Z: rec.Normal.Z + 1})
	} else {
		unit_direction := vec3.UnitVector(r.Direction())
		t := 0.5 * (unit_direction.Y + 1.0)
		return vec3.AddVectors(vec3.MulConstant(1.0 - t, vec3.Vec3{X: 1.0, Y: 1.0, Z: 1.0}), vec3.MulConstant(t, vec3.Vec3{X: 0.5, Y: 0.7, Z: 1.0}))
	} 
} 

func main()  {
	file, err := os.Create("outputs/5.ppm")
	if err != nil {
		panic(err)
	}

	defer file.Close()

	nx:=200
	ny:=100

	lower_left_corner := vec3.Vec3{X: -2.0, Y: -1.0, Z: -1.0}
	horizontal := vec3.Vec3{X: 4.0, Y: 0.0, Z: 0.0}
	vertical := vec3.Vec3{X: 0.0, Y: 2.0, Z: 0.0}
	origin := vec3.Vec3{X: 0.0, Y: 0.0, Z: 0.0}

	list := [2]sphere.Sphere{}
	list[0] = sphere.Sphere{Center: vec3.Vec3{X: 0, Y: 0, Z: -1}, Radius: 0.5}
	list[1] = sphere.Sphere{Center: vec3.Vec3{X: 0, Y: -100.5, Z: -1}, Radius: 100}

	hitable.HitRecord = hitablelist.HitableList

	fmt.Println("Writing to file ...")

	start:=time.Now()

	_, err = file.WriteString("P3\n" + strconv.Itoa(nx) + " " + strconv.Itoa(ny) + "\n255\n")

	for j:=ny-1; j >= 0; j-- {
		for i:=0; i < nx; i++ {
			u := float64(i) / float64(nx)
			v := float64(j) / float64(ny)

			sum := []vec3.Vec3{lower_left_corner, vec3.MulConstant(u, horizontal), vec3.MulConstant(v, vertical)}
			r := ray.Ray{A: origin, B: vec3.AddVectors(sum...)}

			col := color(r)

			ir:=int(255.99 * col.X)
			ig:=int(255.99 * col.Y)
			ib:=int(255.99 * col.Z)

			_, err = file.WriteString(strconv.Itoa(ir) + " " + strconv.Itoa(ig) + " " + strconv.Itoa(ib) + "\n")
		}
	}

	end:=time.Now()
	elapsed:=end.Sub(start)

	fmt.Println("Finished in (seconds) ", elapsed.Seconds())
}