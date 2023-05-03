#include "wolfenstein.h"

//a function specifically made to free a char * and log an error
static void free_and_warn(char ** texture)
{
    log_string("Texture with same Identifier detected",1);
    free(*texture);
    *texture = NULL;
}

void store_map_texture(map_t* map,char *line)
{
    unsigned int i = 2;
    while(line[i] == ' ' || line[i] == '\t' ) //detecting any spaces after the id
        i++;
    unsigned int len = i;
    while(line[len] &&  line[len] != ' '
        && line[len] != '\t' && line[len] != '\n' ) //making sure to only take the first word
        len++;
    if(line[i] == '\n' ) //checking if there is no path after an identifier
        return log_string("Found Texture identifier, but line is empty",1);

    if(!ft_strncmp(line,"NO",2))
    {
        if(map->text_north != NULL)
            free_and_warn(&map->text_north);
        map->text_north = ft_substr(line,i,len-i);
    }
    else if(!ft_strncmp(line,"SO",2))
    {
        if(map->text_south != NULL)
            free_and_warn(&map->text_south);
        map->text_south = ft_substr(line,i,len-i);
    }
    else if (!ft_strncmp(line,"WE",2))
    {
        if(map->text_west != NULL)
            free_and_warn(&map->text_west);
        map->text_west = ft_substr(line,i,len-i);
    }
    else if( !ft_strncmp(line,"EA",2))
    {
        if(map->text_east != NULL)
            free_and_warn(&map->text_east);
        map->text_east = ft_substr(line,i,len-i);
    }
}

void store_map_color(map_t* map,char *line)
{
    //printf("found color\n");
    unsigned int i = 1;
    while(line[i] == ' ' || line[i] == '\t' ) //detecting any spaces after the id
        i++;
    unsigned int len = i;
    while(line[len] &&  line[len] != ' '
        && line[len] != '\t' && line[len] != '\n' ) //making sure to only take the first word
        len++;
    if(line[i] == '\n' ) //checking if there is no path after an identifier
        return log_string("Found Color identifier, but line is empty",1);
    char *col_str = ft_substr(line,i,len-i);
    if(!col_str)
        return log_string("Substring error",2);
    char **col_data = ft_split(col_str,',');
    if(!col_data || ft_strarr_len(col_data) != 3)
        return log_string("Didnt find appropriate R,G,B value format ",1);


    //extract Color information
    uint32_t col = 0;
    int lshift = 24;
    int curr = 0;
    while(col_data[curr])
    {
        col |= (uint32_t)ft_atoi(col_data[curr]) << lshift;
        lshift -= 8;
        curr++;
    }
    col |= 255; //adding alpha at the end

    if(line[0] == 'F')
    {
        if(map->col_floor != 0)
            log_string("Addional Ceiling Color found!",1);
        map->col_floor = col;
    }
    else if(line[0] == 'C')
    {
        if(map->col_ceil != 0)
            log_string("Addional Floor Color found!",1);
        map->col_ceil = col;
    }


    //free utilities
    free(col_str);
    int pos = 0;
    while(col_data[pos])
        free(col_data[pos++]);
    free(col_data);
}

//fills the maps int array with values, it will detect errors, but will parse anyways
//actual error checking will be done later
//fill actual map array by looping through the list
//'0' = 0
//'1' = 1
//' ' = 2
//'N/E/S/W' = 0 but save info player info
//err = -1
static void fill_map(map_t* map,t_list* lst_line)
{
    map->map = malloc(sizeof(int) * (map->map_dim));
    char* content;
    int pos = 0;
    int i;
    while(lst_line)
    {
        content = (char *)lst_line->content;
        //printf("{%s}\n",content);
        i = 0;
        while(content[i] && content[i] != '\n')
        {
            if(content[i] == '0')
                map->map[pos] = 0;
            else if(content[i] == '1')
                map->map[pos] = 1;
            else if(content[i] == ' ')
                map->map[pos] = 2;
            else if(content[i] == 'N')
            {
                if(map->p_pos_x != -1)
                    log_string("Addional Player location found, prev Value overriden!",1);
                map->map[pos] = 0;
                map->p_orient = 1.5* PI;
                map->p_pos_x = i;
                map->p_pos_y = (pos+1)/map->map_x;
            }
            else if(content[i] == 'S')
            {
                if(map->p_pos_x != -1)
                    log_string("Addional Player location found, prev Value overriden!",1);
                map->map[pos] = 0;
                map->p_orient = PI/2;
                map->p_pos_x = i;
                map->p_pos_y = (pos+1)/map->map_x;
            }
            else if(content[i] == 'E')
            {
                if(map->p_pos_x != -1)
                    log_string("Addional Player location found, prev Value overriden!",1);
                map->map[pos] = 0;
                map->p_orient = 0;
                map->p_pos_x = i;
                map->p_pos_y = (pos+1)/map->map_x;
            }
            else if(content[i] == 'W')
            {
                if(map->p_pos_x != -1)
                    log_string("Addional Player location found, prev Value overriden!",1);
                map->map[pos] = 0;
                map->p_orient = PI;
                map->p_pos_x = i;
                map->p_pos_y = (pos+1)/map->map_x;
            }
            else{
                map->map[pos] = -1;
            }
            i++;
            pos++;
        }
        while(i++ < map->map_x) //emulate spaces for rectangle shape
            map->map[pos++] = 2;
        lst_line = lst_line->next;
    }
}

