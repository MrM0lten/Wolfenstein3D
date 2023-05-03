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

void draw_wall(mlx_image_t *image, ray *ray,mlx_texture_t *texture, point_t screen_pos, float wall_height)
{
	uint32_t col;
	int x_text;
	if(ray->hit_dir == DIR_NORTH || ray->hit_dir == DIR_SOUTH)
		x_text = (int)ray->hit.x % 64;
	else
		x_text = (int)ray->hit.y % 64;

	int start_pos_y = (int)((IMG_HEIGHT - wall_height)/2);
	int end_pos_y = (int)((IMG_HEIGHT + wall_height)/2);
	float delta =(float)(end_pos_y - start_pos_y)/64;
	float it = delta;
	int y_text = 0;
	for (int i = 0; i < end_pos_y - start_pos_y; i++)
	{
		it--;
		if (it < 0) {
			it += delta;
			y_text++;
		}
		col = (uint32_t)texture->pixels[y_text * texture->width * 4 + 0 + (x_text * 4)] << 3 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 1 + (x_text * 4)] << 2 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 2 + (x_text * 4)] << 1 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 3 + (x_text * 4)] << 0 * 8;
		//printf("pixel pos [%d][%d]\n", screen_pos.x, i + start_pos_y);
		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + start_pos_y}, col);
	}
}

void draw_wall_flip(mlx_image_t *image, ray *ray, mlx_texture_t *texture, point_t screen_pos, float wall_height)
{
	uint32_t col;
	int x_text;
	if(ray->hit_dir == DIR_NORTH || ray->hit_dir == DIR_SOUTH)
		x_text = 64 - ((int)ray->hit.x % 64) - 1;
	else
		x_text = 64 - ((int)ray->hit.y % 64) - 1;

	int start_pos_y = (int)((IMG_HEIGHT - wall_height) / 2);
	int end_pos_y = (int)((IMG_HEIGHT + wall_height) / 2);
	float delta =(float)(end_pos_y - start_pos_y) / 64;
	float it = delta;
	int y_text = 0;
	for (int i = 0; i < end_pos_y - start_pos_y; i++)
	{
		it--;
		if (it < 0) {
			it += delta;
			y_text++;
		}
		col = (uint32_t)texture->pixels[y_text * texture->width * 4 + 0 + (x_text * 4)] << 3 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 1 + (x_text * 4)] << 2 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 2 + (x_text * 4)] << 1 * 8
			| (uint32_t)texture->pixels[y_text * texture->width * 4 + 3 + (x_text * 4)] << 0 * 8;
		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + start_pos_y}, col);
	}
}

void draw_debugplayer(mlx_image_t *img, player_t *player,int ratio)
{
	int p_size = 6;
	point_t p_gridpos ={(player->pos.x/ratio),(player->pos.y/ratio)};
	for (int i = 0; i < p_size; i++)
	{
		for (int j = 0; j < p_size; j++)
			my_mlx_put_pixel(img,(point_t){p_gridpos.x+j-p_size/2,p_gridpos.y+i-p_size/2},0xFF0000FF);
	}

	float line_mult = 5;
	//debug_player(player);
	drawline(img, p_gridpos, (point_t){p_gridpos.x+player->dx*line_mult, p_gridpos.y+player->dy*line_mult},0xFF0000FF);
}

int find_next_pow(int val)
{
	int n = 1;
	while(n < val)
		n *= 2;
	return n;
}

void draw_debugmap(void *param)
{
	meta_t *meta = param;
	debug_t* debug = &meta->debugmap;
	for (int i = 0; i < meta->map->map_x; i++) {
		for (int j = 0; j < meta->map->map_y; j++) {
			if (meta->map->map[j * meta->map->map_x + i] == 1) {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, 0xA9A9A9FF, 0x303030FF);
			}
			else {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, 0xFFFFFFFF, 0x303030FF);
			}


		}
	}

	point_t p_gridpos ={(meta->player.pos.x/debug->ratio),(meta->player.pos.y/debug->ratio)};
	for (int i = 0; i < meta->raycaster.num_rays; i++)
		drawline(meta->debugmap.img,p_gridpos,(point_t){meta->raycaster.rays[i].hit.x/debug->ratio,meta->raycaster.rays[i].hit.y/debug->ratio},0xFF00FFFF);
	draw_debugplayer(meta->debugmap.img, &meta->player,debug->ratio);

}

void draw_square(mlx_image_t *image, point_t start, int len, uint32_t fill, uint32_t bor)
{
	// Filling
	for (int y = 0; y < len; y++) {
		for (int x = 0; x < len; x++) {
			my_mlx_put_pixel(image, (point_t){start.x + x,start.y + y}, fill);
		}
	}
	// Border note: removed top and left border otherwise line thickness is 2
	for (int i = 0; i < len; i++) {
		//my_mlx_put_pixel(image, (point_t){start.x + i, start.y}, bor);
		my_mlx_put_pixel(image, (point_t){start.x + i, start.y + len - 1}, bor);
		//my_mlx_put_pixel(image, (point_t){start.x, start.y + i}, bor);
		my_mlx_put_pixel(image, (point_t){start.x + len - 1, start.y + i}, bor);
	}
}

//later refactor of draw_scene function
/* void draw_vertical_slice()
{
	draw_ceiling();
	draw_wall();
	draw_floor();
} */

void draw_scene(void *param)
{
	meta_t* meta = param;
	//printf("In draw_scene\n");
	raycaster_t *rayc = &meta->raycaster;
	point_t wall_upper;
	point_t wall_lower;
	float wall_height;
	raycaster(meta->raycaster.num_rays, meta->player.fov, meta->raycaster.rays, meta);
	//debug_raycaster(rayc);
	for (int i = 0; i < meta->raycaster.num_rays; i++) {
		wall_height = (64 * meta->dist_to_proj)/(rayc->rays[i].len);
		//printf("wallheight = %f\n", wall_height);
		wall_upper.x = i;
		wall_upper.y = (int)(meta->win_height - wall_height) / 2;
		wall_lower.x = i;
		wall_lower.y = (int)(meta->win_height + wall_height) / 2;

		//printf("Before first drawline\n");
		drawline(meta->main_scene, (point_t){i, 0}, wall_upper, meta->map->col_ceil);
		//printf("After first drawline\n");
		if (rayc->rays[i].hit_dir == DIR_NORTH)
			draw_wall(meta->main_scene, &rayc->rays[i], meta->map->texture_north, wall_upper, wall_height);
		else if(rayc->rays[i].hit_dir == DIR_SOUTH)
			draw_wall_flip(meta->main_scene, &rayc->rays[i], meta->map->texture_south, wall_upper, wall_height);
		else if(rayc->rays[i].hit_dir == DIR_WEST)
			draw_wall_flip(meta->main_scene, &rayc->rays[i], meta->map->texture_west, wall_upper, wall_height);
		else
			draw_wall(meta->main_scene, &rayc->rays[i], meta->map->texture_east, wall_upper, wall_height);
		//printf("Before second drawline\n");
		drawline(meta->main_scene, wall_lower, (point_t){i, meta->win_height}, meta->map->col_floor);
		//printf("After second drawline\n");
	}
	//printf("exiting\n");
}

//drawline(meta->image_window,(point_t){(int)(i+HALF_SCREEN),0},(int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),map->col_ceil);
