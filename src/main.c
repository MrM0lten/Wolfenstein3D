#include "wolfenstein.h"

void ft_hook(void* param)
{
	meta_t* meta = param;
	player_t *player = &meta->player;

	if (mlx_is_key_down(meta->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(meta->mlx);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_LEFT)) {
		player->a -=0.1;
		if(player->a < 0)
			player->a += 2 * PI;
		player->dx = cos(player->a) * 5;
		player->dy = sin(player->a) * 5;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_RIGHT)) {
		player->a +=0.1;
		if(player->a > 2 * PI)
			player->a -= 2 * PI;
		player->dx = cos(2*PI - player->a) * 5;
		player->dy = sin(player->a) * 5;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_UP)) {
		player->pos.x += player->dx;
		player->pos.y += player->dy;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_DOWN)) {
		player->pos.x -= player->dx;
		player->pos.y -= player->dy;
	}
}

int setup_debugmap(meta_t *meta, debug_t* debugmap, int width, int height)
{
	int grid_size_x = meta->debugmap.width/meta->map->map_x;
	int grid_size_y = meta->debugmap.height/meta->map->map_y;

	if(meta->map->map_x > meta->map->map_y)
		debugmap->grid_size = meta->map->map_x;
	else
		debugmap->grid_size = meta->map->map_x;

	debugmap->grid_size = height/find_next_pow(debugmap->grid_size);
	debugmap->ratio = (CUBE_DIM/debugmap->grid_size);

	debugmap->width = width;
	debugmap->height = height;
	debugmap->img = mlx_new_image(meta->mlx, width, height);
	return 1;
}

int setup_mlx(meta_t *meta)
{
	meta->mlx = mlx_init(meta->win_width, meta->win_height, "wolfenstein", true);
	if (meta->mlx == NULL) {}
	meta->main_scene = mlx_new_image(meta->mlx, meta->win_width, meta->win_height);
	if (meta->main_scene == NULL) {}
	mlx_loop_hook(meta->mlx, draw_scene, meta);
	mlx_loop_hook(meta->mlx, draw_debugmap, meta);
	mlx_loop_hook(meta->mlx, ft_hook, meta);
	//mlx_loop_hook(meta->mlx, count_frames, meta);
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
	return 1;
}

meta_t *setup()
{
	meta_t *meta = malloc(sizeof(meta_t));

	meta->win_height = WIN_HEIGHT;
	meta->win_width = WIN_WIDTH;

	meta->map = read_map("./resources/maps/show_off.cub");
	if (meta->map == NULL) {}
	if (setup_player(&meta->player,(point_t){meta->map->p_pos_x,meta->map->p_pos_y},meta->map->p_orient) == 0) {}
	if (setup_mlx(meta) == 0) {}
	if(setup_raycaster(&meta->raycaster, RAYS)) {}
	if(setup_debugmap(meta,&meta->debugmap,DEBUGMAP_WIDTH,DEBUGMAP_HEIGHT)){}

	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);

	meta->fps_counter.img = NULL;
	meta->fps_counter.lastTime = get_time();
	meta->fps_counter.nbFrames = 0;
	return meta;
}

void free_meta(meta_t* meta)
{
	free_map(meta->map);
	free(meta->raycaster.rays);
}

int cleanup(meta_t* meta)
{
	free_meta(meta);
	mlx_terminate(meta->mlx);
	return 1;
}

int main()
{
	meta_t *meta = setup();
	if (meta == NULL) {}
	mlx_image_to_window(meta->mlx, meta->main_scene, 0, 0);
	mlx_image_to_window(meta->mlx, meta->debugmap.img, 512, 0);
	mlx_loop(meta->mlx);
	cleanup(meta);
	return (0);
}
