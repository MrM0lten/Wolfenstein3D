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

#define IMG_WIDTH 512
#define IMG_HEIGHT 512
#define WIN_WIDTH 1024
#define WIN_HEIGHT 512
#define DEBUGMAP_HEIGHT 512
#define DEBUGMAP_WIDTH 512
#define MAP_HEIGHT 8
#define MAP_WIDTH 8
#define MAP_DIM MAP_HEIGHT * MAP_WIDTH
#define SQ_DIM MAP_DIM

#define CUBE_DIM 64
#define MAX_RAY_ITER 8

#define RAYS 512

#define PI 3.1415926

#define DELAYPROCESS 100000000

typedef struct debug_s
{
	mlx_image_t *img;
	int width;
	int height;
	int grid_size;
	int ratio;
} debug_t;

typedef struct point_s
{
	float x;
	float y;
}point_t;

typedef struct ray
{
	point_t hit;
	double len;
	double dir;
	enum directions{
		DIR_NORTH,
		DIR_SOUTH,
		DIR_EAST,
		DIR_WEST
	} hit_dir;
}ray;

extern ray ray_data[RAYS];

typedef struct raycaster
{
	ray *rays;
	int num_rays;

}raycaster_t;


typedef struct player
{
	point_t pos;
	float dx;
	float dy;
	float a;
	float fov;
	float speed;
}player_t;

enum map_type_id{
    MP_ERR = 0,
    MP_TEXT,
    MP_COL,
    MP_MAP,
};

typedef struct map_s
{
    char *text_north;
    char *text_south;
    char *text_west;
    char *text_east;

	mlx_texture_t* texture_north;
	mlx_texture_t* texture_south;
	mlx_texture_t* texture_east;
	mlx_texture_t* texture_west;

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
	debug_t debugmap;
	raycaster_t raycaster;

	fps_counter_t fps_counter;
	float dist_to_proj;
}meta_t;

/// @brief a structure that holds all information about a Cub3D map
/// @param text_north Texture path to be used for all walls facing north
/// @param text_south Texture path to be used for all walls facing south
/// @param text_west Texture path to be used for all walls facing west
/// @param text_east Texture path to be used for all walls facing east
/// @param col_floor Color to be used to texture the floor
/// @param col_ceil Color to be used to texture the ceiling
/// @param map The 1D array containing the actual map data
/// @param map_x Map X dimensions
/// @param map_y Map Y dimensions
/// @param map_dim Calculated Map dimensions (map_x * map_y)
/// @param p_pos_x Players start X position within the map array
/// @param p_pos_y Players start Y position within the map array
/// @param p_orient Player orientation on game start in Radians
extern map_t* map;

//len needs to be renamed or additional param used for actual wall pixel height
// Raycaster
void raycaster(int nb_rays, double fov,ray *arr,meta_t *meta);
void print_raydata();

// Drawing
void drawline(mlx_image_t* image, point_t start, point_t end, int color);
void draw_debugmap(void* param);
void draw_scene(void *param);
void draw_square(mlx_image_t* image, point_t start, int len, uint32_t fill, uint32_t bor);

// FPS counter
void update_fps_counter(meta_t *meta);
void count_frames(void* param);
size_t	get_time(void);


int is_up(double radian);

map_t* init_map(void);
map_t* read_map(char *path);
int validate_map(map_t* map);
void free_map(map_t *map);

void my_mlx_put_pixel(mlx_image_t *img, point_t pos, int color);

void store_map_array(map_t* map,char *line,int fd);
void store_map_color(map_t* map,char *line);
void store_map_texture(map_t* map,char *line);
enum map_type_id identify_line(char *line);

//map debugging
void display_map_data(map_t *map);
void print_map(map_t *map);
void log_string(char *message, int level);
int ft_strarr_len(char **arr);

void debug_meta(meta_t *meta);
void debug_raycaster(raycaster_t* raycaster);
void debug_player(player_t* player);
void debug_ray(ray *ray);
void debug_point(point_t* point);


//utilities
//returns the next bigger power of 2 value given a value e.g 5 -> 8
int find_next_pow(int val);

#endif
