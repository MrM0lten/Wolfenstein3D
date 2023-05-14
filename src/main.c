#include "wolfenstein.h"

int setup_debugmap(meta_t *meta, debug_t* debugmap, int width, int height)
{
	int grid_size_x = width/meta->map->map_x;
	int grid_size_y = height/meta->map->map_y;

	if(meta->map->map_x > meta->map->map_y)
		debugmap->grid_size = meta->map->map_x;
	else
		debugmap->grid_size = meta->map->map_x;
	debugmap->grid_size = height/find_next_pow(debugmap->grid_size);
	debugmap->ratio = ((float)CUBE_DIM/debugmap->grid_size);
	debugmap->width = width;
	debugmap->height = height;
	debugmap->img = mlx_new_image(meta->mlx, width, height);
	if(!debugmap->img){
		log_string(2, 1, "Minimap setup failed");
		return 0;
	}
	debugmap->db_rays = 0;
	debugmap->db_show_sprites = 1;
	debugmap->db_show_sprite_dist = 1;
	return 1;
}

int setup_mlx(meta_t *meta)
{
	meta->mlx = mlx_init(meta->win_width, meta->win_height, "wolfenstein", false);
	if (meta->mlx == NULL) {
		log_string(2,1,"MLX init failed");
		return 0;
	}
	meta->main_scene = mlx_new_image(meta->mlx, meta->win_width, meta->win_height);
	if (meta->main_scene == NULL) {
		log_string(2,1,"MLX Main Scene setup failed");
		return 0;
	}
	mlx_loop_hook(meta->mlx, draw_scene, meta);
	mlx_loop_hook(meta->mlx, draw_debugmap, meta);
	mlx_loop_hook(meta->mlx, count_frames, meta);
	mlx_loop_hook(meta->mlx, ft_hook, meta);
	mlx_cursor_hook(meta->mlx,mouse_rot,meta);
	mlx_key_hook(meta->mlx,ft_key,meta);
	return 1;
}

int setup_player(player_t *player, point_t start_pos, double orientation)
{
	player->pos.x = start_pos.x * CUBE_DIM + CUBE_DIM/2;
	player->pos.y = start_pos.y * CUBE_DIM + CUBE_DIM/2;
	player->a = orientation;
	player->speed = 5;
	player->dx = cos(player->a) * player->speed;
	player->dy = sin(player->a) * player->speed;
	player->fov = PI/2;
	return 1;
}

int setup_raycaster(raycaster_t* raycaster,int num_rays)
{
	raycaster->num_rays = num_rays;
	raycaster->rays = malloc(sizeof(ray) * num_rays);
	if(!raycaster->rays) {
		log_string(2,1,"Malloc for Raycaster failed");
		return 0;
	}
	return 1;
}

int setup_fps_counter(fps_counter_t* fps_counter)
{
	fps_counter->img = NULL;
	fps_counter->lastTime = get_time();
	fps_counter->nbFrames = 0;
	return 1;
}

meta_t *setup(char *file)
{
	meta_t *meta = malloc(sizeof(meta_t));
	meta->win_height = WIN_HEIGHT;
	meta->win_width = WIN_WIDTH;
	meta->map = read_map(file);
	if (meta->map == NULL) {
		free(meta);
		return NULL;
	}
	setup_player(&meta->player,(point_t){meta->map->p_pos_x,meta->map->p_pos_y},meta->map->p_orient);
	if (setup_mlx(meta) == 0)
		return free_meta(meta);
	if (setup_raycaster(&meta->raycaster, RAYS) == 0)
		return free_meta(meta);
	if (setup_debugmap(meta, &meta->debugmap, DEBUGMAP_WIDTH, DEBUGMAP_HEIGHT) == 0)
		return free_meta(meta);
	setup_fps_counter(&meta->fps_counter);
	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);
	meta->prev_mouse_pos = (point_t){512,512};
	meta->mouse_sensitivity = 0.15f;
	meta->tot_sprites = 0;
	meta->sprite_data = NULL;
	mlx_set_cursor_mode(meta->mlx, MLX_MOUSE_HIDDEN);
	return meta;
}

void *free_meta(meta_t* meta)
{
	free_map(meta->map);
	free(meta->raycaster.rays);
	if(meta->sprite_data != NULL) {
		for (int i = 0; i < meta->tot_sprites; i++)
		{
			if(meta->sprite_data[i] != NULL) {
				free(meta->sprite_data[i]->texture_01->pixels);
				free(meta->sprite_data[i]->texture_01);
				free(meta->sprite_data[i]->texture_02->pixels);
				free(meta->sprite_data[i]->texture_02);
				free(meta->sprite_data[i]);
			}
		}
	}
	free(meta->sprite_data);
	free(meta);
	return NULL;
}

int cleanup(meta_t* meta)
{
	mlx_terminate(meta->mlx);
	free_meta(meta);
	return 1;
}

int main(int ac, char** av)
{
	if(ac != 2)
	{
		log_string(2,1,"Only 1 argument may be used.");
		return 1;
	}

	meta_t *meta = setup(av[1]);
	if (meta == NULL) {
		return 1;
	}
	mlx_image_to_window(meta->mlx, meta->main_scene, 0, 0);
	mlx_image_to_window(meta->mlx, meta->debugmap.img, 512, 0);
	mlx_set_instance_depth(&meta->debugmap.img->instances[0],-1);

	mlx_loop(meta->mlx);
	cleanup(meta);
	return (0);
}
