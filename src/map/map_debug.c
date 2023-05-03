#include "wolfenstein.h"

void display_map_data(map_t *map)
{
    printf("--------------MAP DATA-----------------\n");
    printf("NORTH Texture path: [%s]\n",map->text_north);
    printf("SOUTH Texture path: [%s]\n",map->text_south);
    printf("EAST Texture path: [%s]\n",map->text_east);
    printf("WEST Texture path: [%s]\n",map->text_west);
    printf("Ceiling Color: %u (RGBA = %i,%i,%i,%i)\n",map->col_ceil,
    (map->col_ceil>>24),
    ((map->col_ceil<<8)>>24),
    ((map->col_ceil<<16)>>24),
    ((map->col_ceil<<24)>>24));
    printf("Floor Color: %u (RGBA = %i,%i,%i,%i)\n",map->col_floor,
    (map->col_floor>>24),
    ((map->col_floor<<8)>>24),
    ((map->col_floor<<16)>>24),
    ((map->col_floor<<24)>>24));
    printf("Player Position in Grid: [%d][%d]\n",map->p_pos_x,map->p_pos_y);
    printf("Player Start orientation %f\n", map->p_orient);

    printf("Map x,y: [%d][%d]\n", map->map_x, map->map_y);
    printf("Map dimensions = %d\n",map->map_dim);
    print_map(map);
}

void print_map(map_t *map)
{
    for (int i = 0; i < map->map_x + 1; i++)
        printf("_");
    printf("\n");
    for (int i = 0; i < map->map_dim; i++)
    {
        if(i % map->map_x == 0)
            printf("|");
        if(map->map[i] == GD_VOID)
            printf(" ");
        else if(map->map[i] == -1)
            printf("?");
        else
            printf("%d",map->map[i]);
        if((i + 1) % map->map_x  == 0 && i != 0)
            printf("|\n");
    }
    for (int i = 0; i < map->map_x + 1; i++)
        printf("-");
    printf("\n");
}

//generic function to give info to user
//0 = log
//1 = warning
//2 = error
void log_string(char *message, int level)
{
    if(level == 0)
        printf("[LOG]: ");
    else if(level == 1)
        printf("[WARNING]: ");
    else if (level == 2)
        printf("[ERROR]: ");
    printf("%s\n",message);
}

//takes a char** and checks how many strings are part of the array.
//arr needs to have nullptr as last elem
int ft_strarr_len(char **arr)
{
    int i = 0;
    while(arr[i])
        i++;
    return i;
}