//will finish reading the map and get map
void store_map_array(map_t* map,char *line,int fd)
{
    //figure out dimensions of the map, generate list of lines
    t_list* lst_line = NULL;
    int tempx;
    while(line)
    {
        tempx = (int)ft_strlen((const char*)line);
        if(tempx == 1) //stop as soon as a line is empty
        {
            free(line);
            break;
        }//
        if(line[tempx - 1] == '\n') //removing 1 if last character is new line
            tempx--;
        if(tempx > map->map_x)
            map->map_x = tempx;

        ft_lstadd_back(&lst_line,ft_lstnew(line));
        map->map_y++;
        line = get_next_line(fd);
    }
    map->map_dim = map->map_x * map->map_y;

    fill_map(map,lst_line);
    display_map_data(map);
    ft_lstclear(&lst_line,free);
}

enum map_type_id identify_line(char *line)
{
    if(line == NULL)
        return MP_ERR;
    size_t len = ft_strlen((const char*)line);
    if(len < 2)
        return MP_ERR;
    if(!ft_strncmp(line,"NO",2) || !ft_strncmp(line,"SO",2)
        || !ft_strncmp(line,"WE",2) || !ft_strncmp(line,"EA",2))
        return MP_TEXT;
    else if(!ft_strncmp(line,"F",1) || !ft_strncmp(line,"C",1))
        return MP_COL;
    else if(ft_strchr(line,'1') != NULL || ft_strchr(line,'1') != NULL)
        return MP_MAP;

    return MP_ERR;
}

// only there for allocating a map and default initializing it
map_t* init_map(void)
{
    map_t* map = malloc(sizeof(map_t));
    map->text_north = NULL;
    map->text_south = NULL;
    map->text_east = NULL;
    map->text_west = NULL;
	map->texture_north = NULL;
	map->texture_south = NULL;
	map->texture_east = NULL;
	map->texture_west = NULL;
    map->col_ceil = 0; // black, but no transparency
    map->col_floor = 0; // black, but no transparency
    map->p_orient = -1;
    map->p_pos_x = -1;
    map->p_pos_y = -1;
    map->map = NULL;
    map->map_x = 0;
    map->map_y = 0;
    map->map_dim = 0;

    return map;

}

map_t* read_map(char *path)
{
    int fd = open(path,O_RDONLY);
    if(fd == -1)
        return NULL;
    //check if the format is .cub

    map_t* map = init_map();
    char *line;
    line = get_next_line(fd);
    enum map_type_id id;
    while(line)
    {
        id = identify_line(line);
        if(id == MP_TEXT)
            store_map_texture(map,line);
        else if(id == MP_COL)
            store_map_color(map,line);
        else if(id == MP_MAP) //will finish reading the map and get map
        {
            store_map_array(map,line,fd);
            break;
        }
        free(line);
        line = get_next_line(fd);
    }
    close(fd);
    if(!validate_map(map))
    {
        free_map(map);
        return NULL;
    }
	map->texture_north = mlx_load_png(map->text_north);
	map->texture_south = mlx_load_png(map->text_south);
	map->texture_east = mlx_load_png(map->text_east);
	map->texture_west = mlx_load_png(map->text_west);

    return map;
}

//retrieves the direct value of a maps->map given the xy coordinates
int get_grid_val(int x,int y, map_t* map)
{
    return map->map[y * map->map_x+ x];
}

//based on a grid checks if a given point(x,y) is adjacent to a wall in each cardial direction
int is_walled(int x, int y,map_t* map)
{
    int val = map->map[y * map->map_x + x];
    //printf("val = %d\n",val);
    if(val == 1)
    {
        //printf("IS WALL\n");
        return 1;
    }
    //initially checking for borders
    if(x == 0 || x+1 == map->map_x || y == 0 || y+1 == map->map_y)
        return 0;

    //need to check every element as map could have 'holes'
    if(get_grid_val(x+1,y,map) == 2 || get_grid_val(x-1,y,map) == 2
    || get_grid_val(x,y+1,map) == 2 || get_grid_val(x,y-1,map) == 2)
        return 0;
    return 1;
}


//returns 1 on success
// 0 on failure
int validate_map(map_t* map)
{
    int ret = 1;
    if(map->text_north == NULL || access(map->text_north,F_OK | R_OK)){
        log_string("No NORTH texture found",2);
        ret = 0;
    }
    if(map->text_south == NULL || access(map->text_south,F_OK | R_OK)){
        log_string("No SOUTH texture found",2);
        ret = 0;
    }
    if(map->text_east == NULL || access(map->text_east,F_OK | R_OK)){
        log_string("No EAST texture found",2);
        ret = 0;
    }
    if(map->text_west == NULL || access(map->text_west,F_OK | R_OK)){
        log_string("No WEST texture found",2);
        ret = 0;
    }

    if(map->col_ceil == 0){
        log_string("No Ceiling color found",2);
        ret = 0;
    }
    if(map->col_floor == 0){
        log_string("No Ceiling color found",2);
        ret = 0;
    }

    if(map->p_pos_x == -1){
        log_string("player Position was not set within the map",2);
        ret = 0;
    }

    int val;
    //check if each 0 has only 1 and 0 as adjacent positions
    for (int y = 0; y < map->map_y; y++)
    {
        for (int x = 0; x < map->map_x; x++)
        {
            val = map->map[y * map->map_x + x];
            if(val == 2)
                continue;
            //printf("val in loop= %d\n",val);
            if(val == -1 || !is_walled(x,y,map))
            {
                //printf("x,y = [%d][%d]\n",x,y);
                log_string("Map is impossible.",2);
                return 0;
            }
        }
    }
    return ret;
}

void free_map(map_t *map)
{
    free(map->text_north);
    free(map->text_south);
    free(map->text_east);
    free(map->text_west);

    free(map->texture_north->pixels);
    free(map->texture_north);
    free(map->texture_east->pixels);
    free(map->texture_east);
    free(map->texture_south->pixels);
    free(map->texture_south);
    free(map->texture_west->pixels);
    free(map->texture_west);

    free(map->map);
    free(map);
}
