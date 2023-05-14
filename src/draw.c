#include "wolfenstein.h"

void my_mlx_put_pixel(mlx_image_t *img, point_t pos, int color)
{
	char	*dst;

	if (pos.x > IMG_WIDTH -1 || pos.x < 0)
		return ;
	if (pos.y > IMG_HEIGHT -1 || pos.y < 0)
		return ;
	mlx_put_pixel(img, (int)pos.x, (int)pos.y, color);
}

void drawline(mlx_image_t* image, point_t start, point_t end, int color)
{
	int dx = end.x - start.x;
	int dy = end.y - start.y;
	double x, y, xinc, yinc;
	int steps;

	if (abs(dx) > abs(dy))
		steps = abs(dx);
	else
		steps = abs(dy);
	xinc = (double)dx / steps;
	yinc = (double)dy / steps;
	x = start.x;
	y = start.y;
	while (steps--) {
		my_mlx_put_pixel(image, (point_t){x, y}, color);
		x = x + xinc;
		y = y + yinc;
	}
}

//a simple function that simply returns its color without changing the pixel itself
uint8_t col_default(uint8_t col, void* effect)
{
	(void)effect;
	return col;
}

uint32_t get_color_from_text(mlx_texture_t *texture, int x, int y,uint8_t (*f)(uint8_t, void*), void* transform)
{
	return( (uint32_t)f(texture->pixels[y * texture->width * 4 + 0 + (x * 4)],transform)  << 3 * 8
		  | (uint32_t)f(texture->pixels[y * texture->width * 4 + 1 + (x * 4)],transform)  << 2 * 8
		  | (uint32_t)f(texture->pixels[y * texture->width * 4 + 2 + (x * 4)],transform) << 1 * 8
		  | (uint32_t)texture->pixels[y * texture->width * 4 + 3 + (x * 4)] << 0 * 8 );
}

int darken(int col, float modifier) {
    int r = (col & 0xFF000000) >> 24;
    int g = (col & 0x00FF0000) >> 16;
    int b = (col & 0x0000FF00) >> 8;
    int a = col & 0x000000FF;

	r *= modifier;
	g *= modifier;
	b *= modifier;

    int result = (r << 24) | (g << 16) | (b << 8) | a;
    return result;
}

//returns a texture within the map texture_data structure based on a given ray, will take direction into account
mlx_texture_t* get_text_from_hit(map_t* map, ray* ray)
{
	point_t ray_grid_pos = (point_t){ray->hit.x/CUBE_DIM,ray->hit.y/CUBE_DIM};

	if(map->map[(int)ray_grid_pos.y * map->map_x + (int)ray_grid_pos.x] == GD_DOOR_CLOSE)
		return map->texture_data[TXT_DOOR];
	return map->texture_data[ray->hit_dir];
}

//depending on the direction of the ray and its hit position calculate which x pos of a texture slice to use
int get_texture_offset_x(ray* ray)
{
	switch (ray->hit_dir) {
	case DIR_NORTH:
		return((int)ray->hit.x % CUBE_DIM);
	case DIR_SOUTH:
		return CUBE_DIM - ((int)ray->hit.x % CUBE_DIM) - 1;
	case DIR_EAST:
		return((int)ray->hit.y % CUBE_DIM);
	case DIR_WEST:
		return CUBE_DIM - ((int)ray->hit.y % CUBE_DIM) - 1;
	}

	return -99;
}

//note: using the wall height as iterator might have a plus minus one issue for for loop
void draw_wall(mlx_image_t *image, map_t* map,ray* ray, point_t screen_pos, float wall_height, void* effect)
{
	mlx_texture_t* texture = get_text_from_hit(map,ray);
	int x_text = get_texture_offset_x(ray);
	int y_text = 0;
	float delta = CUBE_DIM / wall_height;
	float it = 0;

	for (int i = 0; i < (int)wall_height + 1; i++)
	{
		it += delta;
		y_text = (int)it;
		if(y_text >= 63)
			y_text = 63;

		if (ray->hit_dir == DIR_NORTH || ray->hit_dir == DIR_SOUTH) {
			my_mlx_put_pixel(image, (point_t){screen_pos.x, i + screen_pos.y},
			darken(get_color_from_text(texture, x_text, y_text, col_default, effect), 0.7));
		}
		else {
			my_mlx_put_pixel(image, (point_t){screen_pos.x, i + screen_pos.y},
			get_color_from_text(texture, x_text, y_text, col_default, effect));
		}
	}
}

double dotProd(point_t a,point_t b)
{
	return a.x * b.x + a.y * b.y;
}

void draw_scene(void *param)
{
	meta_t* meta = param;
	raycaster_t *rayc = &meta->raycaster;
	point_t wall_upper;
	point_t wall_lower;
	float wall_height;
	float delta = IMG_WIDTH/meta->raycaster.num_rays;
	float pos = 0;
	raycaster(meta->raycaster.num_rays, meta->player.fov, meta->raycaster.rays, meta,GD_WALL | GD_DOOR_CLOSE);
	//debug_meta(meta);
	for (int i = 0; i < meta->raycaster.num_rays; i++) {
		wall_height = ((CUBE_DIM-PLAYER_HEIGHT) * meta->dist_to_proj)/(rayc->rays[i].len);
		wall_upper.x = i;
		wall_upper.y = (int)(meta->win_height - wall_height) / 2;
		wall_lower.x = i;
		wall_lower.y = (int)(meta->win_height + wall_height) / 2;
		drawline(meta->main_scene, (point_t){i, 0}, wall_upper, meta->map->col_ceil);
		draw_wall(meta->main_scene, meta->map, &rayc->rays[i], wall_upper, wall_height, NULL);
		drawline(meta->main_scene, wall_lower, (point_t){i, meta->win_height}, meta->map->col_floor);
	}
	draw_sprites(meta, &meta->player, meta->sprite_data, meta->tot_sprites);
}

