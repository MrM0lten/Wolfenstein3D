#include "wolfenstein.h"

void ft_key(mlx_key_data_t keydata, void* param)
{
	meta_t* meta = param;

	if(mlx_is_key_down(meta->mlx, MLX_KEY_E))
	{
		ray ray = raycast(meta->player.a, meta,GD_DOOR_CLOSE | GD_DOOR_OPEN);
		if(ray.len <= 100.f)
		{
			int mx = (int)ray.hit.x>>6;
			int my = (int)ray.hit.y>>6;
			int mp = my * meta->map->map_x + mx;
			if(meta->map->map[mp] == GD_DOOR_CLOSE)
				meta->map->map[mp] = GD_DOOR_OPEN;
			else
				meta->map->map[mp] = GD_DOOR_CLOSE;
		}
	}
	debug_toggles(meta);
}

void rotate_left(player_t *player)
{
	player->a -=0.1;
	if(player->a < 0)
		player->a += 2 * PI;
	player->dx = cos(player->a) * 5;
	player->dy = sin(player->a) * 5;
}

void rotate_right(player_t *player)
{
	player->a +=0.1;
	if(player->a > 2 * PI)
		player->a -= 2 * PI;
	player->dx = cos(2*PI - player->a) * 5;
	player->dy = sin(player->a) * 5;
}

void walk_forward(meta_t *meta, player_t *player)
{
	int p_grid_x = player->pos.x/CUBE_DIM;
	int p_grid_y = player->pos.y/CUBE_DIM;
	int offset_x = 0;
	int offset_y = 0;
	if(player->dx > 0)
		offset_x = PLAYER_COL_DIST;
	else
		offset_x = -PLAYER_COL_DIST;
	if(player->dy > 0)
		offset_y = PLAYER_COL_DIST;
	else
		offset_y = -PLAYER_COL_DIST;
	int p_posgrid_offset_x = (player->pos.x+offset_x)/CUBE_DIM;
	int p_posgrid_offset_y = (player->pos.y+offset_y)/CUBE_DIM;
	int grid_id1 = meta->map->map[p_grid_y * meta->map->map_x + p_posgrid_offset_x];
	int grid_id2 = meta->map->map[p_posgrid_offset_y * meta->map->map_x + p_grid_x];
	if(grid_id1 == GD_FREE || grid_id1 == GD_DOOR_OPEN)
		player->pos.x += player->dx;
	if(grid_id2 == GD_FREE || grid_id2 == GD_DOOR_OPEN)
		player->pos.y += player->dy;
}

void walk_backward(meta_t *meta, player_t *player)
{
	int p_grid_x = player->pos.x/CUBE_DIM;
	int p_grid_y = player->pos.y/CUBE_DIM;
	int offset_x = 0;
	int offset_y = 0;
	if(player->dx > 0)
		offset_x = PLAYER_COL_DIST;
	else
		offset_x = -PLAYER_COL_DIST;
	if(player->dy > 0)
		offset_y = PLAYER_COL_DIST;
	else
		offset_y = -PLAYER_COL_DIST;
	int p_neggrid_offset_x = (player->pos.x-offset_x)/CUBE_DIM;
	int p_neggrid_offset_y = (player->pos.y-offset_y)/CUBE_DIM;
	int grid_id1 = meta->map->map[p_grid_y * meta->map->map_x + p_neggrid_offset_x];
	int grid_id2 = meta->map->map[p_neggrid_offset_y * meta->map->map_x + p_grid_x];
	if(grid_id1 == GD_FREE || grid_id1 == GD_DOOR_OPEN)
		player->pos.x -= player->dx;
	if(grid_id2 == GD_FREE || grid_id2 == GD_DOOR_OPEN)
		player->pos.y -= player->dy;
}

void ft_hook(void* param)
{
	meta_t* meta = param;
	player_t *player = &meta->player;
	if (mlx_is_key_down(meta->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(meta->mlx);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_LEFT)|| mlx_is_key_down(meta->mlx, MLX_KEY_A))
		rotate_left(player);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_RIGHT) || mlx_is_key_down(meta->mlx, MLX_KEY_D))
		rotate_right(player);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_UP) || mlx_is_key_down(meta->mlx, MLX_KEY_W))
		walk_forward(meta, player);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_DOWN) || mlx_is_key_down(meta->mlx, MLX_KEY_S))
		walk_backward(meta, player);
}

//calculates the player rotation based on the current and previous mouse positon
void mouse_rot(double xpos, double ypos, void* param)
{
	meta_t* meta = param;
	player_t *player = &meta->player;
	float mouse_dx;
	if((float)xpos < meta->prev_mouse_pos.x)
	{
		mouse_dx = meta->prev_mouse_pos.x - xpos;
		player->a -= (mouse_dx*2*PI)/WIN_WIDTH * meta->mouse_sensitivity;
		if(player->a < 0)
			player->a += 2 * PI;
		player->dx = cos(player->a) * player->speed;
		player->dy = sin(player->a) * player->speed;
	}
	else
	{
		mouse_dx = xpos - meta->prev_mouse_pos.x;
		player->a += (mouse_dx*2*PI)/WIN_WIDTH * meta->mouse_sensitivity;
		if(player->a > 2 * PI)
			player->a -= 2 * PI;
		player->dx = cos(2*PI - player->a) * player->speed;
		player->dy = sin(player->a) * player->speed;
	}
	//forcing the mouse position to always be at the center of the screen
	meta->prev_mouse_pos.x = WIN_WIDTH/2;
	mlx_set_mouse_pos(meta->mlx, WIN_WIDTH/2, WIN_HEIGHT/2);
}
