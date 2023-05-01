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

void draw_wall(mlx_image_t *image, ray ray,mlx_texture_t *texture, point_t screen_pos)
{
	uint32_t col;
	int x_text;
	if(ray.hit_dir == DIR_NORTH || ray.hit_dir == DIR_SOUTH)
		x_text = (int)ray.hit.x % 64;
	else
		x_text = (int)ray.hit.y % 64;

	int start_pos_y = (int)((IMG_HEIGHT - ray.len)/2);
	int end_pos_y = (int)((IMG_HEIGHT + ray.len)/2);
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
		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + start_pos_y}, col);
	}
}

void draw_wall_flip(mlx_image_t *image, ray ray, mlx_texture_t *texture, point_t screen_pos)
{
	uint32_t col;
	int x_text;
	if(ray.hit_dir == DIR_NORTH || ray.hit_dir == DIR_SOUTH)
		x_text = 64 - ((int)ray.hit.x % 64);
	else
		x_text = 64 - ((int)ray.hit.y % 64);

	int start_pos_y = (int)((IMG_HEIGHT - ray.len) / 2);
	int end_pos_y = (int)((IMG_HEIGHT + ray.len) / 2);
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

void draw_player(mlx_image_t *img, player_t *player)
{
	int p_size = 6;
	for (int i = 0; i < p_size; i++)
	{
		for (int j = 0; j < p_size; j++)
			mlx_put_pixel(img, player->pos.x+j-p_size/2, player->pos.y+i-p_size/2, 0xFF0000FF);
	}

	float line_mult = 5;
	drawline(img, player->pos, (point_t){player->pos.x+player->dx*line_mult,player->pos.y+player->dy*line_mult},0xFF0000FF);
}

void draw_minimap(void *param)
{
	meta_t *meta = param;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if (meta->map->map[j * meta->map->map_x + i] == 1) {
				draw_square(meta->image_window, (point_t){i * 64, j * 64}, 64, 0xA9A9A9FF, 0x303030FF);
			}
			else {
				draw_square(meta->image_window, (point_t){i * 64, j * 64}, 64, 0xFFFFFFFF, 0x303030FF);
			}


		}
	}
	//		draw_square(meta->image_window, j, i, meta->map->map[(i*8)+j]);

	draw_player(meta->image_window, &meta->player);

	raycaster(meta->raycaster.num_rays, meta->player.fov, meta->raycaster.rays, meta);
	draw_scene(meta);
}

//NOT CLEAN
// void draw_fps_counter(mlx_t *mlx)
// {
// 	//draw fps counter
// 	char *test;
// 	test = ft_itoa(nbFrames);
// 	//mlx_put_string(mlx,test,768,256);
// 	mlx_delete_image(mlx, image_counter);
// 	image_counter = mlx_put_string(mlx,test,IMG_WIDTH -32,8);
// 	free(test);
// }

void draw_square(mlx_image_t *image, point_t start, int len, uint32_t fill, uint32_t bor)
{
	// Filling
	for (int y = 0; y < len; y++) {
		for (int x = 0; x < len; x++) {
			my_mlx_put_pixel(image, (point_t){start.x + x,start.y + y}, fill);
		}
	}
	// Border
	for (int i = 0; i < len; i++) {
		my_mlx_put_pixel(image, (point_t){start.x + i, start.y}, bor);
		my_mlx_put_pixel(image, (point_t){start.x + i, start.y + len - 1}, bor);
		my_mlx_put_pixel(image, (point_t){start.x, start.y + i}, bor);
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

void draw_scene(meta_t *meta)
{
	raycaster_t *raycaster = &meta->raycaster;
	point_t wall_upper;
	point_t wall_lower;
	for (int i = 0; i < meta->raycaster.num_rays; i++) {

		wall_upper.x = meta->win_width/2 + i;
		wall_upper.y = (int)(meta->win_height - raycaster->rays[i].len) / 2;

		wall_lower.x = meta->win_width/2 + i;
		wall_lower.y = (int)(meta->win_height + raycaster->rays[i].len) / 2;


		drawline(meta->image_window, (point_t){meta->win_width/2 + i, 0}, wall_upper, meta->map->col_ceil);

		if (raycaster->rays[i].hit_dir == DIR_NORTH) 
			draw_wall(meta->image_window, raycaster->rays[i], meta->map->texture_north, wall_upper);
		else if(raycaster->rays[i].hit_dir == DIR_SOUTH)
			draw_wall_flip(meta->image_window, raycaster->rays[i], meta->map->texture_south, wall_upper);
		else if(raycaster->rays[i].hit_dir == DIR_WEST)
			draw_wall_flip(meta->image_window, raycaster->rays[i], meta->map->texture_west, wall_upper);
		else
			draw_wall(meta->image_window, raycaster->rays[i], meta->map->texture_east, wall_upper);

		drawline(meta->image_window, wall_lower, (point_t){meta->win_width/2 + i, meta->win_height}, meta->map->col_floor);
	}
}

//drawline(meta->image_window,(point_t){(int)(i+HALF_SCREEN),0},(int)(i+HALF_SCREEN),(int)((IMG_HEIGHT- ray_data[i].len)/2),map->col_ceil);
