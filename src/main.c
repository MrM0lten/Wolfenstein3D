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

int setup_mlx(meta_t *meta)
{
	meta->mlx = mlx_init(meta->win_width, meta->win_height, "wolfenstein", true);
	if (meta->mlx == NULL) {}
	meta->image_window = mlx_new_image(meta->mlx, meta->win_width, meta->win_height);
	if (meta->image_window == NULL) {}
	mlx_loop_hook(meta->mlx, draw_minimap, meta);
	mlx_loop_hook(meta->mlx, ft_hook, meta);
	mlx_loop_hook(meta->mlx, count_frames, meta);
	mlx_image_to_window(meta->mlx, meta->image_window, 0, 0);
	return 1;
}

int setup_player(player_t *player, point_t start_pos, double orientation)
{
	player->pos.x = start_pos.x * 64;
	player->pos.y = start_pos.y * 64;
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
	mlx_loop(meta->mlx);
	cleanup(meta);
	return (0);
}
