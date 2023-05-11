#include "wolfenstein.h"

//retrieves the direct value of a maps->map given the xy coordinates
int get_grid_val(int x,int y, map_t* map)
{
    return map->map[y * map->map_x + x];
}

//based on a grid checks if a given point(x,y) is adjacent to a wall in each cardial direction
int is_walled(int x, int y, map_t* map)
{
    int val = map->map[y * map->map_x + x];
    if(val == GD_WALL)
        return GD_WALL;
    //initially checking for borders
    if(x == 0 || x + 1 == map->map_x || y == 0 || y + 1 == map->map_y)
        return 0;

    //need to check every element as map could have 'holes'
    if(get_grid_val(x+1,y,map) == GD_VOID || get_grid_val(x-1,y,map) == GD_VOID
    || get_grid_val(x,y+1,map) == GD_VOID || get_grid_val(x,y-1,map) == GD_VOID)
        return 0;
    return 1;
}

//tries finding one of several valid texture ids, 1 = found,0 = not found
//return value corresponds to the texture in enum type "texture_type"
//order of these strings is important!
int find_texture_id(char *line)
{
    char * ids[] = {"NO","EA","SO","WE","DO",NULL};

    int i = 0;
    while(ids[i]) {
        if(!ft_strncmp(line,ids[i],2))
            return i;
        i++;
    }
    return -1;
}

//returns the player rotation in gradiens based on an input of N,E,S,W
//east = 0;
//south = PI/2
//west = PI
//north = 1.5 * PI
double player_rot_from_char(char c)
{
    char * ids = {"ESWN"};

    for (int i = 0; i < 4; i++) {
        if(ids[i] == c)
            return (i * PI/2);
    }
    return -1;
}

//converts a string of strings containing R G B values to a uint32 color format
uint32_t str_to_col(char **col_data)
{
    //extract Color information
    uint32_t col = 0;
    int lshift = 24;
    int curr = 0;
    int col_val;
    while(col_data[curr]) {
        col_val = ft_atoi(col_data[curr]);
        if(col_val > 255)
            log_string(1,1,"Color value was too high, overflow detected");
        if(col_val < 0)
            log_string(1,1,"Color value was too low, underflow detected");
        col |= (uint32_t)((unsigned char)col_val) << lshift;
        lshift -= 8;
        curr++;
    }
    col |= 255; //adding alpha at the end

    return col;
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

//will scale an image according to a new width and height input
bool resize_texture(mlx_texture_t* txt, uint32_t nwidth, uint32_t nheight)
{
	if (!nwidth || !nheight || nwidth > INT16_MAX || nheight > INT16_MAX)
    {
        //give error
		return false;
    }
    if (nwidth != txt->width || nheight != txt->height)
	{
	    uint32_t* origin = (uint32_t*)txt->pixels;
	    float wstep = (float)txt->width / nwidth;
	    float hstep = (float)txt->height / nheight;

	    uint8_t* tempbuff = calloc(nwidth * nheight, 4);
	    if (!tempbuff)
	    	return (false);
        txt->pixels = tempbuff;

	    uint32_t* destin = (uint32_t*)txt->pixels;
	    for (uint32_t j = 0; j < nheight; j++)
	    	for (uint32_t i = 0; i < nwidth; i++)
	    		destin[j * nwidth + i] = origin[(uint32_t)(j * hstep) * txt->width + (uint32_t)(i * wstep)];
	    (*(uint32_t*)&txt->width) = nwidth;
	    (*(uint32_t*)&txt->height) = nheight;
	    free(origin);
    }
    return true;
}
