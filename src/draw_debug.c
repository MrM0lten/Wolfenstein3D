#include "wolfenstein.h"

void draw_debugsprite(mlx_image_t* img,sprite_t* sprite, int ratio,uint32_t col)
{
    point_t gridpos = (point_t){sprite->pos.x/ratio,sprite->pos.y/ratio};
    int size = 5;
    drawline(img,(point_t){gridpos.x-size, gridpos.y - size}, (point_t){gridpos.x + size, gridpos.y + size}, col);
    drawline(img,(point_t){gridpos.x-size, gridpos.y + size}, (point_t){gridpos.x + size, gridpos.y - size}, col);
}

void draw_map_player(mlx_image_t *img, player_t *player,int ratio)
{
	int p_size = 6;
	point_t p_gridpos ={(player->pos.x/ratio),(player->pos.y/ratio)};
	for (int i = 0; i < p_size; i++) {
		for (int j = 0; j < p_size; j++)
			my_mlx_put_pixel(img,(point_t){p_gridpos.x+j-p_size/2,p_gridpos.y+i-p_size/2},0xFF0000FF);
	}
	float line_mult = 5;
	drawline(img, p_gridpos, (point_t){p_gridpos.x+player->dx*line_mult, p_gridpos.y+player->dy*line_mult},0xFF0000FF);
}

int find_next_pow(int val)
{
	int n = 1;
	while(n < val)
		n *= 2;
	return n;
}

void draw_map_squares(meta_t *meta, debug_t *debug)
{
	for (int i = 0; i < meta->debugmap.height; i++) {
		for (int j = 0; j < meta->debugmap.width; j++) {
			my_mlx_put_pixel(meta->debugmap.img, (point_t){i, j}, 0x00000000);
		}
	}
	for (int i = 0; i < meta->map->map_x; i++) {
		for (int j = 0; j < meta->map->map_y; j++) {
			if (meta->map->map[j * meta->map->map_x + i] == GD_WALL) {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_WALL, DBG_GRID_BORDER);
			}
			else if(meta->map->map[j * meta->map->map_x + i] == GD_DOOR_CLOSE)
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_DOOR, DBG_GRID_BORDER);
			else if(meta->map->map[j * meta->map->map_x + i] == GD_VOID)
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_VOID, DBG_GRID_BORDER);
			else {
				draw_square(debug->img, (point_t){i * debug->grid_size, j * debug->grid_size}, debug->grid_size, DBG_GRID_FREE, DBG_GRID_BORDER);
			}
		}
	}
}

void draw_map_rays(meta_t *meta, debug_t *debug, point_t p_gridpos)
{
	if(debug->db_rays) {
	    for (int i = 0; i < meta->raycaster.num_rays; i++)
		    drawline(meta->debugmap.img,p_gridpos,(point_t){meta->raycaster.rays[i].hit.x/debug->ratio,meta->raycaster.rays[i].hit.y/debug->ratio},0xFF00FFFF);
    }
    else {
        drawline(meta->debugmap.img,p_gridpos,(point_t){meta->raycaster.rays[0].hit.x/debug->ratio,meta->raycaster.rays[0].hit.y/debug->ratio},0xFF00FFFF);
        drawline(meta->debugmap.img,p_gridpos,(point_t){meta->raycaster.rays[RAYS-1].hit.x/debug->ratio,meta->raycaster.rays[RAYS-1].hit.y/debug->ratio},0xFF00FFFF);
    }
}

void draw_map_sprite(meta_t *meta, debug_t *debug, point_t p_gridpos)
{
    if(debug->db_show_sprites){
        for (int i = 0; i < meta->tot_sprites; i++) {
            draw_debugsprite(meta->debugmap.img,meta->sprite_data[i],debug->ratio,0x000000FF);
            if(debug->db_show_sprite_dist){
                drawline(meta->debugmap.img,p_gridpos,(point_t){meta->sprite_data[i]->pos.x/debug->ratio,meta->sprite_data[i]->pos.y/debug->ratio},0x0000FFFF);
            }
        }
    }
}

void draw_debugmap(void *param)
{
	meta_t *meta = param;
	debug_t* debug = &meta->debugmap;
	point_t p_gridpos ={(meta->player.pos.x/debug->ratio),(meta->player.pos.y/debug->ratio)};
	draw_map_squares(meta, debug);
	draw_map_rays(meta, debug, p_gridpos);
	draw_map_sprite(meta, debug, p_gridpos);
	draw_map_player(meta->debugmap.img, &meta->player,debug->ratio);
}

void draw_square(mlx_image_t *image, point_t start, int len, uint32_t fill, uint32_t bor)
{
	for (int y = 0; y < len; y++) {
		for (int x = 0; x < len; x++) {
			my_mlx_put_pixel(image, (point_t){start.x + x,start.y + y}, fill);
		}
	}
	for (int i = 0; i < len; i++) {
		my_mlx_put_pixel(image, (point_t){start.x + i, start.y + len - 1}, bor);
		my_mlx_put_pixel(image, (point_t){start.x + len - 1, start.y + i}, bor);
	}
}


void toggle_flag(int* flag)
{
	if(*flag == 1)
		*flag = 0;
	else
		*flag = 1;
}

void debug_toggles(meta_t* meta)
{
	if (mlx_is_key_down(meta->mlx, MLX_KEY_KP_0)) {
		if(meta->debugmap.img->instances[0].z == -1)
			mlx_set_instance_depth(&meta->debugmap.img->instances[0],1);
		else
			mlx_set_instance_depth(&meta->debugmap.img->instances[0],-1);
	}
	if(mlx_is_key_down(meta->mlx,MLX_KEY_BACKSPACE)) {
		meta->debugmap.db_rays = 0;
		meta->debugmap.db_show_sprites = 0;
		meta->debugmap.db_show_sprite_dist = 0;
	}
    if (mlx_is_key_down(meta->mlx,MLX_KEY_KP_1))
		toggle_flag(&meta->debugmap.db_rays);
    if (mlx_is_key_down(meta->mlx,MLX_KEY_KP_2))
		toggle_flag(&meta->debugmap.db_show_sprites);
	if (mlx_is_key_down(meta->mlx,MLX_KEY_KP_3))
		toggle_flag(&meta->debugmap.db_show_sprite_dist);
}
