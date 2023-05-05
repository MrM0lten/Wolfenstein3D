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
uint8_t col_lut_mult(uint8_t col, void* effect)
{
	(void)effect;
	return (col * *(float*)effect);
}

//a simple function that simply returns its color without changing the pixel itself
inline uint8_t col_default(uint8_t col, void* effect)
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

//returns a texture within the map texture_data structure based on a given ray, will take direction into account
mlx_texture_t* get_text_from_hit(map_t* map, ray* ray)
{
	point_t ray_grid_pos = (point_t){ray->hit.x/CUBE_DIM,ray->hit.y/CUBE_DIM};

	//debug_point(&ray_grid_pos);
	if(map->map[(int)ray_grid_pos.y * map->map_x + (int)ray_grid_pos.x] == GD_DOOR)
		return map->texture_data[TXT_DOOR];
	printf("hit dir = %d\n",ray->hit_dir);

	//note this works because the hit_dir enum corresponds to the right array elements
	return map->texture_data[ray->hit_dir];
}

//depending on the direction of the ray and its hit position calculate which x pos of a texture slice to use
int get_texture_offset_x(ray* ray)
{
	//note: if texture is south or west the wall needs to be flipped
	//hence the offset x needs to come from the other side

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

	return -99; //should never get here!
}
//note: using the wall height as iterator might have a plus minus one issue for for loop
void draw_wall_on_steroids(mlx_image_t *image, map_t* map,ray* ray, point_t screen_pos, float wall_height, void* effect)
{
	mlx_texture_t* texture = get_text_from_hit(map,ray);
	int x_text = get_texture_offset_x(ray); //already knows about flipped or not
	int y_text = 0;

	//calculate what direction to read from
	float delta = CUBE_DIM / wall_height;
	float it = 0;

	for (int i = 0; i < (int)wall_height; i++)
	{
		it += delta;
		y_text = (int)it;
		if(y_text >= 64) //setting to max value of text array
			y_text = 63;

		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + screen_pos.y},
		get_color_from_text(texture,x_text,y_text,col_default,&effect[(int)ray->len]));
	}

}


void draw_wall(mlx_image_t *image, ray *ray,mlx_texture_t *texture, point_t screen_pos, float wall_height,float* color_lut)
{
	uint32_t col;
	int x_text;
	if(ray->hit_dir == DIR_NORTH || ray->hit_dir == DIR_SOUTH)
		x_text = (int)ray->hit.x % CUBE_DIM;
	else
		x_text = (int)ray->hit.y % CUBE_DIM;

	int start_pos_y = (int)((IMG_HEIGHT - wall_height)/2);
	int end_pos_y = (int)((IMG_HEIGHT + wall_height)/2);
	int total_pixel_to_draw = end_pos_y - start_pos_y;
	float delta =(float)(total_pixel_to_draw)/CUBE_DIM;
	float it = delta;
	int y_text = 0;
	//printf("ytext start [%d], end [%d], start -end [%d]\n",start_pos_y,end_pos_y,total_pixel_to_draw);
	//printf("delta = [%f]\n",delta);
	float negdelta = (float)CUBE_DIM/total_pixel_to_draw;
	float negit = negdelta;
	//printf("\n");
	//printf("wallheight vs total_pixel_to_draw [%f][%d]",wall_height,total_pixel_to_draw);
	for (int i = 0; i < total_pixel_to_draw; i++)
	{
		if(total_pixel_to_draw >= 64) {
			it--;
			if (it < 0) {
				it += delta;
				y_text++;
			}
		}
		else {
			//printf("in here\n");
			negit += negdelta;
			y_text = (int)negit;
			if(y_text >= 64) //setting to max value of text array
				y_text = 63;
			//printf("ytext = [%d]\n",y_text);
		}
		col = (uint32_t)(texture->pixels[y_text * texture->width * 4 + 0 + (x_text * 4)] * color_lut[(int)ray->len]) << 3 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 1 + (x_text * 4)] * color_lut[(int)ray->len]) << 2 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 2 + (x_text * 4)] * color_lut[(int)ray->len]) << 1 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 3 + (x_text * 4)]) << 0 * 8  ;
		//printf("RAY [%f][%f]\n", ray->hit.x, ray->hit.y);

		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + start_pos_y}, col);
	}
}

void draw_wall_flip(mlx_image_t *image, ray *ray, mlx_texture_t *texture, point_t screen_pos, float wall_height,float* color_lut)
{
	uint32_t col;
	int x_text;
	if(ray->hit_dir == DIR_NORTH || ray->hit_dir == DIR_SOUTH)
		x_text = CUBE_DIM - ((int)ray->hit.x % CUBE_DIM) - 1;
	else
		x_text = CUBE_DIM - ((int)ray->hit.y % CUBE_DIM) - 1;

	int start_pos_y = (int)((IMG_HEIGHT - wall_height) / 2);
	int end_pos_y = (int)((IMG_HEIGHT + wall_height) / 2);
	int total_pixel_to_draw = end_pos_y - start_pos_y;
	float delta =(float)(end_pos_y - start_pos_y) / CUBE_DIM;
	float it = delta;
	int y_text = 0;
	float negdelta = (float)CUBE_DIM/total_pixel_to_draw;
	float negit = negdelta;
	for (int i = 0; i < end_pos_y - start_pos_y; i++)
	{
		if(total_pixel_to_draw >= 64) {
			it--;
			if (it < 0) {
				it += delta;
				y_text++;
			}
		}
		else {
			//printf("in here\n");
			negit += negdelta;
			y_text = (int)negit;
			if(y_text >= 64) //setting to max value of text array
				y_text = 63;
			//printf("ytext = [%d]\n",y_text);
		}
		col = (uint32_t)(texture->pixels[y_text * texture->width * 4 + 0 + (x_text * 4)]* color_lut[(int)ray->len]) << 3 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 1 + (x_text * 4)]* color_lut[(int)ray->len]) << 2 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 2 + (x_text * 4)]* color_lut[(int)ray->len]) << 1 * 8
			| (uint32_t)(texture->pixels[y_text * texture->width * 4 + 3 + (x_text * 4)]) << 0 * 8;
		//printf("col = %u\n",col);
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
			if (meta->map->map[j * meta->map->map_x + i] == GD_WALL) {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_WALL, DBG_GRID_BORDER);
			}
			else if(meta->map->map[j * meta->map->map_x + i] == GD_DOOR)
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_DOOR, DBG_GRID_BORDER);
			else if(meta->map->map[j * meta->map->map_x + i] == GD_VOID)
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_VOID, DBG_GRID_BORDER);
			else {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_FREE, DBG_GRID_BORDER);
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
	for (int i = 0; i < meta->raycaster.num_rays; i++) {
		wall_height = ((CUBE_DIM-PLAYER_HEIGHT) * meta->dist_to_proj)/(rayc->rays[i].len);
		wall_upper.x = i;
		wall_upper.y = (int)(meta->win_height - wall_height) / 2;
		wall_lower.x = i;
		wall_lower.y = (int)(meta->win_height + wall_height) / 2;

		drawline(meta->main_scene, (point_t){i, 0}, wall_upper, meta->map->col_ceil);

		//trial
		draw_wall_on_steroids(meta->main_scene,meta->map,&rayc->rays[i],wall_upper,wall_height,meta->shading_lut);

		drawline(meta->main_scene, wall_lower, (point_t){i, meta->win_height}, meta->map->col_floor);
	}
}

