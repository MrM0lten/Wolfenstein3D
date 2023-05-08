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
	//(void)effect;
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
	//printf("hit dir = %d\n",ray->hit_dir);

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
		if(y_text >= 63) //setting to max value of text array
			y_text = 63;

		my_mlx_put_pixel(image, (point_t){screen_pos.x, i + screen_pos.y},
		get_color_from_text(texture, x_text, y_text, col_default, effect));
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

//later refactor of draw_scene function
/* void draw_vertical_slice()
{
	draw_ceiling();
	draw_wall();
	draw_floor();
} */
double dotProd(point_t a,point_t b)
{
	return a.x * b.x + a.y * b.y;
}

void draw_sprite(meta_t* meta,sprite_t* sprite)
{
	point_t sp = (point_t){sprite->pos.x- meta->player.pos.x,sprite->pos.y- meta->player.pos.y};

	double a = atan2(sp.y,sp.x);
	if (a < 0)
		a += 2 * PI;
	if (a > 2*PI)
		a -= 2*PI;
	double p = a - (meta->player.a - (meta->player.fov/2));
	if (p < 0)
		p += 2 * PI;
	if (p > 2*PI)
		p -= 2*PI;
	printf("relative angle %f\n", p);

	int screen_x = p /(PI/2) * IMG_WIDTH;
	//int screen_y = meta->dist_to_proj * 32 /vector2d_len(sp.x,sp.y);
	//int screen_y = (20 *meta->dist_to_proj)/vector2d_len(sp.x,sp.y);
	//int screen_y = ((IMG_HEIGHT - 10 +IMG_HEIGHT/2) -(vector2d_len(sp.x,sp.y))*(cos(p)/sin(meta->player.fov/2)))/2;
	//screen_y = (screen_height - sprite_height / (distance * cos(vertical_angle - player_vertical_angle))) / 2
/* 	debug_point(&sp);
	printf("sprite angle = [%f]\n",a);
	printf("ratio = [%f]\n",p /(PI/2));
	printf("player angle = [%f]\n",meta->player.a);
	printf("Screen Pos = [%d][%d]\n",screen_x,screen_y);
 */
	//meta->dist_to_proj * 32 /vector2d_len(sp.x,sp.y)


	point_t lookdir = (point_t) {cos(meta->player.a),sin(meta->player.a)};

	double dist_to_obj_fwd = dotProd(sp,lookdir);
	double yFOV = 2 * atan(tan(meta->player.fov/2)* (IMG_WIDTH/IMG_HEIGHT));
	double dist_to_obj_vert = dist_to_obj_fwd * tan(yFOV);
	double zOffset_to_cam = -86;
	double halfheight = IMG_HEIGHT/2 ;
	int screen_y = halfheight + halfheight * zOffset_to_cam/dist_to_obj_vert;

	debug_point(&lookdir);
	printf("A =[%f]\n",dist_to_obj_fwd);
	printf("yFOV =[%f]\n",yFOV);
	printf("B =[%f]\n",dist_to_obj_vert);
	printf("D =[%f]\n",halfheight);
	printf("Screen y =[%d]\n",screen_y);

	int  projected_height = meta->dist_to_proj * (CUBE_DIM/2) / vector2d_len(sp.x,sp.y);
	//	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);
	printf("meta->dist_to_proj =[%f]\n",meta->dist_to_proj);

	printf("proportion =[%d]\n",projected_height);

	draw_square(meta->main_scene,(point_t){screen_x-projected_height/2,screen_y -projected_height},projected_height,0xFF00FFFF,0x00000000);
}

void draw_scene(void *param)
{
	meta_t* meta = param;
	//printf("In draw_scene\n");
	raycaster_t *rayc = &meta->raycaster;
	point_t wall_upper;
	point_t wall_lower;
	float wall_height;
	float delta = IMG_WIDTH/meta->raycaster.num_rays;
	//printf("delta = [%f]\n",delta);
	float pos = 0;
	raycaster(meta->raycaster.num_rays, meta->player.fov, meta->raycaster.rays, meta);
	for (int i = 0; i < meta->raycaster.num_rays; i++) {
		for (int j = 0; j < delta; j++)
		{
			pos = i * delta + j;

			wall_height = ((CUBE_DIM-PLAYER_HEIGHT) * meta->dist_to_proj)/(rayc->rays[i].len);
			wall_upper.x = pos;
			wall_upper.y = (int)(meta->win_height - wall_height) / 2;
			wall_lower.x = pos;
			wall_lower.y = (int)(meta->win_height + wall_height) / 2;
			drawline(meta->main_scene, (point_t){pos, 0}, wall_upper, meta->map->col_ceil);
			draw_wall_on_steroids(meta->main_scene, meta->map, &rayc->rays[i], wall_upper, wall_height, &meta->shading_lut[(int)rayc->rays[i].len]);
			drawline(meta->main_scene, wall_lower, (point_t){pos, meta->win_height}, meta->map->col_floor);
		}
	}
	draw_sprite(meta,&meta->sprite_data[0]);
}

