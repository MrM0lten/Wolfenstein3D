#include "wolfenstein.h"

void ft_hook(void* param)
{
	meta_t* meta = param;
	player_t *player = &meta->player;


	//handle player collision, by creation a vector in front and behind the player
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
	int p_neggrid_offset_x = (player->pos.x-offset_x)/CUBE_DIM;
	int p_neggrid_offset_y = (player->pos.y-offset_y)/CUBE_DIM;

	//printf("player grid = [%d][%d]\n",p_grid_x,p_grid_y);
	//printf("player offset grid = [%d][%d]\n",p_posgrid_offset_x,p_posgrid_offset_y);
	//printf("player delta = [%f][%f]\n",player->dx,player->dy);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_ESCAPE))
		mlx_close_window(meta->mlx);
	if (mlx_is_key_down(meta->mlx, MLX_KEY_LEFT)|| mlx_is_key_down(meta->mlx, MLX_KEY_A)) {
		player->a -=0.1;
		if(player->a < 0)
			player->a += 2 * PI;
		player->dx = cos(player->a) * 5;
		player->dy = sin(player->a) * 5;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_RIGHT) || mlx_is_key_down(meta->mlx, MLX_KEY_D)) {
		player->a +=0.1;
		if(player->a > 2 * PI)
			player->a -= 2 * PI;
		player->dx = cos(2*PI - player->a) * 5;
		player->dy = sin(player->a) * 5;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_UP) || mlx_is_key_down(meta->mlx, MLX_KEY_W)) {

		if(meta->map->map[p_grid_y * meta->map->map_x + p_posgrid_offset_x] == GD_FREE)
			player->pos.x += player->dx;
		if(meta->map->map[p_posgrid_offset_y * meta->map->map_x + p_grid_x] == GD_FREE)
			player->pos.y += player->dy;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_DOWN) || mlx_is_key_down(meta->mlx, MLX_KEY_S)) {
		if(meta->map->map[p_grid_y * meta->map->map_x + p_neggrid_offset_x] == GD_FREE)
			player->pos.x -= player->dx;
		if(meta->map->map[p_neggrid_offset_y * meta->map->map_x + p_grid_x] == GD_FREE)
			player->pos.y -= player->dy;
	}
	if(mlx_is_key_down(meta->mlx, MLX_KEY_E))
	{
		ray ray = raycast(meta->player.a, meta);
		if(ray.hit_id == GD_DOOR)
		{
			int mx = (int)ray.hit.x>>6;
			int my = (int)ray.hit.y>>6;
			int mp = my * meta->map->map_x + mx;
			meta->map->map[mp] = GD_FREE;
		}
	}
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
	//printf("[%f][%f]\n",xpos,ypos);
	meta->prev_mouse_pos.x = (float)xpos;
}

int setup_debugmap(meta_t *meta, debug_t* debugmap, int width, int height)
{
	int grid_size_x = width/meta->map->map_x;
	int grid_size_y = height/meta->map->map_y;

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
	mlx_loop_hook(meta->mlx, count_frames, meta);
	mlx_loop_hook(meta->mlx, ft_hook, meta);
	mlx_cursor_hook(meta->mlx,mouse_rot,meta);

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

	meta->map = read_map("./resources/maps/test_dir.cub");
	if (meta->map == NULL) {}
	if (setup_player(&meta->player,(point_t){meta->map->p_pos_x,meta->map->p_pos_y},meta->map->p_orient) == 0) {}
	if (setup_mlx(meta) == 0) {}
	if(setup_raycaster(&meta->raycaster, RAYS)) {}
	if(setup_debugmap(meta,&meta->debugmap,DEBUGMAP_WIDTH,DEBUGMAP_HEIGHT)){}

	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);

	meta->fps_counter.img = NULL;
	meta->fps_counter.lastTime = get_time();
	meta->fps_counter.nbFrames = 0;
	meta->prev_mouse_pos = (point_t){512,512};
	meta->mouse_sensitivity = 1.f;


	//currently linear decrease
	meta->shading_lut = malloc(sizeof(float) * MAX_DRAW_DIST);
	float falloff_mult = 1;
	float step_val = (float)1/MAX_DRAW_DIST * falloff_mult;
	float val = 1;
	for (int i = 0; i < MAX_DRAW_DIST; i++)
	{
		meta->shading_lut[i] = val;
		//val = (float)(pow((double)i,2)/100000);
		if(val > 0)
			val -= step_val;
		else
			val = 0;
		printf("val at [%d] = %f\n",i,meta->shading_lut[i]);
	}

	return meta;
}

void free_meta(meta_t* meta)
{
	free_map(meta->map);
	free(meta->raycaster.rays);
	free(meta->shading_lut);
	free(meta);
}

int cleanup(meta_t* meta)
{
	mlx_terminate(meta->mlx);
	free_meta(meta);
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
