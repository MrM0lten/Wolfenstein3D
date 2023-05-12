#include "wolfenstein.h"

double	vector2d_len(double x, double y)
{
	return (sqrt(x * x + y * y));
}

int is_hit(double rx, double ry, meta_t* meta, int channel)
{
	if (rx < 0 || ry < 0)
		return 0;
	int mx = (int)rx>>6;
	int my = (int)ry>>6;
	int mp = my * meta->map->map_x + mx;
	if (mp >= 0 && mp <  meta->map->map_dim)
	{
		if (meta->map->map[mp] & channel)
			return meta->map->map[mp];
	}
	return 0;
}

point_t raycast_hor(double radian, meta_t* meta,int channel)
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
	for (int i = 0 ; i < MAX_RAY_ITER && !is_hit(rx, ry, meta,channel); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}
	return (point_t){rx,ry};
}

point_t raycast_ver(double radian, meta_t* meta,int channel)
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
	for (int i = 0; i < MAX_RAY_ITER && !is_hit(rx, ry, meta, channel); i++) {
		rx = rx + xo;
		ry = ry + yo;
	}
	return (point_t){rx,ry};
}


ray raycast(double radian, meta_t* meta,int channel)
{
	point_t hray = raycast_hor(radian, meta, channel);
	point_t vray = raycast_ver(radian, meta, channel);
	double len_hor = vector2d_len(hray.x - meta->player.pos.x,hray.y - meta->player.pos.y);
	double len_vert = vector2d_len(vray.x - meta->player.pos.x,vray.y - meta->player.pos.y);
	ray ray;

	ray.dir = radian;
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
	ray.hit_id = is_hit(ray.hit.x, ray.hit.y, meta, channel);
	return ray;
}

double angle_fix(double angle)
{
	if (angle > 2*PI)
		angle -= 2*PI;
	else if (angle < 0)
		angle += 2*PI;
	return angle;
}
//actually refactor this properly!
void raycaster(int nb_rays, double fov, ray *arr, meta_t *meta, int channel)
{
	player_t *p = &meta->player;
	for (int i = 0; i < nb_rays; i++) {
		double ray_angle =  angle_fix(p->a - (atan2(i-(nb_rays/2), meta->dist_to_proj)*-1));
		arr[i] = raycast(ray_angle, meta, channel);
		arr[i].len = arr[i].len * cos(p->a - ray_angle);
	}
}
