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

#define PI 3.1415926

enum map_type_id{
    MP_ERR = 0,
    MP_TEXT,
    MP_COL,
    MP_MAP,

};

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
typedef struct map_s
{
    char *text_north;
    char *text_south;
    char *text_west;
    char *text_east;

    uint32_t col_floor;
    uint32_t col_ceil;

    int *map;
    int map_x;
    int map_y;
    int map_dim;


    int p_pos_x;
    int p_pos_y;
    double p_orient;

} map_t;

map_t* init_map(void);
map_t* read_map(char *path);
int validate_map(map_t* map);
void delete_map(map_t *map);

void store_map_array(map_t* map,char *line,int fd);
void store_map_color(map_t* map,char *line);
void store_map_texture(map_t* map,char *line);
enum map_type_id identify_line(char *line);

//map debugging
void display_map_data(map_t *map);
void print_map(map_t *map);
void log_string(char *message, int level);
int ft_strarr_len(char **arr);

#endif
