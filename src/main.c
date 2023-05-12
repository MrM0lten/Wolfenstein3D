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

	int grid_id1;
	int grid_id2;

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

		grid_id1 = meta->map->map[p_grid_y * meta->map->map_x + p_posgrid_offset_x];
		grid_id2 = meta->map->map[p_posgrid_offset_y * meta->map->map_x + p_grid_x];
		if(grid_id1 == GD_FREE || grid_id1 == GD_DOOR_OPEN)
			player->pos.x += player->dx;
		if(grid_id2 == GD_FREE || grid_id2 == GD_DOOR_OPEN)
			player->pos.y += player->dy;
	}
	if (mlx_is_key_down(meta->mlx, MLX_KEY_DOWN) || mlx_is_key_down(meta->mlx, MLX_KEY_S)) {
		grid_id1 = meta->map->map[p_grid_y * meta->map->map_x + p_neggrid_offset_x];
		grid_id2 = meta->map->map[p_neggrid_offset_y * meta->map->map_x + p_grid_x];
		if(grid_id1 == GD_FREE || grid_id1 == GD_DOOR_OPEN)
			player->pos.x -= player->dx;
		if(grid_id2 == GD_FREE || grid_id2 == GD_DOOR_OPEN)
			player->pos.y -= player->dy;
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
	//forcing the mouse position to always be at the center of the screen
	meta->prev_mouse_pos.x = WIN_WIDTH/2;
	mlx_set_mouse_pos(meta->mlx, WIN_WIDTH/2, WIN_HEIGHT/2);
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
	if(!debugmap->img){
		log_string(2, 1, "Minimap setup failed");
		return 0;
	}

	debugmap->db_rays = 0; //setting to 0 by default because the rays are annoying
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

meta_t *setup()
{
	meta_t *meta = malloc(sizeof(meta_t));

	meta->win_height = WIN_HEIGHT;
	meta->win_width = WIN_WIDTH;
	meta->map = read_map("./resources/maps/broken.cub");
	if (meta->map == NULL) {
		free(meta);
		return NULL;
	}
	setup_player(&meta->player,(point_t){meta->map->p_pos_x,meta->map->p_pos_y},meta->map->p_orient);
	if (setup_mlx(meta) == 0) {
		free_meta(meta);
		return NULL;
	}
	if (setup_raycaster(&meta->raycaster, RAYS) == 0) {
		free_meta(meta);
		return NULL;
	}
	if (setup_debugmap(meta, &meta->debugmap, DEBUGMAP_WIDTH, DEBUGMAP_HEIGHT) == 0){
		free_meta(meta);
		return NULL;
	}

	meta->dist_to_proj = (meta->win_width/2)/tan(meta->player.fov/2);

	meta->fps_counter.img = NULL;
	meta->fps_counter.lastTime = get_time();
	meta->fps_counter.nbFrames = 0;
	meta->prev_mouse_pos = (point_t){512,512};
	meta->mouse_sensitivity = 0.15f;

	meta->tot_sprites = 3;
	meta->sprite_data = NULL;
	meta->sprite_data = malloc(sizeof(sprite_t*) * meta->tot_sprites);
	meta->sprite_data[0] =  malloc(sizeof(sprite_t));
	meta->sprite_data[0]->pos = (point_t){704.f,500.f};
	meta->sprite_data[0]->texture = mlx_load_png("./resources/textures/officer.png");
	meta->sprite_data[1] =  malloc(sizeof(sprite_t));
	meta->sprite_data[1]->pos = (point_t){704.f,400.f};
	meta->sprite_data[1]->texture = mlx_load_png("./resources/textures/officer.png");
	meta->sprite_data[2] =  malloc(sizeof(sprite_t));
	meta->sprite_data[2]->pos = (point_t){680.f,550.f};
	meta->sprite_data[2]->texture = mlx_load_png("./resources/textures/officer.png");

	//hiding the cursor, while retaining functionality
	mlx_set_cursor_mode(meta->mlx, MLX_MOUSE_HIDDEN);

	return meta;
}

void free_meta(meta_t* meta)
{
	free_map(meta->map);
	free(meta->raycaster.rays);
	if(meta->sprite_data != NULL) {
		for (int i = 0; i < meta->tot_sprites; i++)
		{
			if(meta->sprite_data[i] != NULL) {
				free(meta->sprite_data[i]->texture->pixels);
				free(meta->sprite_data[i]->texture);
				free(meta->sprite_data[i]);
			}
		}
	}
	free(meta->sprite_data);
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
