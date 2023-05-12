#include "wolfenstein.h"

sprite_t* init_sprite(float pos_x,float pos_y, char *filepath, char *filepath2)
{
	if(filepath == NULL || access(filepath, F_OK | R_OK)
	|| filepath2 == NULL || access(filepath2, F_OK | R_OK)){

		log_string(1,1,"Tried adding sprite, but cant find file.");
		return NULL;
	}

	sprite_t* new = malloc(sizeof(sprite_t));
	new->texture_01 = mlx_load_png(filepath);
	if(!new->texture_01) {
		free(new);
		log_string(1,1,"Load PNG for sprite failed.");
		return NULL;
	}
	new->texture_02 = mlx_load_png(filepath2);
	if(!new->texture_02) {
		free(new->texture_01);
		free(new);
		log_string(1,1,"Load PNG for sprite failed.");
		return NULL;
	}
	new->pos.x = pos_x;
	new->pos.y = pos_y;
	return new;
}

void add_sprite(meta_t* meta,float pos_x,float pos_y, char *filepath, char *filepath2)
{
	sprite_t* new = init_sprite(pos_x,pos_y,filepath,filepath2);
	if(!new)
		return;

	sprite_t** arr = malloc(sizeof(sprite_t*) * (meta->tot_sprites + 1));
	for (int i = 0; i < meta->tot_sprites; i++)
		arr[i] = meta->sprite_data[i];
	arr[meta->tot_sprites] = new;
	if(meta->sprite_data != NULL)
		free(meta->sprite_data);
	meta->sprite_data = arr;
	meta->tot_sprites++;

}

point_t sprite_screen_pos(meta_t *meta, sprite_t *sprite, point_t sp)
{
	point_t pos;

	double a = angle_fix(atan2(sp.y,sp.x));
	double p = angle_fix(a - (meta->player.a - (meta->player.fov/2)));

	int screen_x = p /(PI/2) * IMG_WIDTH;
	pos.x = screen_x;

	point_t lookdir = (point_t) {cos(meta->player.a),sin(meta->player.a)};

	double dist_to_obj_fwd = dotProd(sp,lookdir);
	double yFOV = 2 * atan(tan(meta->player.fov/2)* (IMG_WIDTH/IMG_HEIGHT));
	double dist_to_obj_vert = dist_to_obj_fwd * tan(yFOV);
	double zOffset_to_cam = -86;
	double halfheight = IMG_HEIGHT/2 ;
	int screen_y = halfheight + halfheight * zOffset_to_cam/dist_to_obj_vert;
	pos.y = screen_y;
	return pos;
}

mlx_texture_t *set_texture(sprite_t *sprite)
{
	return get_time() % 2 ? sprite->texture_01 : sprite->texture_02;
}

void draw_sprite(meta_t* meta, sprite_t* sprite)
{
	point_t sp = (point_t){sprite->pos.x- meta->player.pos.x,sprite->pos.y- meta->player.pos.y};
	point_t screen = sprite_screen_pos(meta, sprite, sp);
	if(screen.x <= 0 || screen.x >= 1024) //if sprite is offscreen, no need to draw it
		return;

	int projected_height = meta->dist_to_proj * (64) / (vector2d_len(sp.x,sp.y) * cos(meta->player.a - angle_fix(atan2(sp.y, sp.x))));
	int	projected_width = (projected_height * sprite->texture_01->width) / sprite->texture_01->height;
	float dx = (float)sprite->texture_01->width / projected_width;
	float dy = (float)sprite->texture_01->height / projected_height;
	float itx = 0, ity = 0;
	int x_text = 0, y_text = 0;
	int ray_index;

	mlx_texture_t* read_texture = set_texture(sprite);
	for (int x = 0; x < projected_width - 1; x++) {
		itx += dx;
		x_text = (int)itx;
		ity = 0;
		y_text = 0;
		ray_index = screen.x - (projected_width/2) + x;
		if(ray_index <= 0 || ray_index >= RAYS || vector2d_len(sp.x,sp.y) >= meta->raycaster.rays[ray_index].len) //only draw sprite if its closer than current wall at that pixel collumn
				continue;
		for (int y = 0; y < projected_height-1; y++) {
			ity += dy;
			y_text = (int)ity;
			unsigned int col = get_color_from_text(read_texture, x_text, y_text, col_default,NULL);
			if (col != 0x980088FF)
				my_mlx_put_pixel(meta->main_scene, (point_t){screen.x - (projected_width/2) + x, screen.y - projected_height + y}, col);
		}
	}
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
			if(sprites[j]->len < sprites[j + 1]->len) {
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
}
