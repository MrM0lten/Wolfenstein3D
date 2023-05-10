#include "wolfenstein.h"

inline void draw_ray(point_t ray, meta_t *meta) {
	drawline(meta->main_scene, meta->player.pos, ray, 0xFF00FFFF);
}

double	vector2d_len(double x, double y)
{
	return (sqrt(x * x + y * y));
}

void print_raydata()
{
	printf("-----------------PRINTING RAY DATA---------\n");
	for (int i = 0; i < RAYS; i++) {
		//printf("Ray [%i]; len=[%f]; hit_type=[%i]\n",i,ray_data[i].len,ray_data[i].hit_dir);
	}
}

//note: jean changed the return value to the map pos in array to diff wall from door
int is_wall(double rx, double ry, meta_t* meta)
{
	if (rx < 0 || ry < 0)
		return 0;
	int mx = (int)rx>>6;
	int my = (int)ry>>6;
	int mp = my * meta->map->map_x + mx;
	if (mp >= 0 && mp <  meta->map->map_dim && (meta->map->map[mp] == GD_WALL || meta->map->map[mp] == GD_DOOR_OPEN)) {
		return meta->map->map[mp];
	}
	return 0;
}

// 0000 -> 1100
// if map_value == 1 do operation 1000_2 & 1100_2 = true
// if map_value == 2 do operation 0100_2 & 1100_2 = true
// if map_value == 3 do operation 0010_2 & 1100_2 = false


// Mask
/*
	WALL		:= 1 << 0
	DOOR_CLOSE	:= 1 << 1
	DOOR_OPEN	:= 1 << 2
	HIDDEN_DOOR	:= 1 << 3

*/

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
/* 	debug_meta(meta);
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
	} */
}

// int is_wall2(point_t origin, double x_dist, double y_dist, map_t *map)
// {

// }

// ray ray_build()
// {

// }

// ray raycast2(point_t origin, double angle, map_t *map)
// {
// 	double ray_dir_x =






// 	double x_dist, y_dist, d_x, d_y;
// 	d_y = tan(angle), d_x = 1/tan(angle);
// 	if (angle > PI/2 && angle < 1.5*PI) {x_dist = ((int)origin.x>>6)<<6;}
// 	else {x_dist = ((int)origin.x>>6)<<6 + 64;}
// 	if (angle > PI) {y_dist = ((int)origin.y>>6)<<6;}
// 	else {y_dist = ((int)origin.y>>6)<<6 + 64;}

// 	int i = 0;
// 	while (i < 8 && !is_wall2(origin, x_dist, y_dist, map)) {
// 		if (x_dist < y_dist) {
// 			x_dist += d_x;
// 		}
// 		else {
// 			y_dist += d_y;
// 		}
// 		i++;
// 	}
// 	return ray_build(angle, x_dist, y_dist);
// }

// void raycaster2(raycaster2_t *rc)
// {
// 	for (int i = 0; i < rc->nb_rays; i++) {
// 		double ray_angle =  angle_fix(rc->angle - atan2(i-256, rc->plane_dist));
// 		rc->rays[i] = raycast2(rc->origin, ray_angle, rc->map);
// 		rc->rays[i].len = rc->rays[i].len * cos(rc->angle - ray_angle);
// 	}
// }
