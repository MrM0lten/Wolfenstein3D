#ifndef WOLFENSTEIN_H
#define WOLFENSTEIN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>

#include "MLX42.h"
#include "libft.h"

#define IMG_WIDTH 1024
#define IMG_HEIGHT 512
#define WIN_WIDTH 1024
#define WIN_HEIGHT 512
#define DEBUGMAP_HEIGHT 512
#define DEBUGMAP_WIDTH 512

#define CUBE_DIM 64
#define PLAYER_HEIGHT 0
#define PLAYER_COL_DIST 10
#define MAX_RAY_ITER 32

#define RAYS 1024

#define PI 3.1415926

#define DELAYPROCESS 100000000
#define MAX_DRAW_DIST 1000

#define DBG_GRID_WALL 0xA9A9A9FF
#define DBG_GRID_FREE 0xFFFFFFFF
#define DBG_GRID_VOID 0x000000FF
#define DBG_GRID_DOOR 0x393939FF
#define DBG_GRID_BORDER 0x303030FF

enum texture_type{
	TXT_NORTH = 0,
	TXT_EAST = 1,
	TXT_SOUTH = 2,
	TXT_WEST = 3,
	TXT_DOOR = 4,
};

enum map_type_id{
    MP_ERR = 0,
    MP_TEXT,
    MP_COL,
    MP_MAP,
};

enum grid_type
{
	GD_FREE = 0 << 0,
	GD_WALL = 1 << 0,
	GD_DOOR_OPEN = 1 << 1,
	GD_DOOR_CLOSE = 1 << 2,
	GD_VOID = 1 << 8
};

typedef struct debug_s
{
	mlx_image_t *img;
	int width;
	int height;
	int grid_size;
	int ratio;

	int db_rays;
	int db_show_sprites;
	int db_show_sprite_dist;
} debug_t;

typedef struct point_s
{
	float x;
	float y;
}point_t;

typedef struct sprite_s
{
	point_t pos;
	mlx_texture_t *texture;
	double len;
} sprite_t;

typedef struct ray
{
	point_t hit;
	double len;
	double dir;
	enum directions{
		DIR_NORTH = 0,
		DIR_EAST,
		DIR_SOUTH,
		DIR_WEST
	} hit_dir;
	enum map_type_id hit_id;
}ray;

typedef struct raycaster
{
	ray *rays;
	int num_rays;
}raycaster_t;

typedef struct raycaster2
{
	ray *rays;
	point_t origin;
	double angle;
	double plane_dist;
	int nb_rays;
	int channel;
}raycaster2_t;


typedef struct player
{
	point_t pos;
	float dx;
	float dy;
	float a;
	float fov;
	float speed;
}player_t;

/// @brief a structure that holds all information about a Cub3D map
/// @param file_data string array containing all the paths to files
/// @param texture_data Array of textures loaded into the game from file data
/// @param col_floor Color to be used to texture the floor
/// @param col_ceil Color to be used to texture the ceiling
/// @param map The 1D array containing the actual map data
/// @param map_x Map X dimensions
/// @param map_y Map Y dimensions
/// @param map_dim Calculated Map dimensions (map_x * map_y)
/// @param p_pos_x Players start X position within the map array
/// @param p_pos_y Players start Y position within the map array
/// @param p_orient Player orientation on game start in Radians
typedef struct map_s
{
	int total_textures;
	char ** file_data;
	mlx_texture_t** texture_data;

    uint32_t col_floor;
    uint32_t col_ceil;

    int *map;
    int map_x;
    int map_y;
    int map_dim;

    int p_pos_x;
    int p_pos_y;
    double p_orient;

}map_t;

typedef struct fps_counter
{
	mlx_image_t *img;
	int nbFrames;
	size_t lastTime;
}fps_counter_t;

typedef struct meta
{
	player_t player;

	int win_height;
	int win_width;

	mlx_image_t *main_scene;
	mlx_t* mlx;
	map_t *map;

	sprite_t** sprite_data;
	int tot_sprites;

	debug_t debugmap;
	raycaster_t raycaster;

	fps_counter_t fps_counter;
	point_t prev_mouse_pos;
	float mouse_sensitivity;
	float dist_to_proj;

}meta_t;


//len needs to be renamed or additional param used for actual wall pixel height
// Raycaster
void raycaster(int nb_rays, double fov,ray *arr,meta_t *meta, int channel);
void print_raydata();
ray raycast(double radian, meta_t* meta, int channel);
double	vector2d_len(double x, double y);

// Drawing
void drawline(mlx_image_t* image, point_t start, point_t end, int color);
void draw_debugmap(void* param);
void draw_scene(void *param);
void draw_square(mlx_image_t* image, point_t start, int len, uint32_t fill, uint32_t bor);

// FPS counter
void update_fps_counter(meta_t *meta);
void count_frames(void* param);
size_t	get_time(void);

map_t* init_map(void);
map_t* read_map(char *path);
int validate_map(map_t* map);
void free_map(map_t *map);

void my_mlx_put_pixel(mlx_image_t *img, point_t pos, int color);

void store_map_array(map_t* map,char *line,int fd);
void store_map_color(map_t* map,char *line);
void store_map_texture(map_t* map,char *line);
enum map_type_id identify_line(char *line);
int find_texture_id(char *line);

//map debugging
void debug_map(map_t *map);
void print_map(map_t *map);

void debug_meta(meta_t *meta);
void debug_raycaster(raycaster_t* raycaster);
void debug_player(player_t* player);
void debug_ray(ray *ray);
void debug_point(point_t* point);
void debug_toggles(meta_t* meta);

//utilities
int find_next_pow(int val);
int get_grid_val(int x,int y, map_t* map);
uint32_t str_to_col(char **col_data);
double player_rot_from_char(char c);
int find_texture_id(char *line);
int is_walled(int x, int y, map_t* map);
int ft_strarr_len(char **arr);
bool resize_texture(mlx_texture_t* txt, uint32_t nwidth, uint32_t nheight);
void add_sprite(meta_t* meta,float pos_x,float pos_y, char *filepath);
double angle_fix(double angle);

//cleanup
void free_meta(meta_t* meta);
int cleanup(meta_t* meta);

//error
void free_and_warn(char ** str);

/**
 * A function to display messages of varying degrees
 * 0 = Log
 * 1 = Warning
 * 2 = Error
 * @param level The log level
 * @param amount The amount of strings passed.
*/
void log_string(int level, int amount, ...);
#endif
