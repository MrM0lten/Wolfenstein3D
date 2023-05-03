#include "wolfenstein.h"

inline void draw_ray(point_t ray, meta_t *meta) {
	drawline(meta->main_scene, meta->player.pos, ray, 0xFF00FFFF);
}

double	vector2d_len(double x, double y)
{
	return (sqrt(pow(x, 2) + pow(y, 2)));
}

void print_raydata()
{
	printf("-----------------PRINTING RAY DATA---------\n");
	for (int i = 0; i < RAYS; i++) {
		//printf("Ray [%i]; len=[%f]; hit_type=[%i]\n",i,ray_data[i].len,ray_data[i].hit_dir);
	}
}


int is_wall(double rx, double ry, meta_t* meta)
{
	if (rx < 0 || ry < 0)
		return 0;
	int mx = (int)rx>>6;
	int my = (int)ry>>6;
	int mp = my * meta->map->map_x + mx;
	if (mp >= 0 && mp <  meta->map->map_dim && meta->map->map[mp] == 1) {
		return 1;
	}
	return 0;
}

point_t raycast_hor(double radian, meta_t* meta)
{
	double px = meta->player.pos.x, py = meta->player.pos.y, rx, ry, xo, yo;
	if (radian > PI) {
		ry = (((int)py>>6)<<6)-0.002;
		rx = px-((py-ry)/tan(radian));
		yo = -CUBE_DIM, xo = yo/tan(radian);
	}
	else if (radian < PI) {
		ry = (((int)py>>6)<<6) + CUBE_DIM;
		rx = px-((py-ry)/tan(radian));
		yo = CUBE_DIM, xo = yo/tan(radian);
	}
	if (radian == 0 || radian == PI) {return (point_t){px,py};}
	for (int i = 0 ; i < MAX_RAY_ITER && !is_wall(rx, ry, meta); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}
	return (point_t){rx,ry};
}

point_t raycast_ver(double radian, meta_t* meta)
{
	double px = meta->player.pos.x, py = meta->player.pos.y, rx, ry, xo, yo;
	if (radian > PI/2 && radian < 1.5*PI) {
		rx = (((int)px>>6)<<6)-0.002;
		ry = py - (tan(radian) * (px-rx));
		xo = -CUBE_DIM, yo = tan(radian) * xo;
	}
	else if (radian < PI/2 || radian > 1.5*PI) {
		rx = (((int)px>>6)<<6) + CUBE_DIM;
		ry = py - (tan(radian) * (px-rx));
		xo = CUBE_DIM, yo = tan(radian) * xo;
	}
	if (radian == PI / 2 || radian == PI * 1.5) {return (point_t){px,py};}
	for (int i = 0; i < MAX_RAY_ITER && !is_wall(rx, ry, meta); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}

	return (point_t){rx,ry};
}


ray raycast(double radian, meta_t* meta)
{
	point_t hray = raycast_hor(radian, meta);
	point_t vray = raycast_ver(radian, meta);
	double len_hor = vector2d_len(hray.x - meta->player.pos.x,hray.y - meta->player.pos.y);
	double len_vert = vector2d_len(vray.x - meta->player.pos.x,vray.y - meta->player.pos.y);
	ray ray;

	if (len_hor == 0 || len_hor > len_vert) {
		ray.hit = vray;
		ray.len = len_vert;
		if(ray.dir > PI/2 && ray.dir < PI*1.5)
			ray.hit_dir = DIR_WEST;
		else
			ray.hit_dir = DIR_EAST;
	}
	else {
		ray.hit = hray;
		ray.len = len_hor;
		if(ray.dir > PI)
			ray.hit_dir = DIR_NORTH;
		else
			ray.hit_dir = DIR_SOUTH;
	}
	ray.dir = radian;
	return ray;
}

//actually refactor this properly!
void raycaster(int nb_rays, double fov,ray *arr,meta_t *meta)
{
	//debug_meta(meta);
	if (nb_rays == 1) {
		arr[0] = raycast(meta->player.a, meta);
		return;
	}
	double start_radian = meta->player.a-fov/2;
	double step = fov / (nb_rays -1);
	for (int i=0; i < nb_rays; i++) {
		if (start_radian > 2*PI)
			start_radian -= 2*PI;
		else if (start_radian < 0)
			start_radian += 2*PI;
		arr[i] = raycast(start_radian, meta);
		double temp = meta->player.a - start_radian;
		if(temp > 2*PI)
			temp -= 2*PI;
		else if(temp < 0)
			temp += 2*PI;
		arr[i].len = arr[i].len * cos(temp);
		start_radian += step;
	}
}
