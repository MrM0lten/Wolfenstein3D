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

	//debug_point(&ray_grid_pos);
	if(map->map[(int)ray_grid_pos.y * map->map_x + (int)ray_grid_pos.x] == GD_DOOR_CLOSE)
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

	for (int i = 0; i < (int)wall_height + 1; i++)
	{
		it += delta;
		y_text = (int)it;
		if(y_text >= 63) //setting to max value of text array
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

void draw_sprite(meta_t* meta, sprite_t* sprite)
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


	// mlx_texture_t* texture = get_text_from_hit(map,ray);
	// int x_text = get_texture_offset_x(ray); //already knows about flipped or not
	// int y_text = 0;

	// //calculate what direction to read from
	// float delta = CUBE_DIM / wall_height;
	// float it = 0;

	// for (int i = 0; i < (int)wall_height; i++)
	// {
	// 	it += delta;
	// 	y_text = (int)it;
	// 	if(y_text >= 63) //setting to max value of text array
	// 		y_text = 63;

	// 	my_mlx_put_pixel(image, (point_t){screen_pos.x, i + screen_pos.y},
	// 	get_color_from_text(texture, x_text, y_text, col_default, effect));

	int projected_height = meta->dist_to_proj * (64) / vector2d_len(sp.x,sp.y);
//	int	projected_width = (projected_height / sprite->texture->height) * sprite->texture->width;
	int	projected_width = (projected_height * sprite->texture->width) / sprite->texture->height;

	printf("Projected_height = %d\n", projected_height);
	printf("Projected_width = %d\n", projected_width);
	printf("texture_height = %d\n", sprite->texture->height);
	printf("texture_width = %d\n", sprite->texture->width);
	//	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);

	float dx = (float)sprite->texture->width / projected_width;
	float dy = (float)sprite->texture->height / projected_height;

	float itx = 0, ity = 0;
	int x_text = 0, y_text = 0;

	for (int x = 0; x < projected_width-1; x++) {
		itx += dx;
		x_text = (int)itx;
		ity = 0;
		y_text = 0;
		if(vector2d_len(sp.x,sp.y) >= meta->raycaster.rays[screen_x - (projected_width/2) + x].len) //only draw sprite if its closer than current wall at that pixel collumn
				continue;
		//printf("x_text = %d\n", x_text);
		for (int y = 0; y < projected_height-1; y++) {
			ity += dy;
			y_text = (int)ity;
			//printf("dy = %f\n", dy);
			//unsigned col = 0;
			unsigned int col = get_color_from_text(sprite->texture, x_text, y_text, col_default,NULL);
			if (col != 0x980088FF)
				my_mlx_put_pixel(meta->main_scene, (point_t){screen_x - (projected_width/2) + x, screen_y - projected_height + y}, col);
		}
	}

	// printf("meta->dist_to_proj =[%f]\n",meta->dist_to_proj);
	// printf("proportion =[%d]\n",projected_height);
	// draw_square(meta->main_scene,(point_t){screen_x-projected_height/2,screen_y -projected_height},projected_height,0xFF00FFFF,0x00000000);
}

int compare( const void* a, const void* b)
{
	sprite_t *a1 = a1;
	sprite_t *b1 = b1;
     double int_a = a1->len;
     double int_b = b1->len;

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

void swap_sprites(sprite_t** a, sprite_t** b)
{
	sprite_t* temp = *a;
	*a = *b;
	*b = temp;
}

void sort_sprites(sprite_t** sprites,int total_sprites)
{
	for (int i = 0; i < total_sprites - 1; i++) {
		for (int j = 0; j < total_sprites -1; j++) {
			printf("sprites[%d].len=%f\n",j,sprites[j]->len);
			printf("sprites[%d].len=%f\n",j+1,sprites[j + 1]->len);
			if(sprites[j]->len < sprites[j + 1]->len) {
				printf("SPRITES SWAPPED\n");
				swap_sprites(&sprites[j],&sprites[j+1]);
			}
		}
	}
}


void draw_sprites(meta_t *meta, player_t *player, sprite_t **sprite_arr, int size)
{
	for (int i = 0; i < meta->tot_sprites; i++) {
		sprite_arr[i]->len = vector2d_len(sprite_arr[i]->pos.x- player->pos.x, sprite_arr[i]->pos.y - player->pos.y);
	}
	sort_sprites(sprite_arr,meta->tot_sprites);
	for (int i = 0; i < meta->tot_sprites; i++) {
		draw_sprite(meta, sprite_arr[i]);
	}
	for (int i = 0; i < meta->tot_sprites; i++) {
		printf("SPRITES = len at [%d] = [%f]\n",i,sprite_arr[i]->len);
	}
	
/* 	double len1 = vector2d_len(sprite_arr[0].pos.x- player->pos.x, sprite_arr[0].pos.y - player->pos.y);
	double len2 = vector2d_len(sprite_arr[1].pos.x- player->pos.x, sprite_arr[1].pos.y - player->pos.y);
	// for (int i = 0; i < size; i++) {
	// 	sprite_arr[i].len = vector2d_len(sprite_arr[i].pos.x- player->pos.x, sprite_arr[i].pos.y - player->pos.y);
	// 	printf("calculated len of [%d]= %f\n",i, sprite_arr[i].len);
	// }
	//qsort(sprite_arr, size, sizeof(sprite_t), compare);
	// for (int i = 0; i < size; i++) {
	// 	draw_sprite(meta, &sprite_arr[i]);
	// }
	if (len1 < len2) {
		draw_sprite(meta, &sprite_arr[1]);
		draw_sprite(meta, &sprite_arr[0]);
	}
	else {
		draw_sprite(meta, &sprite_arr[0]);
		draw_sprite(meta, &sprite_arr[1]);
	} */
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
	raycaster(meta->raycaster.num_rays, meta->player.fov, meta->raycaster.rays, meta,GD_WALL | GD_DOOR_CLOSE);
	for (int i = 0; i < meta->raycaster.num_rays; i++) {
/* 		for (int j = 0; j < delta; j++)
		{
			pos = i * delta + j; */

			wall_height = ((CUBE_DIM-PLAYER_HEIGHT) * meta->dist_to_proj)/(rayc->rays[i].len);
			wall_upper.x = i;
			wall_upper.y = (int)(meta->win_height - wall_height) / 2;
			wall_lower.x = i;
			wall_lower.y = (int)(meta->win_height + wall_height) / 2;
			drawline(meta->main_scene, (point_t){i, 0}, wall_upper, meta->map->col_ceil);
			// if (rayc->rays[i].hit_dir == DIR_NORTH || rayc->rays[i].hit_dir == DIR_SOUTH)
			// 	drawline(meta->main_scene, wall_upper, wall_lower, 0x52a447FF);
			// else
			// 	drawline(meta->main_scene, wall_upper, wall_lower, 0x46923cFF);
			draw_wall_on_steroids(meta->main_scene, meta->map, &rayc->rays[i], wall_upper, wall_height, &meta->shading_lut[(int)rayc->rays[i].len]);
			drawline(meta->main_scene, wall_lower, (point_t){i, meta->win_height}, meta->map->col_floor);
		//}
	}
	draw_sprites(meta, &meta->player, meta->sprite_data, meta->tot_sprites);
}

